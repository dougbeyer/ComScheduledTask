// UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>


#include <string>
#include <iostream>
//#include <fstream>

#ifdef UNICODE
  #define CIN       std::wcin
  #define COUT      std::wcout
  #define STRING    std::wstring
  //#define IFSTREAM  std::wifstream
  //#define OFSTREAM  std::wofstream
#else
  #define CIN       std::cin
  #define COUT      std::cout
  #define STRING    std::string
  //#define IFSTREAM  std::ifstream
  //#define OFSTREAM  std::ofstream
#endif



static void pause()
  {
  _tprintf( _T( "Hit enter to exit...\n" ) );

  // Causes all previously entered chars to be ignored; of particular conern are any '\n' chars
  // which would cause the wcin.getline() call to return immediately.
  CIN.sync();

  STRING str;
  getline( CIN, str );
  }


int main()
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener(&result);

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener(&progress);

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;

  //runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );  // Default registry.
  runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry("TaskSchedulerUtilTests").makeTest());  // Using named registries to control execution order.

  runner.run(controller);

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
  outputter.write();

  pause();

  return result.wasSuccessful() ? 0 : 1;
}





