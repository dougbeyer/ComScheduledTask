/*
Overview:
  This file provides the console app for the COM Task Scheduler coding exercise.

Created By: Doug Beyer, 08/24/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


#include "stdafx.h"
#include "windows.h"
#include "Utils.h"
#include "TaskSchedulerUtil.h"


using namespace std;
using namespace cofense;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Supporting definitions.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define RET_CODE_SUCCESS    0
#define RET_CODE_BAD_ARGS   1
#define RET_CODE_FAILURE    2


///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support function prototypes/variables.
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace
  {
  const STRING TASK_NAME          ( _T( "TaskSchedulerUtilTests Task" ) );
  const STRING AUTHOR_NAME        ( _T( "Some Author Name" ) );
  const STRING EXE_PATH           ( _T( "c:\\windows\\notepad.exe" ) ); // This path exists on all Windows machines.
  const STRING WORKING_DIR        ( _T( "" ) );

  const STRING MSG_EXIT           ( _T( "Hit enter to exit...\n" ) );
  const STRING MSG_WAIT_FOR_TASK  ( _T( "Hit enter when done waiting for task to fire...\n" ) );


  void usage  ();
  void pause  ( const STRING & msg );
  }



///////////////////////////////////////////////////////////////////////////////////////////////////
//  Main program.
///////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain( int argc, _TCHAR* argv[] )
  {
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  // DSB, 08/28/2018 - For this coding exercise, I'm just going to hardcode the task info rather than passing the task
  // name, author, working dir, and exe path in as cmd line args.
  //
  // Process command line args.
  //if ( argc < `xxx` )
  //  {
  //usage();
  //return RET_CODE_BAD_ARGS;
  //}

  // Prepare the task scheduler.
  TaskSchedulerUtil taskScedUtil;
  if ( !taskScedUtil.init() )
    {
    Utils::log( _T( "\nMain() - Faied to init task scheduler." ) );
    return RET_CODE_FAILURE;
    }

  // Create the task. Note that it will fire as soon as it's registered.
  if ( !taskScedUtil.createScheduledTask_LaunchExecutable( TASK_NAME, AUTHOR_NAME, WORKING_DIR, EXE_PATH ) )
    {
    Utils::log( _T( "\nMain() - Faied to init scheduled task." ) );
    return RET_CODE_FAILURE;
    }

  pause( MSG_WAIT_FOR_TASK );

  if ( !taskScedUtil.deleteScheduledTask( TASK_NAME ) )
    {
    Utils::log( _T( "\nMain() - Faied to delete scheduled task." ) );
    return RET_CODE_FAILURE;
    }

  pause( MSG_EXIT );

  return RET_CODE_SUCCESS;
  }



///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  void usage()
    {
    //_tprintf( _T( "Usage:  `usage`" ) );
    }


  void pause( const STRING & msg )
    {
    _tprintf( msg.c_str() );

    // Causes all previously entered chars to be ignored; of particular conern are any '\n' chars
    // which would cause the wcin.getline() call to return immediately.
    CIN.sync();

    STRING str;
    getline( CIN, str );
    }
  }


