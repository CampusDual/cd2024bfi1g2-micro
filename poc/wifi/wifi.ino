#include <WiFi.h>
 
#include "config.h"  // Sustituir con datos de vuestra red
#include "ESP32_C3_Utils.hpp"
 
void setup()
{
  Serial.begin(9600);
  
  ConnectWiFi_STA();
}
 
void loop() 
{
}