#include <wchar.h>
#include "xmlprint.h"


enum JobType { JT_NOTHING, JT_TEST1, JT_TEST2};

int loops = 1;
JobType job = JT_NOTHING;
SString *testFile = NULL;

void printError(){
  wprintf(
    L"\nUsage: xmlparser (command) (parameters)\n"
    L" Commands:\n"
    L"  -t<n>      Run the test number <n>\n"
    L" Parameters:\n"
    L"  -c<n>      Number of test runs\n"
    L"  -f<path>   Test file\n\n"
    L" Test:\n"
    L"   1         ColorPrintInConsole\n"
    L"   1         PrintInConsole\n"
    );
};

int init(int argc, wchar_t *argv[])
{
  for(int i = 1; i < argc; i++){
    if (argv[i][0] != L'-'){
      return -1;
    }
    if (argv[i][1] == L't'){
      if (argv[i][2]){
        job = (JobType)_wtoi(argv[i]+2);
      }
      else{
        return -1;
      }
      continue;
    }
    if (argv[i][1] == L'c'){
      if (argv[i][2]){
        loops =_wtoi(argv[i]+2);
      }
      if (!loops){
        loops=1;
      }
      continue;
    }
    if (argv[i][1] == L'f' && (i+1 < argc || argv[i][2])){
      if (argv[i][2]){
        testFile=new SString(DString(argv[i]+2));
      }else{
        testFile=new SString(DString(argv[i+1]));
        i++;
      }
      continue;
    }
    if (argv[i][1]) 
    {
      wprintf(L"WARNING: unknown option '-%s'\n", argv[i]+1);
      return -1;
    }
  }
  return 1;
}

int wmain(int argc, wchar_t *argv[])
{
  if ((argc<2)||(init(argc,argv)==-1)) 
  {
    printError();
    return 1;
  }
  try{
    switch (job){
    case JT_NOTHING:
      printError();
      break;
    case JT_TEST1:
      PrintInConsole(testFile,true);
      break;
    case JT_TEST2:
      PrintInConsole(testFile,false);
      break;
    }
  }catch(...){
    wprintf(L"Unknown exception\n");
    return 1;
  };
  return 0;
};
