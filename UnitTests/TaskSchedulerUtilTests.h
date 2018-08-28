/*
Overview:
  This file provides the TaskSchedulerUtil test fixture.

Created By: Doug Beyer, 08/26/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/

#pragma once

#include <cppunit/extensions/HelperMacros.h>


namespace test
{

class TaskSchedulerUtilTests : public CPPUNIT_NS::TestFixture
  {
  CPPUNIT_TEST_SUITE( TaskSchedulerUtilTests );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testInit );
    CPPUNIT_TEST( testCreateScheduledTask_LaunchExecutable );
    CPPUNIT_TEST( testDeleteScheduledTask );
    CPPUNIT_TEST( testTaskExists );
    // Add additional tests here...
  CPPUNIT_TEST_SUITE_END();


  public:
    void setUp();
    void tearDown();


  private:

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  Tests.

    void testConstruction();
    void testInit();
    void testCreateScheduledTask_LaunchExecutable();
    void testDeleteScheduledTask();
    void testTaskExists();

  };


};


