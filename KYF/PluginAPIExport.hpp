#pragma once

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace PluginAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = "KYF:Internal";

	const char* pluginName = "KYFNative";
	const UInt32 pluginUID = 'KYFN';
	const UInt32 pluginVersionInt = 0x0050;
	const char* pluginVersionString = "0.5.0";

	BSFixedString GetVersionString(StaticFunctionTag* _)
	{
		return pluginVersionString;
	};

	UInt32 GetVersionInt(StaticFunctionTag* _)
	{
		return pluginVersionInt;
	};

	bool Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, BSFixedString>("GetVersionString", EXPORT_PAPYRUS_SCRIPT, GetVersionString, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionString", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("GetVersionInt", EXPORT_PAPYRUS_SCRIPT, GetVersionInt, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetVersionInt", IFunction::kFunctionFlag_NoWait);
		
		return true;
	}
}