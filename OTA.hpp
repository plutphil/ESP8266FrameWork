
#include <ArduinoOTA.h>
#ifdef ESP32
#include <ESPmDNS.h>
#elif defined(ESP8266)
#include <ESP8266mDNS.h>
#endif
void ota_setup(){
  ArduinoOTA.onStart([]() { events.send("start", "o"); });
  ArduinoOTA.onEnd([]() { events.send("Update End", "o"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
    events.send(p, "ota");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    if(error == OTA_AUTH_ERROR) events.send("authF", "o");
    else if(error == OTA_BEGIN_ERROR) events.send("BeginF", "o");
    else if(error == OTA_CONNECT_ERROR) events.send("ConnectF", "o");
    else if(error == OTA_RECEIVE_ERROR) events.send("RecieveF", "o");
    else if(error == OTA_END_ERROR) events.send("EndF", "o");
  });
  ArduinoOTA.setHostname("esp8266");
  ArduinoOTA.begin();
  MDNS.addService("http","tcp",80);
}
