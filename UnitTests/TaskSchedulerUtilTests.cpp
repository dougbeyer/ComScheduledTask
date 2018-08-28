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
// the cpp source files that are copied into the unit test project as "excluded from build" if they are included here.
#include "TaskSchedulerUtil.cpp"


using namespace test;
using namespace cofense;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  const STRING TASK_NAME    ( _T( "TaskSchedulerUtilTests Task" ) );
  const STRING AUTHOR_NAME  ( _T( "Some Author Name" ) );

  // C:\\ProgramData\\TaskSchedulerUtilTest is created by post-build steps in the UnitTest VStuido project.
  //
  const STRING EXE_PATH     ( _T( "C:\\ProgramData\\TaskSchedulerUtilTests\\copy.bat" ) );
  const STRING WORKING_DIR  ( _T( "C:\\ProgramData\\TaskSchedulerUtilTests" ) );
  const STRING COPIED_FILE  ( _T( "C:\\ProgramData\\TaskSchedulerUtilTests\\dummy2.txt" ) );


  bool getNowPlus   ( unsigned int numSecondsToAdd, TDateTime & theTime );
  bool waitForFile  ( const STRING & fileName, DWORD timeoutInSeconds );
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
  CPPUNIT_ASSERT( Utils::deleteFile( COPIED_FILE ) );
  }


void TaskSchedulerUtilTests::tearDown()
  {
  CPPUNIT_ASSERT( Utils::deleteFile( COPIED_FILE ) );
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

  // init() called multiple times.
  for ( unsigned int i = 0; i < 10; ++i )
    {
    CPPUNIT_ASSERT( tsu.init() );
    }
  };


void TaskSchedulerUtilTests::testCreateScheduledTask_LaunchExecutable()
  {
  TaskSchedulerUtil tsu;

  // Test attempting to create the task without first initializing COM.
  CPPUNIT_ASSERT( !tsu.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, WORKING_DIR, EXE_PATH ) );

  // Initialize COM.
  CPPUNIT_ASSERT( tsu.init() );

  // Verify that the result file doesn't exist.
  CPPUNIT_ASSERT( !Utils::fileExists( COPIED_FILE ) );

  // Create the task.
  CPPUNIT_ASSERT( tsu.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, WORKING_DIR, EXE_PATH ) );

  // Verify that the result file does exist.
  CPPUNIT_ASSERT( waitForFile( COPIED_FILE, 5 ) );

  // Remove the scheduled task.
  CPPUNIT_ASSERT( tsu.deleteScheduledTask( TASK_NAME ) );
  };


void TaskSchedulerUtilTests::testDeleteScheduledTask()
  {
  TaskSchedulerUtil tsu;

  // Test attempting to delete the task without first initializing COM.
  CPPUNIT_ASSERT( !tsu.deleteScheduledTask( TASK_NAME ) );

  // Initialize COM.
  CPPUNIT_ASSERT( tsu.init() );

  // Create the task.
  CPPUNIT_ASSERT( tsu.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, WORKING_DIR, EXE_PATH ) );

  // Verify that the result file does exist. This is a sufficient to verify that the task was created successfully.
  CPPUNIT_ASSERT( waitForFile( COPIED_FILE, 5 ) );

  // Remove the scheduled task.
  CPPUNIT_ASSERT( tsu.deleteScheduledTask( TASK_NAME ) );

  // Verify the task is gone.
  CPPUNIT_ASSERT( !tsu.taskExists( TASK_NAME ) );
  };


void TaskSchedulerUtilTests::testTaskExists()
  {
  TaskSchedulerUtil tsu;

  // Test attempting to check the task without first initializing COM.
  CPPUNIT_ASSERT( !tsu.taskExists( TASK_NAME ) );

  // Initialize COM.
  CPPUNIT_ASSERT( tsu.init() );

  // Create the task.
  CPPUNIT_ASSERT( tsu.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, WORKING_DIR, EXE_PATH ) );

  // Verify the task exists.
  CPPUNIT_ASSERT( tsu.taskExists( TASK_NAME ) );

  // Remove the scheduled task.
  CPPUNIT_ASSERT( tsu.deleteScheduledTask( TASK_NAME ) );

  // Verify the task is gone.
  CPPUNIT_ASSERT( !tsu.taskExists( TASK_NAME ) );
  };





///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support function implementations.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  bool getNowPlus( unsigned int numSecondsToAdd, TDateTime & theTime )
    {
    theTime = _time64( NULL );
    theTime += numSecondsToAdd;
    return true;
    }


  bool waitForFile( const STRING & fileName, DWORD timeoutInSeconds )
    {
    TDateTime startTime = _time64( NULL );
    TDateTime endTime   = startTime + timeoutInSeconds;

    bool result = false;
    do
      {
      if ( Utils::fileExists( fileName ) )
        {
        result = true;
        break;
        }

      Sleep( 200 );

      } while ( _time64( NULL ) < endTime );

    return result;
    }
  }
