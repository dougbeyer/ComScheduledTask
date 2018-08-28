/*
Overview:
  This file provides a utility class which abstracts Windows Task Scheduler functionality.

Created By: Doug Beyer, 08/26/2018

Copyright: Copyright 2018, Doug Beyer, All Rights Reserved.

Modified:

Misc Comments:

*/


#pragma once

#include <chrono>


namespace cofense
{
  class TaskSchedulerUtil
    {
    public:
      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Constructors/Destructor

      TaskSchedulerUtil          ();
      virtual ~TaskSchedulerUtil ();


      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Functionality.

      // init() should be called first to prepare the TaskSchedulerUtil for use.
      bool init();


      // DSB, 08/26/2018 - There are many settings that can be set in tasks. However, for this coding exercise, I'll
      // just pass a few. Of course, to be more general, we could define a header with constants for the setting names,
      // and pass a map of the values the caller wants to set.
      //
      // DSB, 08/26/2018 - Since the coding exercise description was very brief, I'll assume it's ok to create any kind
      // of scheduled task that I feel is appropriate.
      //
      // createScheduledTask_LaunchExecutable() will launch the specified executable as soon as the task is registered.
      // The executable's working directory will be workingDir (if blank, no working dir will be set).
      //
      bool createScheduledTask_LaunchExecutable(  const STRING &    taskName,
                                                  const STRING &    authorName,
                                                  const STRING &    workingDir,
                                                  const STRING &    exePath );


      ///////////////////////////////////////////////////////////////////////////////////////////
      //  Getters/Setters



    protected:


    private:

      ///////////////////////////////////////////////////////////////////////////////////////////
      // Prevent copying and assigning of instances.
      // Do not implement to allow linker to yell if someone attempts to use them.
      TaskSchedulerUtil            ( const TaskSchedulerUtil& Object );
      TaskSchedulerUtil            ( TaskSchedulerUtil&& Object );
      TaskSchedulerUtil& operator= ( const TaskSchedulerUtil& Object );
      TaskSchedulerUtil& operator= ( TaskSchedulerUtil&& Object );


      bool  m_ComInitialized  = false;

      bool comInitialized () { return m_ComInitialized; };
      void deinitCom      ();


    };  // TaskSchedulerUtil

};  // namespace cofense
