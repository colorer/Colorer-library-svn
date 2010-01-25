#include "tests.h"
#include "SanTimer.h"
#include<iostream>
using namespace std;

/* 
*  speed test a class ParserFactory constructor
*  search and reading catalog.xml
*/
void TestParserFactoryConstructor(int count, SString *catalogPath)
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

/* 
*  speed test a class ParserFactory->getHRCParser
*  load proto.hrc 
*/
void TestParserFactoryHRCParser(int count, SString *catalogPath)
{
  double all_time = 0, t;
  for (int i=1; i<=count; i++)
  {
    ParserFactory *parserFactoryLocal = NULL;
    parserFactoryLocal = new ParserFactory(catalogPath);
    // start timer
    CTimer timer(true);
    HRCParser *hrcParserLocal = NULL;
    hrcParserLocal = parserFactoryLocal->getHRCParser();
    // stop timer
    t = timer.StopGet();
    all_time = all_time + t;
    cout<<t<<endl;
    delete parserFactoryLocal;
  }
  cout<< "the average time for "<<count<<" tests "<<all_time/count << " sec."; 
}

/* 
*  speed test a class ParserFactory->createStyledMapper
*  load default hrd scheme
*/
void TestParserFactoryStyledMapper(int count, SString *catalogPath)
{
  double all_time = 0, t;
  for (int i=1; i<=count; i++)
  {
    ParserFactory *parserFactoryLocal = NULL;
    parserFactoryLocal = new ParserFactory(catalogPath);
    // start timer
    CTimer timer(true);
    RegionMapper *regionMapperLocal = NULL;
    regionMapperLocal = parserFactoryLocal->createStyledMapper(&DString("console"), NULL);

    // stop timer
    t = timer.StopGet();
    all_time = all_time + t;
    cout<<t<<endl;
    delete regionMapperLocal;
    delete parserFactoryLocal;
  }
  cout<< "the average time for "<<count<<" tests "<<all_time/count << " sec."; 
}