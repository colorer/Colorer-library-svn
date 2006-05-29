
#include <common/Logging.h>

#if 0 // legacy logging API from beta4

#ifdef __cplusplus
extern "C"{
#endif

void _Z14colorer_loggeriPKcS0_Pc(int level, const char *cname, const char *msg, va_list v);

#define colorer_logger _Z14colorer_loggeriPKcS0_Pc

void colorer_logger_error(const char *cname, const char *msg, ...){
  va_list v;
  va_start(v, msg);
  colorer_logger(COLORER_FEATURE_LOGLEVEL_ERROR, cname, msg, v);
  va_end (v);
}

void colorer_logger_warn(const char *cname, const char *msg, ...){
  va_list v;
  va_start(v, msg);
  colorer_logger(COLORER_FEATURE_LOGLEVEL_WARN, cname, msg, v);
  va_end (v);
}
void colorer_logger_trace(const char *cname, const char *msg, ...){
  va_list v;
  va_start(v, msg);
  colorer_logger(COLORER_FEATURE_LOGLEVEL_TRACE, cname, msg, v);
  va_end (v);
}
void colorer_logger_info(const char *cname, const char *msg, ...){
  va_list v;
  va_start(v, msg);
  colorer_logger(COLORER_FEATURE_LOGLEVEL_INFO, cname, msg, v);
  va_end (v);
}

#undef colorer_logger

#ifdef __cplusplus
}
#endif


#endif
