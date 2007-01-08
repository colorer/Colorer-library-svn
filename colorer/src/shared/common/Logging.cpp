
#include<stdlib.h>
#include<string.h>
#include<common/Logging.h>
#include<unicode/String.h>
#include<common/Vector.h>

/**

Environment variables:

COLORER5LOGGING = QUIET|ERROR|WARN|TRACE|INFO
    Logging level

COLORER5LOGGINGTO = OUT|ERR|FILE
    To stdout, stderr, or file

COLORER5LOGGINGON = Component1,Component2
    Component names to enable logging for

COLORER5LOGGINGOFF = Component1,Component2
    Component names to disable logging for


ParserFactory, BaseEditor, TextParserImpl

BaseEditorNative, NSC:JHRCParser:getRegion, JavaLineSource

*/

static const char *levelNames[] = {"QUIET", "ERROR", "WARN", "TRACE", "INFO" };

static int envLevel = -1;

static Vector<char*> *envLoggingNamesOn = null;
static Vector<char*> *envLoggingNamesOff = null;


static FILE *log = 0;


static void file_logger(int level, const char *cname, const char *msg, va_list v) {

    int idx = 0;

    while (log == 0 && idx < 10) {
        char log_name[30];
#ifdef __unix__
        sprintf(log_name, "/tmp/colorer-logging-%d.log", idx);
#else
        sprintf(log_name, "c:\\colorer-logging-%d.log", idx);
#endif
        log = fopen(log_name, "ab+");
        idx++;
    }

    fprintf(log, "[%s][%s] ", levelNames[level], cname);

    vfprintf(log, msg, v);

    fprintf(log, "\n");

    fflush(log);
}

static void console_logger(int level, const char *cname, const char *msg, va_list v){

    fprintf(stderr, "[%s][%s] ", levelNames[level], cname);

    vfprintf(stderr, msg, v);

    fprintf(stderr, "\n");

    fflush(stderr);
}


void colorer_logger_set_target(const char *logfile){
    if (logfile == 0) {
        log = stderr;
        return;
    }
    if (log == stderr || log == stdout) return;
    
    if (log != 0) {
        fclose(log);
    }

    log = fopen(logfile, "ab+");
}



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



void colorer_logger(int level, const char *cname, const char *msg, va_list v){

  
  if (envLevel == -1) {
      
      char *envLevelChar = getenv("COLORER5LOGGING");
      envLevel = 0;
      
      while(stricmp(levelNames[envLevel], envLevelChar) != 0 ) {
          envLevel++;
          if (envLevel == COLORER_FEATURE_LOGLEVEL_FULL) {
              break;
          }
      } 
      
      char *envLoggingChar = getenv("COLORER5LOGGINGON");
      envLoggingNamesOn = new Vector<char*>;

      while (envLoggingChar && *envLoggingChar) {
          envLoggingNamesOn->addElement(envLoggingChar);
          
          envLoggingChar = strchr(envLoggingChar, ',');
          if (envLoggingChar != null) envLoggingChar++;
      }

      envLoggingChar = getenv("COLORER5LOGGINGOFF");
      envLoggingNamesOff = new Vector<char*>;

      while (envLoggingChar && *envLoggingChar) {
          envLoggingNamesOff->addElement(envLoggingChar);
          
          envLoggingChar = strchr(envLoggingChar, ',');
          if (envLoggingChar != null) envLoggingChar++;
      }

      char *envLogTo = getenv("COLORER5LOGGINGTO");

      if (envLogTo && !stricmp(envLogTo, "ERR")) log = stderr;
      if (envLogTo && !stricmp(envLogTo, "OUT")) log = stdout;

  }

  /* Check for environment level */
  if (level > envLevel) return;

  
  if (cname != null) {
      int idx;
      bool foundOn = true;
      bool foundOff = false;

      size_t cnamelen = strlen(cname);

      for (idx = 0; idx < envLoggingNamesOn->size(); idx++) {
          if (strnicmp(envLoggingNamesOn->elementAt(idx), cname, cnamelen) == 0 &&
             (envLoggingNamesOn->elementAt(idx)[cnamelen] == ',' || envLoggingNamesOn->elementAt(idx)[cnamelen] == 0) ) {
              foundOn = true;
              break;
          } else {
              foundOn = false;
          }
      }

      for (idx = 0; idx < envLoggingNamesOff->size(); idx++) {
          if (strnicmp(envLoggingNamesOff->elementAt(idx), cname, cnamelen) == 0 &&
             (envLoggingNamesOff->elementAt(idx)[cnamelen] == ',' || envLoggingNamesOff->elementAt(idx)[cnamelen] == 0) ) {
              foundOff = true;
              break;
          }
      }

      if (!foundOn || foundOff) {
          return;
      }
  }

  file_logger(level, cname, msg, v);
}



/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <cail@nm.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
