/*
Overview:
  This file provides the Utils implementation.

Created By: Doug Beyer, 08/11/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


///////////////////////////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <cctype>
#include <time.h>
#include <stdarg.h>
#include <Windows.h>
#include <psapi.h>
#include "Utils.h"


using namespace cofense;



///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entities.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  bool getProcessName(DWORD processId, STRING & processName);
  }



///////////////////////////////////////////////////////////////////////////////////////////////////
//  Utils implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

void Utils::log( const _TCHAR * pszFmt, ... )
  {
  if ( NULL == pszFmt )
    return;

  va_list args;
  va_start( args, pszFmt );

  _TCHAR buffer[ 256 ] = {};
  _vsntprintf_s( buffer, 256, _TRUNCATE, pszFmt, args );

  // Write error to stdout.
  COUT << buffer;

  // **TODO**: DSB, 08/11/2018 - Log error in debug log.

  va_end( args );
  }


bool Utils::convertDateTimeToStructTm( const TDateTime & theTime, struct tm & timeParts )
  {
  // Prep the output params.
  SecureZeroMemory( &timeParts, sizeof( struct tm ) );

  errno_t errorNum = _localtime64_s( &timeParts, &theTime );
  bool result = ( 0 == errorNum );
  if ( !result )
    Utils::log( _T( "\nconvertDateTimeToStructTm() - Failed to convert the time to struct tm: %x" ), errorNum );

  return result;
  }


bool Utils::getProcessesWithName( const STRING & processName, std::set<DWORD> & processIds )
  {
  // Prep output params.
  processIds.clear();

  // The enum processes code was taken from https://docs.microsoft.com/en-us/windows/desktop/psapi/enumerating-all-processes.

  DWORD aProcesses[1024], cbNeeded, cProcesses;

  size_t arraySize = sizeof( aProcesses );
  if ( !EnumProcesses( aProcesses, (DWORD)arraySize, &cbNeeded ) )
    {
    Utils::log( _T( "\ngetProcessesWithName() - Failed to enum processes: %x" ), GetLastError() );
    return false;
    }

  // EnumProcesses doesn't tell us if the array was to small to handle all the processes. MSDN recommends using a larger
  // array and calling EnumProcesses again if cbNeeded == the size of the array. But, for this coding exercise, I'll just
  // log the condition and continue.
  if ( cbNeeded == arraySize )
    Utils::log( _T( "\ngetProcessesWithName() - Array size may not have been large enough for all processes." ) );

  // Calculate how many process identifiers were returned.
  cProcesses = cbNeeded / sizeof( DWORD );

  bool filter = !processName.empty();

  DWORD   processId = 0;
  STRING  currProcessName;
  const _TCHAR * pszProcessName = processName.c_str();
  size_t processNameLen = processName.length();
  for ( unsigned int i = 0; i < cProcesses; ++i )
    {
    processId = aProcesses[i];

    if ( filter )
      {
      if ( !getProcessName( processId, currProcessName ) )
        {
        // This is too noisy. There's processes that can't be opened by user mode code (e.g. the system process), etc.
        //Utils::log( _T( "\ngetProcessesWithName() - Failed to get process name for pid = %d. Will not include process " )
        //            _T( "in result set." ),
        //            processId );
        continue;
        }

      // If the process names don't match, skip the current process.
      if ( 0 != _tcsnicmp( pszProcessName, currProcessName.c_str(), processNameLen ) )
        continue;
      }

    // If we're here, we should include the process id in the result set.
    processIds.insert( processId );
    }

  return true;
  }


bool Utils::fileExists( const STRING & filePath )
  {
  std::ifstream ifile( filePath.c_str() );
  return (bool)ifile;
  }


bool Utils::deleteFile( const STRING & filePath, bool errorIfNotExists )
  {
  bool fileExists = Utils::fileExists( filePath );

  if ( !fileExists && errorIfNotExists )
    return false;
  else if ( !fileExists && !errorIfNotExists )
    return true;

  // We know the file exists so delete it.
  assert( fileExists );
  return ( 0 == _tremove( filePath.c_str() ) );
  }







///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entity implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {
  bool getProcessName( DWORD processId, STRING & processName )
    {
    bool result = false;

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId );
    if ( NULL == hProcess )
      {
      // This is too noisy. There's processes that can't be opened by user mode code (e.g. the system process), etc.
      //Utils::log( _T( "\ngetProcessName() - Failed to open process for pid = %d." ), processId );
      goto DONE;
      }

    _TCHAR buffer[ 256 ] = {};
    if ( !GetModuleBaseName( hProcess, NULL, buffer, 256 ) )
      {
      Utils::log( _T( "\ngetProcessName() - Failed to get process name: %x" ), GetLastError() );
      goto DONE;
      }

    processName = buffer;
    result      = true;

    DONE:
      if ( NULL != hProcess )
        CloseHandle( hProcess );

      return result;
    }
  }
