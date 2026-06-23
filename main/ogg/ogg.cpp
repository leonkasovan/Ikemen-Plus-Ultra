
#include <windows.h>
#include <locale.h>
#include <process.h>
#include <stdint.h>

#include "vorbis/vorbisfile.h"

#include "sszdef.h"
#include "typeid.h"
#include "arrayandref.hpp"
#include "pluginutil.hpp"
#include "mem_profiler.hpp"


class OggVorbis
{
	OggVorbis_File _vf;
	FILE* _fh;
	MEMBER void fileClose()
	{
		if (_fh) fclose(_fh);
		_fh = nullptr;
	}
public:
	MEMBER OggVorbis()
	{
		memset(&_vf, 0, sizeof(_vf));
		_fh = nullptr;
	}
	MEMBER ~OggVorbis()
	{
		clear();
	}
	MEMBER bool open(std::WSTR file)
	{
		clear();
		_wfopen_s(&_fh, file.c_str(), L("rb"));
		if (!_fh) return false;
		if (ov_open(_fh, &_vf, nullptr, 0) < 0) {
			fileClose();
			return false;
		}
		return true;
	}
	MEMBER void clear()
	{
		if (!_fh) return;
		ov_clear(&_vf);
		_fh = nullptr;
	}
	MEMBER int64_t pcmTotal()
	{
		return ov_pcm_total(&_vf, -1);
	}
	MEMBER int32_t channels()
	{
		auto nc = ov_info(&_vf, -1);
		return nc ? nc->channels : -1;
	}
	MEMBER int32_t rate()
	{
		auto nc = ov_info(&_vf, -1);
		return nc ? nc->rate : -1;
	}
	MEMBER intptr_t read(int16_t* buffer, intptr_t length)
	{
		int current_section;
		auto rlen =
			ov_read(&_vf, (char*)buffer, length * 2, 0, 2, 1, &current_section);
		if (rlen > 0) rlen /= 2;
		return rlen;
	}
	MEMBER int32_t seek(double time)
	{
		return ov_time_seek(&_vf, time);
	}
};

TUserFunc(OggVorbis*, NewOggVorbis)
{
	return new OggVorbis;
}

TUserFunc(void, DeleteOggVorbis, OggVorbis* ov)
{
	delete ov;
}

TUserFunc(bool, OggVorbisOpen, Reference file, OggVorbis* ov)
{
	std::string fname = pu->refToAstr(CP_THREAD_ACP, file);
	MEM_MARK_BEFORE_NAMED(MUSIC, fname.c_str());
	bool result = ov->open(pu->refToWstr(file));
	MEM_MARK_AFTER_NAMED(MUSIC, fname.c_str());
	return result;
}

TUserFunc(void, OggVorbisClear, OggVorbis* ov)
{
	ov->clear();
}

TUserFunc(int64_t, OggVorbisPcmTotal, OggVorbis* ov)
{
	return ov->pcmTotal();
}

TUserFunc(int32_t, OggVorbisChannels, OggVorbis* ov)
{
	return ov->channels();
}

TUserFunc(int32_t, OggVorbisRate, OggVorbis* ov)
{
	return ov->rate();
}

TUserFunc(intptr_t, OggVorbisRead, Reference buffer, OggVorbis* ov)
{
	if(buffer.len() == 0) return 0;
	return ov->read((int16_t*)buffer.atpos(), buffer.len()/sizeof(int16_t));
}

TUserFunc(int32_t, OggVorbisSeek, double time, OggVorbis* ov)
{
	return ov->seek(time);
}

