
#include<stdio.h>
#include<colorer/viewer/ConsoleTools.h>

/** Internal run action type */
enum { JT_NOTHING, JT_REGTEST,
       JT_LIST_LOAD, JT_LIST_TYPES,
       JT_VIEW, JT_GEN, JT_FORWARD } jobType;

/** Reads and parse command line */
void init(ConsoleTools &ct, int argc, char*argv[]){

  fprintf(stderr, "\ncolorer-take5 library\n");
  fprintf(stderr, "copyright (c) 1999-2003 cail lomecb\n\n");

  for(int i = 1; i < argc; i++){
    if (argv[i][0] != '-'){
      ct.setInputFileName(DString(argv[i]));
      continue;
    };
    if (argv[i][1] == 'r') jobType = JT_REGTEST;
    if (argv[i][1] == 'l') jobType = JT_LIST_TYPES;
    if (argv[i][1] == 'l' && argv[i][2] == 'l') jobType = JT_LIST_LOAD;
    if (argv[i][1] == 'f') jobType = JT_FORWARD;
    if (argv[i][1] == 'v') jobType = JT_VIEW;
    if (argv[i][1] == 'h') jobType = JT_GEN;

    if (argv[i][1] == 'd' && argv[i][2] == 'c') ct.setCopyrightHeader(false);
    if (argv[i][1] == 'd' && argv[i][2] == 'b') ct.setBomOutput(false);
    if (argv[i][1] == 'd' && argv[i][2] == 's') ct.setHtmlEscaping(false);
    if (argv[i][1] == 'd' && argv[i][2] == 'h') ct.setHtmlWrapping(false);

    if (argv[i][1] == 't' && (i+1 < argc || argv[i][2])){
      if (argv[i][2]){
        ct.setTypeDescription(DString(argv[i]+2));
      }else{
        ct.setTypeDescription(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1] == 'o' && (i+1 < argc || argv[i][2])){
      if (argv[i][2]){
        ct.setOutputFileName(DString(argv[i]+2));
      }else{
        ct.setOutputFileName(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1] == 'i' && (i+1 < argc || argv[i][2])){
      if (argv[i][2]){
        ct.setHRDName(DString(argv[i]+2));
      }else{
        ct.setHRDName(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1] == 'c' && (i+1 < argc || argv[i][2])){
      if (argv[i][2]){
        ct.setCatalogPath(DString(argv[i]+2));
      }else{
        ct.setCatalogPath(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1] == 'e' && argv[i][2] == 'i' && (i+1 < argc || argv[i][3])){
      if (argv[i][3]){
        ct.setInputEncoding(DString(argv[i]+3));
      }else{
        ct.setInputEncoding(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1] == 'e' && argv[i][2] == 'o' && (i+1 < argc || argv[i][3])){
      if (argv[i][3]){
        ct.setOutputEncoding(DString(argv[i]+3));
      }else{
        ct.setOutputEncoding(DString(argv[i+1]));
        i++;
      };
      continue;
    };
    if (argv[i][1]) fprintf(stderr, "\nUnknown option: %s\n", argv[i]+1);
  };
};

/** Prints usage. */
void printError(){
  fprintf(stderr,
       "use: colorer -(command) (parameters)  [<filename>]\n"
       " commands:\n"
       "  -l         lists all avaiable types\n"
       "  -ll        lists and loads full hrc database\n"
       "  -r         regexp tests\n"
       "  -h         generates plain coloring from <filename> (uses 'rgb' hrd class)\n"
       "  -v         view file <fname> (uses 'console' hrd class)\n"
       "  -f         forward input file to output with selected encodings\n"
       " parameters:\n"
       "  -c<path>   use specified catalog.xml file\n"
       "  -i<name>   loads specified hrd rules from catalog\n"
       "  -t<type>   try to use type <type> instead of type autodetection\n"
       "  -ei<name>  use input file encoding <name>\n"
       "  -eo<name>  use output stream encoding <name>, also viewer encoding in w9x\n"
       "  -o<name>   use file <name> as output stream\n"
       "  -db        disable BOM(ZWNBSP) symbol output in unicode encodings\n"
       "  -dc        disable information header in generator's output\n"
       "  -ds        disable html symbols substitutions in generator's output\n"
       "  -dh        disable html header and footer output\n"
  );
};

/** Creates ConsoleTools class instance and runs it.
*/
int main(int argc, char *argv[])
{
  ConsoleTools ct;
  init(ct, argc, argv);
  try{
    switch(jobType){
      case JT_REGTEST:
        ct.RETest();
        break;
      case JT_LIST_LOAD:
        ct.listTypes(true);
        break;
      case JT_LIST_TYPES:
        ct.listTypes(false);
        break;
      case JT_VIEW:
        ct.viewFile();
        break;
      case JT_GEN:
        ct.genOutput();
        break;
      case JT_FORWARD:
        ct.forward();
        break;
      default:
        printError();
        break;
    };
  }catch(Exception e){
    fprintf(stderr, e.getMessage()->getChars());
    return -1;
  };
  return 0;
};

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
 * The Original Code is the Colorer Library
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <ruiv@uic.nnov.ru>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2003
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