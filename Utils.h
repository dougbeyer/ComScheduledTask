/*
Overview:
  This file provides utility functions via a utility class.

Created By: Doug Beyer, 08/11/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


#pragma once

#include <string>
#include <Windows.h>
#include <set>


namespace cofense
{

  class Utils
    {
    public:

      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Functionality.

      // This function is only here to help clean up the noisy log lines in this coding exercise's source files since
      // I'm not using a real debug log implementation. And obviously, I didn't implement log levels for this call.
      //
      static void log( const _TCHAR * pszFmt, ... );


      // Convert the TDateTime to struct tm.
      static bool convertDateTimeToStructTm( const TDateTime & theTime, struct tm & timeParts );


      // Returns a set of the process ids for all processes with the param name running on the local machine. If processName
      // is blank, all process ids will be returned.
      static bool getProcessesWithName( const STRING & processName, std::set<DWORD> & processIds );


      // Returns true if the file exists; false otherwise.
      static bool fileExists( const STRING & filePath );


      // Returns true if the file is successfully deleted or it the file doesn't exist. If errorIfNotExists is true,
      // then returns false if the file doesn't exist.
      static bool deleteFile(const STRING & filePath, bool errorIfNotExists = false);


      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Getters/Setters



    protected:


    private:

      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Constructors/Destructor

      Utils (){};
      virtual ~Utils(){};


      ///////////////////////////////////////////////////////////////////////////////////////////
      // Prevent copying and assigning of instances.
      // Do not implement to allow linker to yell if someone attempts to use them.
      Utils             ( const Utils& Object );
      Utils             ( Utils&& Object );
      Utils& operator=  ( const Utils& Object );
      Utils& operator=  ( Utils&& Object );


    };  // Utils



};  // namespace cofense
