/*
Overview:
  This file provides the console app for the COM Task Scheduler coding exercise.

Created By: Doug Beyer, 08/24/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


// DSB, 08/25/2018 - The guts of this code came from https://docs.microsoft.com/en-us/windows/desktop/taskschd/time-trigger-example--c---


#include "stdafx.h"


//#define _WIN32_DCOM

#include "windows.h"
/*
#include <iostream>
#include <string>
#include <iostream>
//using namespace std;
//#include <Wbemidl.h>
//#include <comdef.h>
//#include <wincred.h>
//#include <taskschd.h>
#include <mstask.h>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")


using namespace std;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Supporting definitions.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
  #define CIN     wcin
  #define COUT    wcout
  #define STRING  wstring
#else
  #define CIN     cin
  #define COUT    cout
  #define STRING  string
#endif
*/

#define RET_CODE_SUCCESS    0
#define RET_CODE_BAD_ARGS   1
#define RET_CODE_FAILURE    2
#define RET_CODE_UNK_EXC    3

/*

///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support function prototypes/variables.
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace
  {
  bool              initCom         ();
  void              usage           ();
  bool              initCom         ();
  void              deinitCom       ();
  bool              initSecurity    ();
  ITaskScheduler *  getTaskScheduler();
  }
*/


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Main program.
///////////////////////////////////////////////////////////////////////////////////////////////////

int _tmain( int argc, _TCHAR* argv[] )
  {
  UNREFERENCED_PARAMETER( argc );
  UNREFERENCED_PARAMETER( argv );

  int result = RET_CODE_SUCCESS;

  // Process command line args.
  //if ( argc < `xxx` )
  //  {
  //usage();
  //return RET_CODE_BAD_ARGS;
  //}

//  if (!initCom() )
//    {
//    result = RET_CODE_FAILURE;
//    goto DONE;
//    }

//  // Get the task scheduler.
//  ITaskScheduler * pTaskScheduler = getTaskScheduler();


  // **TODO**: DSB, 08/24/2018 - Not implemented yet. Create the new task.


//  DONE:
//    deinitCom();
    return result;
  }



/*
///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support functions.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  void usage()
    {
    //_tprintf( _T( "Usage:  `usage`" ) );
    }


  bool initCom()
    {
    //HRESULT hr = CoInitialize( NULL );
    HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

    bool result = SUCCEEDED( hr );

    if ( !result )
      COUT << _T( "ERROR - Failed to init COM.\n" );

    return result;
    }


  void deinitCom()
    {
    // This function is kind of unnecessary since CoUninitialize() is a void function; hence there's no debug log line
    // if failure, etc. But, at least it maintains consistency with initCom().

    CoUninitialize();
    }


  bool initSecurity()
    {
    HRESULT hr = CoInitializeSecurity(
            NULL,                       // Security descriptor.
           -1,                          // Allow COM to choose which authentication services will be registered.
           NULL,                        // No authentication services to use. Allow COM to choose.
           NULL,                        // Reserved
           RPC_C_AUTHN_LEVEL_DEFAULT,   // Authentication level
           RPC_C_IMP_LEVEL_IMPERSONATE, // Impersonation level
           NULL,                        // No authentication services to use. Allow COM to choose.
           EOAC_NONE,                   // No authentication capabilities
           NULL );                      // Reserved

    bool result = SUCCEEDED( hr );

    if ( !result )
      COUT << _T( "Failed to initialize security. Error code = 0x" ) << hex << hr << endl;

    return result;
    }

  ITaskScheduler *  getTaskScheduler()
    {
    //HRESULT hr = CoCreateInstance(
    return NULL;
    }


  }

*/



