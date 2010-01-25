#include "tests.h"
#include "SanTimer.h"
#include<iostream>
using namespace std;


void TestParserFactory(int count, SString *catalogPath)
{
  double all_time = 0, t;
  for (int i=1; i<=count; i++)
  {
    // start timer
    CTimer timer(true); 
    ParserFactory *parserFactoryLocal = NULL;
    parserFactoryLocal = new ParserFactory(catalogPath);
    // stop timer
    t = timer.StopGet();
    all_time = all_time + t;
    cout<<t<<endl;
    delete parserFactoryLocal;
  }
  cout<< "the average time for "<<count<<" tests "<<all_time/count << " sec."; 
}
