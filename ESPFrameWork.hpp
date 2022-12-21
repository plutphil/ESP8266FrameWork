#include "smalllog.hpp"
#include "MyFS.hpp"
#include "Defaults.h"
#include "Config.hpp"
#include "WebServer.hpp"
#include "OTA.hpp"
#include "WIFI.hpp"

void framework_loop(){
  ArduinoOTA.handle();
  wifi_loop();
  httpUpdateLoop();
  if(shouldReboot){
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }
}
