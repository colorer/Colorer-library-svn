/*
   Colorer syntax engine - Legacy colorer-take5 beta4 logging, should be REMOVED.

   Copyright (C) 1996, 1997 the Free Software Foundation

   Authors: 2006 Igor Russkih <irusskih at gmail dot com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

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
