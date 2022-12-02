#include "feedingScheduler.h"
#include "HardwareSerial.h"

#pragma once

enum MessageType
{
  POST=0,
  GET=1,
  SYNC_TIME=2
};

class DataModel
{
  private:
  float mTemperature,mHumidity;
  float mTemperatures[24]{};

  void handlePostMessage(JSONVar& json,JSONVar& resp)
  {
    if(json.hasOwnProperty("events")) // adding events
    {
      feedingScheduler.clearEvents();
      if(JSON.typeof(json["events"]) == "array")
      {
        JSONVar arr = json["events"];
        for(int i=0;i<arr.length();i++)
        {
          DailyFeedingEvent ev((int)arr[i]["startHour"],(int)arr[i]["startMinute"],(int)arr[i]["durationSeconds"]);
          feedingScheduler.addEvent(ev);
          Serial.println("added new event");
        }
      }
    }
  }

  void handleSyncTimeMessage(JSONVar& json,JSONVar& resp)
  {
    int32_t epoch = (String((const char*) json["epoch"])).toInt();
    setTime(epoch);
    Serial.println("Time : year: " + String(year()) + " " + String(day()) + " " + String(hour()) + " " + String(minute()) + " " + String(second()));
  }

  void handleGetMessage(JSONVar& json,JSONVar& resp)
  {
    if(json.hasOwnProperty("get") == false) 
      return errResponse("json does not have get property");
    JSONVar q = json["get"];
    if(JSON.typeof(q) != "array")
      return errResponse("json doesn't contain an array: " + JSON.typeof(q));
    
    for(int i=0;i<q.length();i++)
    {
      if(JSON.typeof(q[i]) == "string")
      {
        // write the cases for each of the properties
        if(strcmp(q[i],"temp")==0)
        {
          resp["temp"] = getTemperature();
        }
        else if(strcmp(q[i], "temps")==0) // handle request for the temps for the entire day
        {
          for(int j=0;j<24;j++)
            resp["temps"][j] = mTemperatures[j];
        }
        else if(strcmp(q[i], "humidity")==0)
        {
          resp["humidity"] = getHumidity();
        }
        else Serial.println("invalid key");
      }
      else Serial.println("is not string");
    }
  }
  public:

  void setTemperature(float v){mTemperature = v;}
  void setHumidity(float v){mHumidity = v;}
  float getTemperature(){return mTemperature;}
  float getHumidity(){return mHumidity;}
  JSONVar errResponse(String msg)
  {
    JSONVar resp;
    resp["err"] = msg;
    return resp;
  }

  JSONVar processMessage( JSONVar& json)
  {
    if (JSON.typeof(json) == "undefined") 
      return errResponse("invalid json format");

    if(json.hasOwnProperty("type") == false)
      return errResponse("message doesn't have a type");
    JSONVar resp = JSON.parse("{}");
    
    int msgtype = (int)json["type"];
    if(msgtype == POST)
      handlePostMessage(json,resp);
    else if(msgtype == GET)
      handleGetMessage(json,resp);
    else if(msgtype == SYNC_TIME)
      handleSyncTimeMessage(json,resp);
    
    Serial.println("response " + JSON.stringify(resp));
    return resp;
  }


  void readTemperatureAndHumidity(const DHT_Unified& tempSensor)
  {
    sensors_event_t event;
    tempSensor.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
    }
    else {
      setTemperature(event.temperature);
    }
    // Get humidity event and print its value.
    tempSensor.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
    }
    else {
      setHumidity(event.relative_humidity);
    }

    mTemperatures[hour()] = mTemperature;
  }

  void update()
  {
    readTemperatureAndHumidity(dht);
    feedingScheduler.update();
  }

};