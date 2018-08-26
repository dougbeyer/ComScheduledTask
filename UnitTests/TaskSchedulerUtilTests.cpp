/*
Overview:
  This file provides the unit test fixture implementation.

Created By: Doug Beyer, 08/26/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <time.h>
#include "TaskSchedulerUtilTests.h"

// Including the cpp file is good because it gives the unit tests access to the non-public members. Be sure to mark
// the cpp source files that are copied into the unit test project as "excluded from build".
#include "TaskSchedulerUtil.cpp"


using namespace test;
using namespace cofense;


namespace
    {

    }


///////////////////////////////////////////////////////////////////////////////////////////////////
//  TaskSchedulerUtilTests implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

// Register the test fixture. I recommend using the named registry so that execution order can be
//    controlled. See the main program for where the named registry entries get added to the test suite.
//CPPUNIT_TEST_SUITE_REGISTRATION( TaskSchedulerUtilTests );  // Default registry.
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TaskSchedulerUtilTests, "TaskSchedulerUtilTests" );  // Named registry.



void TaskSchedulerUtilTests::setUp()
  {

  }


void TaskSchedulerUtilTests::tearDown()
  {

  }


///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
    {
    const _TCHAR * TASK_NAME    = _T( "TaskSchedulerUtilTests Task" );
    const _TCHAR * AUTHOR_NAME  = _T( "Some Author Name" );

    // The path to Notepad.exe is the same on all Windows versions.
    const _TCHAR * NOTEPAD_PATH = _T( "C:\\Windows\\Notepad.exe" );


    bool getNowPlus                 ( unsigned int numSecondsToAdd, struct tm & theTime );
    bool getNowPlus                 ( unsigned int numSecondsToAdd, TDateTime & theTime );
    void convertStructTmToTDateTime ( struct tm & cTime, TDateTime & theTime );
    }



///////////////////////////////////////////////////////////////////////////////////////////////////
//  Unit tests.
///////////////////////////////////////////////////////////////////////////////////////////////////

void TaskSchedulerUtilTests::testConstruction()
  {
  // We want to ensure you can destroy the instance without having called init().
  TaskSchedulerUtil * pTsu = new TaskSchedulerUtil();
  delete pTsu;

  // Static construction/destruction.
  TaskSchedulerUtil tsu;
  }


void TaskSchedulerUtilTests::testInit()
  {
  // Clean construction, COM initialization, and COM deinitialization.
  TaskSchedulerUtil tsu;
  CPPUNIT_ASSERT( tsu.init() );

  // init multiple times.
  for ( unsigned int i = 0; i < 10; ++i )
    {
    CPPUNIT_ASSERT( tsu.init() );
    }
  };


void TaskSchedulerUtilTests::testCreateScheduledTask_LaunchExecutable()
  {
  TaskSchedulerUtil tsu;

  TDateTime startTime = {0};
  if ( !getNowPlus( 30, startTime ) )
    CPPUNIT_ASSERT( false );

  // Test attempting to create the task without first initializing COM.
  CPPUNIT_ASSERT( !tsu.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, startTime, NOTEPAD_PATH ) );

  // Initialize COM.
  CPPUNIT_ASSERT( tsu.init() );

  // **TODO**: DSB, 08/26/2018 - Not implemented yet. Verify task exists. Will need to add code to TaskSchedulerUtil
  // class to enumerate registered tasks.
  };


// **TODO**: DSB, 08/26/2018 - Not implemented yet. How to verify task executes??? Maybe I can create a task that copies
// a file from the test area. Then I'd only need to verify that the file got copied.




///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support function implementations.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  bool getNowPlus( unsigned int numSecondsToAdd, struct tm & theTime )
    {
    // Prep the output params.
    SecureZeroMemory( &theTime, sizeof( struct tm ) );

    __time64_t now = _time64( NULL );

    now += numSecondsToAdd;

    if ( 0 == _localtime64_s( &theTime, &now ) )
      return true;
    else
      {
      CPPUNIT_ASSERT( false );
      return false;
      }
    }


  bool getNowPlus( unsigned int numSecondsToAdd, TDateTime & theTime )
    {
    struct tm cTime = {0};
    if ( !getNowPlus( numSecondsToAdd, cTime ) )
      {
      CPPUNIT_ASSERT( false );
      return false;
      }

    convertStructTmToTDateTime( cTime, theTime );
    return true;
    }


  void convertStructTmToTDateTime( struct tm & cTime, TDateTime & theTime )
    {
    // Prep the output params.
    SecureZeroMemory( &theTime, sizeof( TDateTime ) );

    theTime.year    = (unsigned short)cTime.tm_year + 1900;
    theTime.month   = (unsigned char)cTime.tm_mon + 1;
    theTime.day     = (unsigned char)cTime.tm_mday;
    theTime.hours   = (unsigned char)cTime.tm_hour;
    theTime.minutes = (unsigned char)cTime.tm_min;
    theTime.seconds = (unsigned char)cTime.tm_sec;

    // Since we're dealing with local time, we can leave the UTC offsets at zero.
    }
  }
