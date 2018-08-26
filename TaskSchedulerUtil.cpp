/*
Overview:
  This file provides the TaskSchedulerUtil implementation.

Created By: Doug Beyer, 08/26/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

  1)  The bulk of this code came from https://docs.microsoft.com/en-us/windows/desktop/taskschd/time-trigger-example--c--- used

*/


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#define _WIN32_DCOM

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h> //  Include the task header file.

#include "TaskSchedulerUtil.h"
#include "Utils.h"

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")


using namespace std;
using namespace cofense;



///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entities.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  ITaskService * getTaskService();
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
  if( FAILED( hr ) )
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


  if( FAILED( hr ) )
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
  STRING      taskName,
  STRING      authorName,
  TDateTime & startTime,
  STRING      exePath
  )
  {
  if ( !comInitialized() )
    {
    Utils::log( _T( "\ncreateScheduledTask() called before COM was initialized." ) );
    return false;
    }

  ITaskService *pService = getTaskService();
  if ( NULL == pService )
    {
    Utils::log( _T( "\ncreateScheduledTask() - Failed to get task service." ) );
    return false;
    }

  bool result = false;

  // **TODO**: DSB, 08/26/2018 - Not implemented yet.

  result = true;

  DONE:
    if ( NULL != pService )
      pService->Release();

    return result;
  }




///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entity implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
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
    if( FAILED( hr ) )
      {
      Utils::log( _T( "\ngetTaskService() - Failed to connect to the ITaskService: %x" ), hr );
      pService->Release();
      return NULL;
      }

    return pService;
    }
  }
