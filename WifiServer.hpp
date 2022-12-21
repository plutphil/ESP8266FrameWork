
void scanWifiRequest(AsyncWebServerRequest *request){
  AsyncResponseStream *response = request->beginResponseStream("text/plain");
  
  int n = WiFi.scanComplete();
  //request->send(200, "text/plain", "n: "+String(n));
  Serial.println(n);
  if(n == -2){
    WiFi.scanNetworks(true,true);
    response->println("scanning");
  }else if(n>=0){
    if(n==0){
      response->println("0");
    }
    for (int8_t i = 0; i < n; i++) {
      response->println(
        WiFi.SSID(i)
        +","+WiFi.BSSIDstr(i)
        +","+String(WiFi.RSSI(i))
        +","+String(WiFi.channel(i))
        +","+String(WiFi.encryptionType(i))
        +","+String(WiFi.isHidden(i)?"1":"0"));
    }
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
  }else{
    response->println("scanfail");  
  }
  request->send(response);
    if(n==0){
      response->println("0");
    }
  
}

bool addWifi(String ssid,String psk){
  int wi = atoi(getConfig("WIFI_COUNT").c_str());
  String wssid = getConfigByVal(ssid);
  if(wssid==""){
      addConfig("WIFI"+String(wi+1)+"_SSID",ssid);
      addConfig("WIFI"+String(wi+1)+"_PSK",psk);
      setConfig("WIFI_COUNT",String(wi+1));
      return true;
  }else{
      wi = wssid[4]-'0';
      setConfig("WIFI"+String(wi)+"_SSID",ssid);
      setConfig("WIFI"+String(wi)+"_PSK",psk);
  }
  
}

void wifi_server(){
  //scan wifi networks
  server.on("/s", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (checkUserWebAuth(request)) {
      scanWifiRequest(request);
      sendmesg(request,"success");
    }
  });
  //add wifi
  server.on("/a",HTTP_GET,[](AsyncWebServerRequest *request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("s")) {
      senderrmesg(request, "no ssid");
      return;
    }
    if (!request->hasParam("p")) {
      senderrmesg(request, "no passkey");
      return;
    }
    const char *ssid = request->getParam("s")->value().c_str();
    const char *psk = request->getParam("p")->value().c_str();
    addWifi(ssid,psk);
    sendmesg(request,"success");
    ESP.restart();
  });
  server.on("/sc",HTTP_GET,[](AsyncWebServerRequest *request) {
  
  int n = WiFi.scanComplete();
  //request->send(200, "text/plain", "n: "+String(n));
  Serial.println(n);
  if(n == -2){
    WiFi.scanNetworks(true,true);
    request->send(200, "text/html", metarefresh(3)+h1("Scanning..."));
  }else if(n>=0){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    if(n==0){
      response->print("0");
    }
    response->print(opentag("table"));
    for (int8_t i = 0; i < n; i++) {
      response->print(
        opentag("tr")
        +td(opentag("a",htmattr("href","/ah?s="+urlencode(WiFi.SSID(i))))+WiFi.SSID(i)+(WiFi.SSID(i)==""?"_":"")+closetag("a"))
        +td(WiFi.BSSIDstr(i))
        +td(String(WiFi.RSSI(i)))
        +td(String(WiFi.channel(i)))
        +td(String(WiFi.encryptionType(i)))
        +td(String(WiFi.isHidden(i)?"Hidden":""))
        +closetag("tr")
        
      );
    }
    response->print(closetag("table"));
    request->send(response);
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
  }else{
    request->send(200, "text/html", metarefresh(3)+h1("Scan failed"));
  }
    
  });
  //add wifi
  server.on("/ah",HTTP_GET,[](AsyncWebServerRequest *request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("s")) {
      request->send(201, "text/html",form("/ah",inputnice("s","SSID")+inputnice("p","Passkey")));
      return;
    }
    const char *ssid = request->getParam("s")->value().c_str();
    if (!request->hasParam("p")) {
      request->send(201, "text/html",form("/ah",inputnice("s","SSID",String(ssid))+inputnice("p","Passkey")));
      return;
    }
    const char *psk = request->getParam("p")->value().c_str();
    addWifi(ssid,psk);
    sendmesg(request,"success");
    ESP.restart();
  });
}
