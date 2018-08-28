/*
Overview:
  This file provides the TaskSchedulerUtil implementation.

Created By: Doug Beyer, 08/26/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

  1)  The bulk of this code originally came from https://docs.microsoft.com/en-us/windows/desktop/taskschd/time-trigger-example--c--- used.
      I've added debug logging, additional error checking, function modularization, etc.

*/


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define _WIN32_DCOM

#include <comdef.h>
//#include <wincred.h>
#include <taskschd.h> //  Include the task header file.

#include "TaskSchedulerUtil.h"
#include "Utils.h"

#pragma comment( lib, "taskschd.lib" )
//#pragma comment( lib, "comsupp.lib" )
//#pragma comment( lib, "credui.lib" )


using namespace std;
using namespace cofense;



///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entities.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  // The string format for Task Scheduler times is documented as YYYY-MM-DDTHH:MM:SS(+-)(timezone); where the the timezone
  // portion (i.e. the difference from UTC) is optional. For this coding exercise, I'm working in local time so I'm ignoring
  // the timezone portion
  //.
  const _TCHAR * FMT_TIME = _T("%4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d");



  STRING            dateTimeToStr             ( TDateTime & theTime );
  ITaskService *    getTaskService            ();
  ITaskFolder *     getTaskFolder             ( ITaskService * pService );
  ITaskDefinition * getTaskDefinition         ( ITaskService * pService );
  bool              deleteTask                ( ITaskFolder * pFolder, const STRING & taskName );
  bool              setupTaskRegistrationInfo ( ITaskDefinition * pTask, const STRING & authorName );
  bool              setupTaskPrincipal        ( ITaskDefinition * pTask );
  bool              setupTaskSettings         ( ITaskDefinition * pTask );
  bool              setupTaskTrigger          ( ITaskDefinition * pTask );
  bool              setupTaskAction           ( ITaskDefinition * pTask, const STRING & workingDir, const STRING & exePath );
  bool              registerTask              ( ITaskFolder * pFolder, ITaskDefinition * pTask, const STRING & taskName );
  }



///////////////////////////////////////////////////////////////////////////////////////////////////
//  TaskSchedulerUtil implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

TaskSchedulerUtil::TaskSchedulerUtil()
  {
  // No additional functionality.
  }


TaskSchedulerUtil::~TaskSchedulerUtil()
  {
  try
    {
    deinitCom();
    }
  catch (...)
    {
    ;   // Don't allow exceptions out of the destructor.
    }
  }


bool TaskSchedulerUtil::init()
  {
  // DSB, 08/26/2018 - Since initialization of COM is on a per-thread basis, I can't use static methods in this class.

  // In case init() gets called more than once.
  deinitCom();

  bool result = false;

  // The sample code from https://docs.microsoft.com/en-us/windows/desktop/taskschd/time-trigger-example--c--- used
  // the multithreaded COM apartment so I'm assuming that's safe to do. Although I'm not sure exactly what class to look
  // for in the Windows registry, I did see that the TaskScheduler class has a ThreadingModel attribute of "both"; so using
  // a multithreaded apartment will be ok.
  //
  // DSB, 08/26/2018 - One could argue that the desired COM apartment model should be passed as a parameter since we
  // shouldn't assume how the client might want to use it. But for this coding exercise, I hide this detail since, actually,
  // the use of COM to do the task scheduling is an implementation detail.
  //
  HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
  if ( FAILED( hr ) )
    {
    Utils::log( _T( "\nCoInitializeEx failed: %x" ), hr );
    goto DONE;
    }

  m_ComInitialized = true;

  // Set general COM security levels.
  hr = CoInitializeSecurity(
    NULL,                           // Security descriptor.
    -1,                             // Allow COM to choose which authentication services will be registered.
    NULL,                           // No authentication services to use. Allow COM to choose.
    NULL,                           // Reserved
    RPC_C_AUTHN_LEVEL_PKT_PRIVACY,  // Authentication level
    RPC_C_IMP_LEVEL_IMPERSONATE,    // Impersonation level
    NULL,                           // No authentication services to use. Allow COM to choose.
    EOAC_NONE,                      // No authentication capabilities
    NULL );                         // Reserved


  if ( FAILED( hr ) )
    {
    Utils::log( _T( "\nCoInitializeSecurity failed: %x" ), hr );
    goto DONE;
    }

  result = true;

  DONE:
    if ( !result )
      deinitCom();

    return true;
  }



