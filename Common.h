/*
Overview:
  This file provides common types, etc.

Created By: Doug Beyer, 08/11/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


#pragma once


#include <assert.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <list>



///////////////////////////////////////////////////////////////////////////////////////////////////
//  Supporting definitions.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
  #define CIN       std::wcin
  #define COUT      std::wcout
  #define STRING    std::wstring
  #define IFSTREAM  std::wifstream
#else
  #define CIN       std::cin
  #define COUT      std::cout
  #define STRING    std::string
  #define IFSTREAM  std::ifstream
#endif


