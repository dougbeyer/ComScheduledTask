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
#include "Utils.h"


using namespace cofense;



///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entities.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {

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




///////////////////////////////////////////////////////////////////////////////////////////////////
//  File-scoped support entity implementation.
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
  {

  }
