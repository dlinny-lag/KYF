#pragma once
#include <algorithm>
#include <unordered_map>
#include "f4se/GameData.h"
#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "PluginAPIExport.hpp"
#include "SerializationHelper.h"
#include <sstream>
#include <iomanip>
#include <ios>

namespace AttributesManager
{
	struct AttributeEntry
	{
		ActorValueInfo* AV;
		BSFixedString DefaultName;
		SInt32 Index;
		bool ShowBase;
		UInt32 Precision;
		void Dump() const
		{
			_MESSAGE(" %d. AV:%08X, DefName=%s", Index, AV->formID, DefaultName.c_str());
		}
	};

	struct Group
	{
		BGSKeyword* Id;
		SInt32 Priority;
		bool Visible;
		std::vector<AttributeEntry> Attributes;
		void Dump() const
		{
			_MESSAGE("%d. Group:%08X, %s", Priority, Id->formID, Visible?"Visible":"Invisible");
			for (const auto& a:Attributes)
			{
				a.Dump();
			}
		}
	};

	struct AttributeEntryLess
	{
		bool operator()(const AttributeEntry& a, const AttributeEntry& b) const
		{
			return a.Index < b.Index;
		}
	};

	struct AttributeAVEqual
	{
		explicit AttributeAVEqual(ActorValueInfo* sample) : my(sample)
		{
		}
		bool operator()(const AttributeEntry& other) const
		{
			return comparer(my, other.AV);
		}
	private:
		ActorValueInfo* my;
		std::equal_to<ActorValueInfo*> comparer;
	};
	struct GroupIdEqual
	{
		explicit GroupIdEqual(BGSKeyword* sample) : my(sample)
		{
		}
		bool operator()(const Group& other) const
		{
			return comparer(my, other.Id);
		}
	private:
		BGSKeyword* my;
		std::equal_to<BGSKeyword*> comparer;
	};

	struct GroupLess
	{
		bool operator()(const Group& a, const Group& b) const
		{
			return a.Priority < b.Priority;
		}
	};

	std::vector<Group> groups;
	BSReadWriteLock storageLock;

	constexpr UInt32 rootTag = 'KYFA';
	constexpr UInt32 currentDataStructureVersion = 1;

	bool Save(const F4SESerializationInterface* serializer)
	{
		if (!serializer->OpenRecord(rootTag, currentDataStructureVersion))
		{
			_WARNING("Failed to open record (%.4s)", &rootTag);
			return false;
		}

		const UInt32 groupsCount = groups.size();
		if (!serializer->WriteRecordData(&groupsCount, sizeof(groupsCount)))
		{
			return false;
		}
		for(const auto& g:groups)
		{
			serializer->WriteRecordData(&g.Id->formID, sizeof(g.Id->formID));
			serializer->WriteRecordData(&g.Priority, sizeof(g.Priority));
			serializer->WriteRecordData(&g.Visible, sizeof(g.Visible));
			const UInt32 attributesCount = g.Attributes.size();

			serializer->WriteRecordData(&attributesCount, sizeof(attributesCount));
			for(const auto& a : g.Attributes)
			{
				serializer->WriteRecordData(&a.AV->formID, sizeof(a.AV->formID));
				SerializationHelper::WriteString(serializer, &a.DefaultName);
				serializer->WriteRecordData(&a.Index, sizeof(a.Index));
				serializer->WriteRecordData(&a.ShowBase, sizeof(a.ShowBase));
				serializer->WriteRecordData(&a.Precision, sizeof(a.Precision));
			}
		}

		return true;
	}

