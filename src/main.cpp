#include <Arduino.h>
#include "HomeSpan.h"         // include the HomeSpan library
#include "DEV_Identify.h"      
#include "DEV_Doors.h" 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);       // start the Serial interface
  const char *ssid = "SmartKing";
  const char *pw = "123456";

  homeSpan.setStatusPin(2);
  homeSpan.setApSSID(ssid);
  homeSpan.setApPassword(pw);
  homeSpan.setLogLevel(1);
  homeSpan.begin(Category::GarageDoorOpeners,"Garage Remote");

  new SpanAccessory();
    new DEV_Identify("Garage Door", "SmartKing", "91INB", "Door", "0.9", 2);
    new Service::HAPProtocolInformation();
      new Characteristic::Version("1.1.0");
    new DEV_GarageDoor(23, 21);
}

void loop() {
  // put your main code here, to run repeatedly:
  homeSpan.poll();
}