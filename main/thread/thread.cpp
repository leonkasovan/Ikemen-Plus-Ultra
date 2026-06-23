
#include "sszdef.h"

#include "typeid.h"
#include "arrayandref.hpp"
#include "pluginutil.hpp"


TUserFunc(void, ThreadDelay, uint32_t ui)
{
	Sleep(ui);
}