void TaskSchedulerUtil::deinitCom()
  {
  if ( comInitialized() )
      {
      CoUninitialize();
      m_ComInitialized = false;
      }
  }


bool TaskSchedulerUtil::createScheduledTask_LaunchExecutable
  (
  const STRING & taskName,
  const STRING & authorName,
  const STRING & workingDir,
  const STRING & exePath
  )
  {
  if ( !comInitialized() )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() called before COM was initialized." ) );
    return false;
    }

  ITaskService *    pService  = NULL;
  ITaskFolder *     pFolder   = NULL;
  ITaskDefinition * pTask     = NULL;

  pService = getTaskService();
  if ( NULL == pService )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to get task service." ) );
    return false;
    }

  bool result = false;

  pFolder = getTaskFolder( pService );
  if ( NULL == pFolder )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to get task folder." ) );
    goto DONE;
    }

  // If the same task exists, remove it.
  if ( !deleteTask( pFolder, taskName ) )
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to delete pre-existing task." ) );

  pTask = getTaskDefinition( pService );
  if ( NULL == pTask )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to get task." ) );
    goto DONE;
    }

  if ( !setupTaskRegistrationInfo( pTask, authorName ) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to set task registration info." ) );
    goto DONE;
    }

  if ( !setupTaskPrincipal( pTask ) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to set task principal info." ) );
    goto DONE;
    }

  if ( !setupTaskSettings( pTask ) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to set task setting info." ) );
    goto DONE;
    }

  if ( !setupTaskTrigger( pTask ) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to set task trigger info." ) );
    goto DONE;
    }

  if ( !setupTaskAction( pTask, workingDir, exePath ) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to set task trigger info." ) );
    goto DONE;
    }

  if ( !registerTask( pFolder, pTask, taskName) )
    {
    Utils::log( _T( "\ncreateScheduledTask_LaunchExecutable() - Failed to register task." ) );
    goto DONE;
    }

  result = true;

  DONE:
    if ( NULL != pService )
      pService->Release();
    if ( NULL != pFolder )
      pFolder->Release();
    if ( NULL != pTask )
      pTask->Release();

    return result;
  }




