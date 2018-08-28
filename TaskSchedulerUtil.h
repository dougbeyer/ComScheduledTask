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

  // DSB, 08/26/2018 - I could have used the <chrono> datetime stuff. But for this coding exercise, I'll make it simple.
  //
  //typedef __time64_t TDateTime;

  //typedef struct
  //  {
  //  unsigned short  year;
  //  unsigned char   month;
  //  unsigned char   day;
  //  unsigned char   hours;
  //  unsigned char   minutes;
  //  unsigned char   seconds;
  //  char            utcOffsetHours; // Can be negative (e.g. Pacific time zone = -8).
  //  unsigned char   utcOffsetMinutes;

  //  void addSeconds( unsigned int seconds )
  //    {
  //    this.seconds += seconds;
  //    if ( this.seconds > 60 )
  //      {
  //      this.minutes += 1;
  //      this.seconds -= 60;
  //      }
  //    }
  //  } TDateTime;


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
      // createScheduledTask_LaunchExecutable() will launch the specified executable at the specified time. The task will
      // become deactivated shortly after the start time; effectively making this a "run once" task.
      //
      bool createScheduledTask_LaunchExecutable(  const STRING &    taskName,
                                                  const STRING &    authorName,
                                                  const TDateTime & startTime,
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
