
#include "../../lib/dll/ssz/ssz/sszdef.h"
#include "../../lib/dll/ssz/ssz/commandline.hpp"

void* SSZ_STDCALL MemoryKakuho(intptr_t size)
{
	return new int8_t[size];
}
void SSZ_STDCALL MemoryKaihou(void *p)
{
	delete [] (int8_t*)p;
}

void* (SSZ_STDCALL *const sszrefnewfunc)(intptr_t) = MemoryKakuho;
void (SSZ_STDCALL *const sszrefdeletefunc)(void*) = MemoryKaihou;


#include "../../lib/dll/ssz/ssz/typeid.h"

#include "../../lib/dll/ssz/ssz/arrayandref.hpp"

#define SSZ_CORE
#include "../../lib/dll/ssz/ssz/pluginutil.hpp"
#undef SSZ_CORE

// Declare the exported Run function so it can be linked directly
extern "C" bool SSZ_STDCALL Run(PluginUtil* pu, Reference r);

#ifndef _WIN32
#include <dlfcn.h>

static HMODULE LoadLibrary(const WCHR *lpFileName)
{
	auto ret = dlopen(PluginUtil::wToA(lpFileName).c_str(), RTLD_NOW);
	if(!ret) fprintf(stderr, "%s\n", dlerror());
	return ret;
}
static FARPROC GetProcAddress(HMODULE hModule, const char *lpProcName)
{
	auto ret = dlsym(hModule, lpProcName);
	if(!ret) fprintf(stderr, "%s\n", dlerror());
	return ret;
}
static BOOL FreeLibrary(HMODULE hModule)
{
	return dlclose(hModule) == 0;
}

#endif

int
main(int argc, char *argv[])
{
#ifndef _WIN32
	setlocale(LC_CTYPE, "en_US.UTF-8");
#endif
	PluginUtil pu(nullptr, nullptr);//Dummy
	CommandLineString<WCHR> cmdline;
#ifdef _WIN32
	cmdline.set(GetCommandLine());
	SetDllDirectory(L"lib/external"); //Change dir where external dlls are loaded.
#else
	std::vector<std::WSTR> arg;
	while(argc--) arg.push_back(pu.aToW(*argv++));
	cmdline.swap(arg);
#endif
	Reference ref;
	ref.init();
	pu.wstrToRef(
		ref, cmdline.get().size() >= 2	? cmdline.get()[1] : L("main.ssz"));
	// Directly call the Run function instead of loading via GetProcAddress
	Run(&pu, ref);
	
	ref.releaseanddelete();
	return 0;
}
