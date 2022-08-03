// address of temporary array is ill-formed in c++

#include <cstring>

#ifdef av_err2str
#undef av_err2str

av_always_inline char* av_err2str(int errnum)
{
  static char str[AV_ERROR_MAX_STRING_SIZE];
  std::memset(str, 0, sizeof(str));
  return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#else
#endif

#ifdef av_ts2str
#undef av_ts2str

av_always_inline char* av_ts2str(int64_t ts)
{
  static char str[AV_TS_MAX_STRING_SIZE];
  std::memset(str, 0, sizeof(str));
  return av_ts_make_string(str, ts);
}
#else
#error 'utils/libav_string_fix.hpp' Must be included after 'libav/timestamp.h'
#endif

#ifdef av_ts2timestr
#undef av_ts2timestr

av_always_inline char* av_ts2timestr(int64_t ts, AVRational* tb)
{
  static char str[AV_TS_MAX_STRING_SIZE];
  std::memset(str, 0, sizeof(str));
  return av_ts_make_time_string(str, ts, tb);
}
#else
#error 'utils/libav_string_fix.hpp' Must be included after 'libav/timestamp.h'
#endif