	bool Load(const F4SESerializationInterface* serializer)
	{
		UInt32 recType, length, curVersion;
		if (!serializer->GetNextRecordInfo(&recType, &curVersion, &length))
		{
			_WARNING("Failed to get record");
			return false;
		}
		if (recType != rootTag)
		{
			_WARNING("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
			return false;
		}
		if (curVersion != currentDataStructureVersion)
		{ // TODO: handle it in future
			_WARNING("Unexpected version %d. Expected %d", curVersion, currentDataStructureVersion);
			return false;
		}

		UInt32 groupsCount;
		if (!serializer->ReadRecordData(&groupsCount, sizeof(groupsCount)))
			return false;

		groups.reserve(groupsCount);
		for (UInt32 i = 0; i < groupsCount; i++)
		{
			Group g;
			UInt32 savedGroupId;
			if (!serializer->ReadRecordData(&savedGroupId, sizeof(savedGroupId)))
				return false;
			if (!serializer->ReadRecordData(&g.Priority, sizeof(g.Priority)))
				return false;
			if (!serializer->ReadRecordData(&g.Visible, sizeof(g.Visible)))
				return false;

			UInt32 attributesCount;
			if (!serializer->ReadRecordData(&attributesCount, sizeof(attributesCount)))
				return false;
			g.Attributes.reserve(attributesCount);

			for (UInt32 j = 0; j < attributesCount; j++)
			{
				UInt32 savedAVId;
				if (!serializer->ReadRecordData(&savedAVId, sizeof(savedAVId)))
					return false;
				AttributeEntry a;
				if (!SerializationHelper::ReadString(serializer, &a.DefaultName))
					return false;
				if (!serializer->ReadRecordData(&a.Index, sizeof(a.Index)))
					return false;
				if (!serializer->ReadRecordData(&a.ShowBase, sizeof(a.ShowBase)))
					return false;
				if (!serializer->ReadRecordData(&a.Precision, sizeof(a.Precision)))
					return false;

				UInt32 actualAvId;
				if (!serializer->ResolveFormId(savedAVId, &actualAvId))
				{
					_WARNING("Can't resolve AV Id %08X", savedAVId);
					continue; // ActorValue disappeared
				}

				ActorValueInfo* av = DYNAMIC_CAST(LookupFormByID(actualAvId), TESForm, ActorValueInfo);
				if (!av)
				{
					_WARNING("AV %08X not found", actualAvId);
					continue; // ActorValue disappeared
				}
				a.AV = av;
				g.Attributes.push_back(a);
			}

			UInt32 actualGroupId;
			if (!serializer->ResolveFormId(savedGroupId, &actualGroupId))
			{
				_WARNING("Can't resolve group Id %08X", savedGroupId);
				continue; // group disappeared
			}
			BGSKeyword* groupId = DYNAMIC_CAST(LookupFormByID(actualGroupId), TESForm, BGSKeyword);
			if (!groupId)
			{
				_WARNING("Group %08X not found", actualGroupId);
				continue; // group disappeared
			}
			g.Id = groupId;

			if (!g.Attributes.empty())
				groups.push_back(g);
			else
				_WARNING("All attributes in group %08X was disappeared", actualGroupId);
		}
		std::sort(groups.begin(), groups.end(), GroupLess());
		return true;
	}

	void Clear()
	{
		groups.clear();
	}

	bool RegisterGroup(StaticFunctionTag* _, BGSKeyword* id, SInt32 priority)
	{
		BSWriteLocker lock(&storageLock);
		const auto f = std::find_if(groups.begin(),groups.end(), GroupIdEqual(id));
		if (f != groups.end())
			return false;

		const Group gr{id, priority, false};
		groups.emplace_back(gr);
		std::sort(groups.begin(), groups.end(), GroupLess());
		return true;
	}
	bool UnregisterGroup(StaticFunctionTag* _, BGSKeyword* id)
	{
		if (!id)
			return false;
		BSWriteLocker lock(&storageLock);
		for(auto iter = groups.begin(); iter != groups.end(); ++iter)
		{
			if (iter->Id == id)
			{
				groups.erase(iter);
				return true;
			}
		}
		
		return false;
	}
	
	bool RegisterAttribute(StaticFunctionTag* _, ActorValueInfo* av, BGSKeyword* groupId, SInt32 indexInGroup, bool showBaseValue, UInt32 precision, BSFixedString defaultName)
	{
		if (!av)
			return false;
		BSWriteLocker lock(&storageLock);
		const auto f = std::find_if(groups.begin(),groups.end(), GroupIdEqual(groupId));
		if (f == groups.end())
			return false;

		const AttributeEntry ae {av, defaultName, indexInGroup, showBaseValue, precision};
		f->Attributes.emplace_back(ae);
		std::sort(f->Attributes.begin(), f->Attributes.end(), AttributeEntryLess());

		return true;
	};

	bool UnregisterAttribute(StaticFunctionTag* _, ActorValueInfo* av, BGSKeyword* groupId)
	{
		if (!av)
			return false;
		BSWriteLocker lock(&storageLock);
		const auto g = std::find_if(groups.begin(),groups.end(), GroupIdEqual(groupId));
		if (g == groups.end())
			return false;

		const auto a = std::find_if(g->Attributes.begin(), g->Attributes.end(), AttributeAVEqual(av));
		if (a == g->Attributes.end())
			return false;
		g->Attributes.erase(a);

		return true;
	}

	VMArray<BGSKeyword*> GetGroups(StaticFunctionTag* _)
	{
		VMArray<BGSKeyword*> retVal;
		BSReadLocker lock(&storageLock);
		for(auto& group : groups)
		{
			retVal.Push(&group.Id);
		}
		return retVal;
	}
	
	VMArray<ActorValueInfo*> GetAttributes(StaticFunctionTag* _, BGSKeyword* groupId)
	{
		VMArray<ActorValueInfo*> retVal;
		retVal.SetNone(true);
		BSReadLocker lock(&storageLock);
		const auto g = std::find_if(groups.begin(),groups.end(), GroupIdEqual(groupId));
		if (g == groups.end())
			return retVal;
		retVal.SetNone(false);
		for(auto& a:g->Attributes)
		{
			retVal.Push(&a.AV);
		}
		return retVal;
	}

	bool SetGroupVisibility(StaticFunctionTag* _, BGSKeyword* groupId, bool visibility)
	{
		BSWriteLocker lock(&storageLock);
		const auto g = std::find_if(groups.begin(),groups.end(), GroupIdEqual(groupId));
		if (g == groups.end())
			return false;
		g->Visible = visibility;
		return true;
	}

	bool GetGroupVisibility(StaticFunctionTag* _, BGSKeyword* groupId)
	{
		BSReadLocker lock(&storageLock);
		const auto g = std::find_if(groups.begin(),groups.end(), GroupIdEqual(groupId));
		if (g == groups.end())
			return false;
		return g->Visible;
	}


	BSFixedString GetReportString(StaticFunctionTag* _, Actor* actor)
	{
		if (!actor)
			return BSFixedString();

		// build report
		std::stringstream result;
		result << std::fixed;
		BSReadLocker lock(&storageLock);
		for (const auto& group : groups)
		{
			if (!group.Visible)
				continue;
			for (const auto& attribute : group.Attributes)
			{
				result << std::setprecision(attribute.Precision);

				ActorValueInfo* av = attribute.AV;

				const char* name = av->fullName.name.c_str();
				if (!name || strlen(name) == 0)
					name = attribute.DefaultName.c_str();

				result << name << ": " ;

				result << actor->actorValueOwner.GetValue(av);
				if (attribute.ShowBase)
				{
					result << "/" << actor->actorValueOwner.GetBase(av);
				}
				result << "\n";
			}
		}

		return BSFixedString(result.str().c_str());
	}

	void Dump(StaticFunctionTag* _)
	{
		_MESSAGE("Dump:");
		for(const auto& g:groups)
		{
			g.Dump();
		}
	}

	bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction2("RegisterGroup", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, RegisterGroup, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "RegisterGroup", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterGroup", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, UnregisterGroup, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "UnregisterGroup", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction6("RegisterAttribute", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, RegisterAttribute, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "RegisterAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("UnregisterAttribute", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, UnregisterAttribute, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "UnregisterAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0("GetGroups", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, GetGroups, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "GetGroups", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("GetAttributes", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, GetAttributes, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "GetAttributes", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("SetGroupVisibility", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, SetGroupVisibility, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "SetGroupVisibility", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("GetGroupVisibility", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, GetGroupVisibility, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "GetGroupVisibility", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("GetReportString", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, GetReportString, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "GetReportString", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0("Dump", PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, Dump, vm));
		vm->SetFunctionFlags(PluginAPIExport::EXPORT_PAPYRUS_SCRIPT, "Dump", IFunction::kFunctionFlag_NoWait);

		return true;
	}
}
