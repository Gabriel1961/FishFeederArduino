#include <Arduino_JSON.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "utility.h"
#include "motor.h"

int state = 0, flag = 0;
const int TEMP_SENSOR_PIN = 50;
DHT_Unified dht(TEMP_SENSOR_PIN, DHT11);
Motor feedingMotor(12,13,14);
#include "feedingScheduler.h"

FeedingScheduler feedingScheduler(feedingMotor);

#include "datamodel.h"



DataModel dataModel;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Hellow computer");
  digitalWrite(LED_BUILTIN, LOW);
  dht.begin();
}


void loop() {
  dataModel.readTemperatureAndHumidity(dht);
  if (Serial1.available() > 0) {
    // read messages comming from the bluetooth device 
    String data = Serial1.readString();
    Serial.println("data");
    int cidx = 0, nidx;
    do { // if multiple messages come together, process each one individually 
      nidx = getNewJsonIdx(data, cidx);
      JSONVar json = JSON.parse(data.c_str() + cidx);
      Serial.println(JSON.stringify(json));
      JSONVar resp = dataModel.processMessage(json);
      String respStr = JSON.stringify(resp);
      if(respStr != "{}") // send if response isn't empty
        Serial1.println(JSON.stringify(resp));
      cidx = nidx;
    }while(nidx != data.length());
  }

  dataModel.update();
}
