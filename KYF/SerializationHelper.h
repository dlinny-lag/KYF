#pragma once
#include "f4se/Serialization.h"

namespace SerializationHelper
{
	bool ReadString(const F4SESerializationInterface* serializer, BSFixedString* str);

	bool WriteString(const F4SESerializationInterface* serializer, const BSFixedString* bstr);
	bool ResolveHandle(const F4SESerializationInterface* serializer, UInt64 handle, UInt64* handleOut);

};