///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entity implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  bool dateTimeToStr( const TDateTime & theTime, STRING & dateTimeStr )
    {
    struct tm timeParts = {0};
    if ( !Utils::convertDateTimeToStructTm( theTime, timeParts ) )
      {
      Utils::log( _T( "\ndateTimeToStr() - Failed to convert time to struct tm." ) );
      return false;
      }

    _TCHAR buffer[ 22 ] = {};
    _sntprintf_s( buffer,
                  22,
                  _TRUNCATE,
                  FMT_TIME,
                  timeParts.tm_year + 1900,
                  timeParts.tm_mon + 1,
                  timeParts.tm_mday,
                  timeParts.tm_hour,
                  timeParts.tm_min,
                  timeParts.tm_sec );

    dateTimeStr = buffer;

    return true;
    }


  ITaskService * getTaskService()
    {
    ITaskService *pService = NULL;
    HRESULT hr = CoCreateInstance(  CLSID_TaskScheduler,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_ITaskService,
                                    (void**)&pService );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\ngetTaskService() - Failed to create an instance of ITaskService: %x" ), hr );
      return NULL;
      }

    // Connect to the task service.
    hr = pService->Connect( _variant_t(), // Execute on local computer.
                            _variant_t(), // Use current user's user name.
                            _variant_t(), // Use current user's domain.
                            _variant_t()  // Use current user's password.
                          );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\ngetTaskService() - Failed to connect to the ITaskService: %x" ), hr );
      pService->Release();
      return NULL;
      }

    return pService;
    }


  ITaskFolder * getTaskFolder( ITaskService * pService )
    {
    if ( NULL == pService )
      {
      Utils::log( _T( "\ngetTaskFolder() - Invalid args." ) );
      return NULL;
      }

    // For this coding exercise, I'll assume it's ok to store the task in the root folder.
    //
    ITaskFolder *pRootFolder = NULL;
    HRESULT hr = pService->GetFolder( _bstr_t( _T( "\\" ) ) , &pRootFolder );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\ngetTaskFolder() - Cannot get root folder pointer: %x" ), hr );
      return NULL;
      }

    return pRootFolder;
    }


  ITaskDefinition * getTaskDefinition ( ITaskService * pService )
    {
    if ( NULL == pService )
      {
      Utils::log( _T( "\ngetTaskDefinition() - Invalid args." ) );
      return NULL;
      }

    ITaskDefinition * pTask = NULL;
    HRESULT hr = pService->NewTask( 0, &pTask );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\ngetTaskDefinition() - Failed to create task: %x." ), hr );
      return NULL;
      }

    return pTask;
    }


  bool setupTaskRegistrationInfo( ITaskDefinition * pTask, const STRING & authorName )
    {
    if ( NULL == pTask || authorName.empty() )
      {
      Utils::log( _T( "\nsetupTaskRegistrationInfo() - Invalid args." ) );
      return false;
      }

    IRegistrationInfo * pRegInfo = NULL;
    HRESULT hr = pTask->get_RegistrationInfo( &pRegInfo );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\nsetupTaskRegistrationInfo() - Cannot get task registration info: %x" ), hr );
      return false;
      }

    hr = pRegInfo->put_Author(_bstr_t( authorName.c_str() ) );
    bool result = SUCCEEDED( hr );
    if ( !result )
      Utils::log( _T( "\nsetupTaskRegistrationInfo() - Cannot put identification info: %x" ), hr );

    pRegInfo->Release();
    return result;
    }


  bool setupTaskPrincipal( ITaskDefinition * pTask )
    {
    if ( NULL == pTask )
      {
      Utils::log( _T( "\nsetupTaskPrincipal() - Invalid args." ) );
      return false;
      }

    IPrincipal * pPrincipal = NULL;
    HRESULT hr = pTask->get_Principal( &pPrincipal );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\nsetupTaskPrincipal() - Cannot get task principal: %x" ), hr );
      return false;
      }

    // Set up principal logon type to the interactive logon.
    hr = pPrincipal->put_LogonType( TASK_LOGON_INTERACTIVE_TOKEN );
    bool result = SUCCEEDED( hr );
    if ( !result )
      Utils::log( _T( "\nsetupTaskPrincipal() - Cannot put principal info: %x" ), hr );

    pPrincipal->Release();
    return result;
    }


  bool setupTaskSettings( ITaskDefinition * pTask )
    {
    if ( NULL == pTask )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Invalid args." ) );
      return false;
      }

    ITaskSettings * pSettings     = NULL;
    IIdleSettings * pIdleSettings = NULL;

    // Create the settings for the task
    HRESULT hr = pTask->get_Settings( &pSettings );
    if ( FAILED( hr ) )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Cannot get task settings: %x" ), hr );
      return false;
      }

    // Set setting values for the task.
    hr = pSettings->put_StartWhenAvailable( VARIANT_TRUE );
    bool result = SUCCEEDED( hr );
    if ( !result )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Cannot set task settings: %x" ), hr );
      goto DONE;
      }

    // Set the idle settings for the task.
    hr = pSettings->get_IdleSettings( &pIdleSettings );
    result = SUCCEEDED( hr );
    if ( !result )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Cannot get task idle settings: %x" ), hr );
      goto DONE;
      }

    hr = pIdleSettings->put_WaitTimeout( L"PT5M" );
    result = SUCCEEDED( hr );
    if ( !result )
      Utils::log( _T( "\nsetupTaskSettings() - Cannot set task idle settings: %x" ), hr );

    DONE:
      pSettings->Release();
      if ( NULL != pIdleSettings )
        pIdleSettings->Release();
      return result;
    }


  bool setupTaskTrigger( ITaskDefinition * pTask )
    {
    if ( NULL == pTask )
      {
      Utils::log( _T( "\nsetupTaskTrigger() - Invalid args." ) );
      return false;
      }

    ITriggerCollection *    pTriggerCollection  = NULL;
    ITrigger *              pTrigger            = NULL;
    IRegistrationTrigger *  pRegTrigger        = NULL;

    HRESULT hr = pTask->get_Triggers( &pTriggerCollection );
    if( FAILED( hr ) )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Cannot set trigger collection: %x" ), hr );
      return false;
      }

    //  Add the registration trigger to the task.
    hr = pTriggerCollection->Create( TASK_TRIGGER_REGISTRATION, &pTrigger );
    bool result = SUCCEEDED( hr );
    if( !result )
      {
      Utils::log( _T( "\nsetupTaskSettings() - Cannot create trigger: %x" ), hr );
      goto DONE;
      }

    hr = pTrigger->QueryInterface( IID_IRegistrationTrigger, (void**) &pRegTrigger );
    result = SUCCEEDED( hr );
    if( !result )
      {
      Utils::log( _T( "\nsetupTaskSettings() - QueryInterface call failed for registration trigger: %x" ), hr );
      goto DONE;
      }

    // **FUTURE**: DSB, 08/27/2018 - The id of the trigger is probably not important since there's only one trigger for
    // this task. However, I may want to use a guid or something to ensure uniqueness.
    //
    // Trigger id.
    hr = pRegTrigger->put_Id( _bstr_t( _T( "Trigger1" ) ) );
    if( FAILED( hr ) )
      Utils::log( _T( "\nsetupTaskSettings() - Cannot put trigger ID: %x" ), hr );

    DONE:
      pTriggerCollection->Release();
      if ( NULL != pTrigger )
        pTrigger->Release();
      if ( NULL != pRegTrigger )
        pRegTrigger->Release();
      return result;
    }


  bool setupTaskAction( ITaskDefinition * pTask, const STRING & workingDir, const STRING & exePath )
    {
    if ( NULL == pTask || exePath.empty() )
      {
      Utils::log( _T( "\nsetupTaskAction() - Invalid args." ) );
      return false;
      }

    IActionCollection * pActionCollection = NULL;
    IAction *           pAction           = NULL;
    IExecAction *       pExecAction       = NULL;

    HRESULT hr = pTask->get_Actions( &pActionCollection );
    if( FAILED( hr ) )
      {
      Utils::log( _T( "\nsetupTaskAction() - Cannot get Task collection pointer: %x" ), hr );
      return false;
      }

    // Create the action, specifying that it is an executable action.
    hr = pActionCollection->Create( TASK_ACTION_EXEC, &pAction );
    bool result = SUCCEEDED( hr );
    if( !result )
      {
      Utils::log( _T( "\nsetupTaskAction() - Cannot create the action: %x" ), hr );
      goto DONE;
      }

    // QI for the executable task pointer.
    hr = pAction->QueryInterface( IID_IExecAction, (void**) &pExecAction );
    result = SUCCEEDED( hr );
    if( !result )
      {
      Utils::log( _T( "\nsetupTaskAction() - QueryInterface call failed for IExecAction: %x" ), hr );
      goto DONE;
      }

    // Set the path of the executable.
    hr = pExecAction->put_Path( _bstr_t( exePath.c_str() ) );
    result = SUCCEEDED( hr );
    if( !result )
      {
      Utils::log( _T( "\nsetupTaskAction() - Cannot put action path: %x" ), hr );
      goto DONE;
      }

    // Set the path of the working directory.
    if ( !workingDir.empty() )
      {
      hr = pExecAction->put_WorkingDirectory( _bstr_t( workingDir.c_str() ) );
      result = SUCCEEDED( hr );
      if( !result )
        {
        Utils::log( _T( "\nsetupTaskAction() - Cannot put action working directory: %x" ), hr );
        goto DONE;
        }
      }

    DONE:
      pActionCollection->Release();
      if ( NULL != pAction )
        pAction->Release();
      if ( NULL != pExecAction )
        pExecAction->Release();
      return result;
    }


  bool registerTask( ITaskFolder * pFolder, ITaskDefinition * pTask, const STRING & taskName )
    {
    if ( NULL == pFolder || NULL == pTask || taskName.empty() )
      {
      Utils::log( _T( "\nregisterTask() - Invalid args." ) );
      return false;
      }

    IRegisteredTask * pRegisteredTask = NULL;
    HRESULT hr = pFolder->RegisterTaskDefinition( _bstr_t( taskName.c_str() ),
                                                  pTask,
                                                  TASK_CREATE_OR_UPDATE,
                                                  _variant_t(),
                                                  _variant_t(),
                                                  TASK_LOGON_INTERACTIVE_TOKEN,
                                                  _variant_t( _T( "" ) ),
                                                  &pRegisteredTask );

    bool result = SUCCEEDED( hr );
    if( !result )
      Utils::log( _T( "\nregisterTask() - Error registering the task: %x" ), hr );

    if ( NULL != pRegisteredTask )
      pRegisteredTask->Release();

    return result;
    }


  bool deleteTask( ITaskFolder * pFolder, const STRING & taskName )
    {
    if ( NULL == pFolder || taskName.empty() )
      {
      Utils::log( _T( "\nregisterTask() - Invalid args." ) );
      return false;
      }

    //  If the task exists, remove it.
    HRESULT hr = pFolder->DeleteTask( _bstr_t( taskName.c_str() ), 0 );
    bool result = SUCCEEDED( hr ) || ( FAILED( hr ) && HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) == hr );

    if ( !result )
      Utils::log( _T( "\ndeleteTask() - Failed to delete task: %x" ), hr );

    return result;
    }

  }


