#pragma once
#include "f4se/Serialization.h"
#include "SerializationHelper.h"

bool SerializationHelper::ReadString(const F4SESerializationInterface* serializer, BSFixedString* str)
{
	UInt32 len = 0;

	if (!serializer->ReadRecordData(&len, sizeof(UInt32)))
		return false;
	if (len == 0)
		return true;
	if (len > INT_MAX)
		return false;

	char* buf = new char[len + 1];
	buf[0] = 0;

	if (!serializer->ReadRecordData(buf, len)) 
	{
		delete[] buf;
		return false;
	}
	buf[len] = 0;

	*str = BSFixedString(buf);
	delete[] buf;
	return true;
}

bool SerializationHelper::WriteString(const F4SESerializationInterface* serializer, const BSFixedString* bstr)
{
	const char* str = bstr->c_str();
	size_t len = strlen(str);
	if (len > INT_MAX)
		return false;
	if (!serializer->WriteRecordData(&len, sizeof(UInt32)))
		return false;
	if (len == 0)
		return true;
	if (!serializer->WriteRecordData(str, len))
		return false;
	return true;
}

bool SerializationHelper::ResolveHandle(const F4SESerializationInterface* serializer, UInt64 handle, UInt64* handleOut)
{
	constexpr UInt64 hiBytesMask = 0xFFFFFFFF00000000;
	const UInt32 lowBytes = handle;
	UInt32 resolvedLowBytes;
	if (!serializer->ResolveFormId(lowBytes, &resolvedLowBytes))
		return false;

	*handleOut = (handle & hiBytesMask) | resolvedLowBytes;
	return true;
}
