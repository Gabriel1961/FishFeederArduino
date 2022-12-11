#include "WString.h"
#include "HardwareSerial.h"
#include "pins_arduino.h"
#include "Arduino.h"
#include "motor.h"
#pragma once
#include "TimeLib.h"

class DailyFeedingEvent
{
  public:
  int startHour, startMinute, durationSeconds;


  DailyFeedingEvent()
    :startHour(0),startMinute(0),durationSeconds(0)
  {

  }
  DailyFeedingEvent(int startHour, int startMinute, int durationSeconds)
    :startHour(startHour),startMinute(startMinute),durationSeconds(durationSeconds)
  {
  
  }

  bool isRunning = 0;
  bool ranToday = 0; 

  void run(Motor& motor)
  {
    isRunning = true;
    ranToday = true;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Started feeding");
    motor.setSpeed(1);
  }

  void stop(Motor& motor)
  {
    isRunning = false;
    Serial.println("Stopped feeding");
    digitalWrite(LED_BUILTIN, LOW);
    motor.setSpeed(0);
  }
};

/*
  Schedules feeding at certain times of the day
*/
class FeedingScheduler
{
  DailyFeedingEvent events[10];
  int eventCount;
  Motor& feedingMotor;
  public:
  FeedingScheduler(Motor& feedingMotor) : feedingMotor(feedingMotor)
  {
    
  }

  void clearEvents()
  {
    for(int i=0;i<eventCount;i++)
      if(events[i].isRunning)
        events[i].stop(feedingMotor);
    eventCount = 0;
  }

  void addEvent(DailyFeedingEvent ev)
  {
    events[eventCount++] = ev;
  }
  int lastDay=0;

  void update()
  {
    // reset all events if the next day comes
    if(lastDay!= day())
    {
      lastDay = day();
      for(int i=0;i<eventCount;i++)
        events[i].ranToday = false;
    }

    for(int i=0;i<eventCount;i++)
    {
      DailyFeedingEvent& e = events[i];
      if(e.isRunning)
      {
        if(second() >= e.durationSeconds)
          e.stop(feedingMotor);
      }
      else if(e.ranToday == false)
      {
        if(hour() == e.startHour && minute() == e.startMinute) 
        {
          e.run(feedingMotor);
        }
      }
    }
  }
};
