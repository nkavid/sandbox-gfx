// address of temporary array is ill-formed in c++

#include <array>

#ifdef av_err2str
#undef av_err2str

av_always_inline char* av_err2str(int errnum)
{
  static std::array<char, AV_ERROR_MAX_STRING_SIZE> str;
  str.fill(0);
  return av_make_error_string(str.data(), str.size(), errnum);
}
#else
#error 'utils/libav_string_fix.hpp' must be included after 'libavutil/error.h'
#endif

#ifdef av_ts2str
#undef av_ts2str

av_always_inline char* av_ts2str(int64_t timestamp)
{
  static std::array<char, AV_TS_MAX_STRING_SIZE> str;
  str.fill(0);
  return av_ts_make_string(str.data(), timestamp);
}
#else
#error 'utils/libav_string_fix.hpp' must be included after 'libavutil/timestamp.h'
#endif

#ifdef av_ts2timestr
#undef av_ts2timestr

av_always_inline char* av_ts2timestr(int64_t timestamp, AVRational* timebase)
{
  static std::array<char, AV_TS_MAX_STRING_SIZE> str;
  str.fill(0);
  return av_ts_make_time_string(str.data(), timestamp, timebase);
}
#else
#error 'utils/libav_string_fix.hpp' must be included after 'libavutil/timestamp.h'
#endif
