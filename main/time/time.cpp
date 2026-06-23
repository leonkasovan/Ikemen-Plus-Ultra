
#include <time.h>

#include "sszdef.h"

#include "typeid.h"
#include "arrayandref.hpp"
#include "pluginutil.hpp"


TUserFunc(uint32_t, TickCount)
{
#ifdef _WIN32
	return timeGetTime();
#else
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return now.tv_sec * 1000 + now.tv_nsec / 1000000;
#endif
}

TUserFunc(int64_t, UnixTime)
{
	return time(nullptr);
}