/********************************************************************
 This sample schedules a task to start notepad.exe 1 minute from the
 time the task is registered.
********************************************************************/
/*
#define _WIN32_DCOM

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <wincred.h>
//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

using namespace std;


int __cdecl wmain()
{
    //  ------------------------------------------------------
    //  Initialize COM.
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if( FAILED(hr) )
    {
        printf("\nCoInitializeEx failed: %x", hr );
        return 1;
    }

    //  Set general COM security levels.
    hr = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        0,
        NULL);

    if( FAILED(hr) )
    {
        printf("\nCoInitializeSecurity failed: %x", hr );
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Create a name for the task.
    LPCWSTR wszTaskName = L"Time Trigger Test Task";

    //  Get the windows directory and set the path to notepad.exe.
    //wstring wstrExecutablePath = _wgetenv( L"WINDIR");
    size_t sizeRequired = 0;
    _TCHAR buffer[ 256 ] = {0};
    errno_t errorNum = _wgetenv_s( &sizeRequired, buffer, 256, L"WINDIR" );
    if ( 0 != errorNum )
      {
      _tprintf( _T( "\nFailed to get WINDIR env var." ) );
      CoUninitialize();
      return 1;
      }

    wstring wstrExecutablePath( buffer );
    wstrExecutablePath += L"\\SYSTEM32\\NOTEPAD.EXE";


    //  ------------------------------------------------------
    //  Create an instance of the Task Service.
    ITaskService *pService = NULL;
    hr = CoCreateInstance( CLSID_TaskScheduler,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_ITaskService,
                           (void**)&pService );
    if (FAILED(hr))
    {
        printf("Failed to create an instance of ITaskService: %x", hr);
        CoUninitialize();
        return 1;
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(),
        _variant_t(), _variant_t());
    if( FAILED(hr) )
    {
        printf("ITaskService::Connect failed: %x", hr );
        pService->Release();
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    ITaskFolder *pRootFolder = NULL;
    hr = pService->GetFolder( _bstr_t( L"\\") , &pRootFolder );
    if( FAILED(hr) )
    {
        printf("Cannot get Root folder pointer: %x", hr );
        pService->Release();
        CoUninitialize();
        return 1;
    }

    //  If the same task exists, remove it.
    pRootFolder->DeleteTask( _bstr_t( wszTaskName), 0  );

    //  Create the task definition object to create the task.
    ITaskDefinition *pTask = NULL;
    hr = pService->NewTask( 0, &pTask );

    pService->Release();  // COM clean up.  Pointer is no longer used.
    if (FAILED(hr))
    {
        printf("Failed to CoCreate an instance of the TaskService class: %x", hr);
        pRootFolder->Release();
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Get the registration info for setting the identification.
    IRegistrationInfo *pRegInfo= NULL;
    hr = pTask->get_RegistrationInfo( &pRegInfo );
    if( FAILED(hr) )
    {
        printf("\nCannot get identification pointer: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pRegInfo->put_Author( L"Author Name" );
    pRegInfo->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot put identification info: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Create the principal for the task - these credentials
    //  are overwritten with the credentials passed to RegisterTaskDefinition
    IPrincipal *pPrincipal = NULL;
    hr = pTask->get_Principal( &pPrincipal );
    if( FAILED(hr) )
    {
        printf("\nCannot get principal pointer: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set up principal logon type to interactive logon
    hr = pPrincipal->put_LogonType( TASK_LOGON_INTERACTIVE_TOKEN );
    pPrincipal->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot put principal info: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Create the settings for the task
    ITaskSettings *pSettings = NULL;
    hr = pTask->get_Settings( &pSettings );
    if( FAILED(hr) )
    {
        printf("\nCannot get settings pointer: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set setting values for the task.
    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    pSettings->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot put setting information: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    // Set the idle settings for the task.
    IIdleSettings *pIdleSettings = NULL;
    hr = pSettings->get_IdleSettings( &pIdleSettings );
    if( FAILED(hr) )
    {
        printf("\nCannot get idle setting information: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pIdleSettings->put_WaitTimeout(L"PT5M");
    pIdleSettings->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot put idle setting information: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }


    //  ------------------------------------------------------
    //  Get the trigger collection to insert the time trigger.
    ITriggerCollection *pTriggerCollection = NULL;
    hr = pTask->get_Triggers( &pTriggerCollection );
    if( FAILED(hr) )
    {
        printf("\nCannot get trigger collection: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Add the time trigger to the task.
    ITrigger *pTrigger = NULL;
    hr = pTriggerCollection->Create( TASK_TRIGGER_TIME, &pTrigger );
    pTriggerCollection->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot create trigger: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ITimeTrigger *pTimeTrigger = NULL;
    hr = pTrigger->QueryInterface(
        IID_ITimeTrigger, (void**) &pTimeTrigger );
    pTrigger->Release();
    if( FAILED(hr) )
    {
        printf("\nQueryInterface call failed for ITimeTrigger: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pTimeTrigger->put_Id( _bstr_t( L"Trigger1" ) );
    if( FAILED(hr) )
        printf("\nCannot put trigger ID: %x", hr);

    hr = pTimeTrigger->put_EndBoundary( _bstr_t(L"2019-08-02T08:00:00") );
    if( FAILED(hr) )
        printf("\nCannot put end boundary on trigger: %x", hr);

    //  Set the task to start at a certain time. The time
    //  format should be YYYY-MM-DDTHH:MM:SS(+-)(timezone).
    //  For example, the start boundary below
    //  is January 1st 2005 at 12:05
    hr = pTimeTrigger->put_StartBoundary( _bstr_t(L"2018-08-25T12:19:00") );
    pTimeTrigger->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot add start boundary to trigger: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }


    //  ------------------------------------------------------
    //  Add an action to the task. This task will execute notepad.exe.
    IActionCollection *pActionCollection = NULL;

    //  Get the task action collection pointer.
    hr = pTask->get_Actions( &pActionCollection );
    if( FAILED(hr) )
    {
        printf("\nCannot get Task collection pointer: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Create the action, specifying that it is an executable action.
    IAction *pAction = NULL;
    hr = pActionCollection->Create( TASK_ACTION_EXEC, &pAction );
    pActionCollection->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot create the action: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    IExecAction *pExecAction = NULL;
    //  QI for the executable task pointer.
    hr = pAction->QueryInterface(
        IID_IExecAction, (void**) &pExecAction );
    pAction->Release();
    if( FAILED(hr) )
    {
        printf("\nQueryInterface call failed for IExecAction: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set the path of the executable to notepad.exe.
    hr = pExecAction->put_Path( _bstr_t( wstrExecutablePath.c_str() ) );
    pExecAction->Release();
    if( FAILED(hr) )
    {
        printf("\nCannot put action path: %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  ------------------------------------------------------
    //  Save the task in the root folder.
    IRegisteredTask *pRegisteredTask = NULL;
    hr = pRootFolder->RegisterTaskDefinition(
            _bstr_t( wszTaskName ),
            pTask,
            TASK_CREATE_OR_UPDATE,
            _variant_t(),
            _variant_t(),
            TASK_LOGON_INTERACTIVE_TOKEN,
            _variant_t(L""),
            &pRegisteredTask);
    if( FAILED(hr) )
    {
        printf("\nError saving the Task : %x", hr );
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    printf("\n Success! Task successfully registered. " );

    //  Clean up.
    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
    return 0;
}
*/
