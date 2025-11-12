#include "f4se_common/f4se_version.h"

#if _F4_RUNTIME_1_10_163
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163
#endif
#if _F4_RUNTIME_1_10_984
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_984
#endif
#if _F4_RUNTIME_1_11_137
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_11_137
#endif
#ifndef REQUIRED_RUNTIME
	#error Invalid project configuration
#endif