#pragma once
// The Miles Sound System functions the game calls (mss.h of the Miles SDK, imported from mss32.dll).
struct _STREAM;
typedef _STREAM *HSTREAM;

extern "C" {
__declspec(dllimport) void __stdcall AIL_pause_stream(HSTREAM stream, long onoff);
__declspec(dllimport) void __stdcall AIL_close_stream(HSTREAM stream);
}

struct _DIG_DRIVER;
struct _MDI_DRIVER;
struct _DLSDEVICE;

extern "C" {
__declspec(dllimport) void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi, _DLSDEVICE **pdls);
__declspec(dllimport) long __stdcall AIL_digital_master_volume(_DIG_DRIVER *dig);
}

extern "C" {
__declspec(dllimport) HSTREAM __stdcall AIL_open_stream(_DIG_DRIVER *dig, const char *filename, long stream_mem);
__declspec(dllimport) char *__stdcall AIL_last_error();
__declspec(dllimport) long __stdcall AIL_auto_service_stream(HSTREAM stream, long onoff);
__declspec(dllimport) void __stdcall AIL_start_stream(HSTREAM stream);
__declspec(dllimport) void __stdcall AIL_set_stream_loop_count(HSTREAM stream, long count);
}
