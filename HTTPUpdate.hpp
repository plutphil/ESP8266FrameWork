
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
void update_started() {
  //logdbg("CALLBACK:  HTTP update process started");
  events.send("start", "o");
}

void update_finished() {
  //logdbg("CALLBACK:  HTTP update process finished");
  events.send("finish", "o");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  events.send(("proc"+String(cur)+" "+String(total)).c_str(), "o");
}

void update_error(int err) {
  //logdbg("CALLBACK:  HTTP update fatal error code %d\n", err);
  events.send(("err:"+String(err)).c_str(), "o");
}
WiFiClient client;
String httpUpdateUrl="";
bool startUpdate=false;
void setup_httpupdate(){
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

  // Add optional callback notifiers
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  server.on("/h", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("u")) {
      senderrmesg(request, "no url");
      return;
    }
    httpUpdateUrl=request->getParam("u")->value();
    startUpdate=true;
    
    sendmesg(request,"started update");
  });
  
}
void httpUpdateLoop(){
  if(startUpdate){
    startUpdate=false;
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, httpUpdateUrl.c_str());
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }  
}
/*
#include<ESP8266HTTPClient.h>
#include<ESP8266httpUpdate.h>

void update_started() {
  logdbg("CALLBACK:  HTTP update process started");
  events.send("start", "o");
}

void update_finished() {
  logdbg("CALLBACK:  HTTP update process finished");
  events.send("finish", "o");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  events.send("proc"+String(curr)+" ", "o");
}

void update_error(int err) {
  logdbg("CALLBACK:  HTTP update fatal error code %d\n", err);
  events.send("proc"+String(curr)+" ", "o");
}
bool startUpdate=false;
String httpUpdateUrl="";
void httpUpdateServer(){
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  // Add optional callback notifiers
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  server.on("/h", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("u")) {
      senderrmesg(request, "no url");
      return;
    }
    const char *u = request->getParam("u")->value().c_str();
    startUpdate=true;
    sendmesg(request,"started update");
  });
}
void httpUpdateLoop(){
  if(startUpdate){
    startUpdate=false;
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url.c_str());
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
  
      case HTTP_UPDATE_NO_UPDATES:
        logdbg("HTTP_UPDATE_NO_UPDATES");
        break;
  
      case HTTP_UPDATE_OK:
        logdbg("HTTP_UPDATE_OK");
        break;
    }
  }  
}
*/
