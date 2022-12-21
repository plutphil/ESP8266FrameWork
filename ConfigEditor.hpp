bool shouldSaveEntry=false;
String entryKey="";
String entryVal="";

void registerConfigEditor(){
  //add wifi
  server.on("/c",HTTP_GET,[](AsyncWebServerRequest *request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("k")) {
      senderrmesg(request, "no key");
      return;
    }
    if (!request->hasParam("v")) {
      senderrmesg(request, "no value");
      return;
    }
    entryKey = request->getParam("k")->value();
    entryVal = request->getParam("v")->value();
    shouldSaveEntry=true;
    configtxt.set(entryKey,entryVal);
    sendmesg(request,"Success");
  });
  server.on("/lc",HTTP_GET,[](AsyncWebServerRequest *request) {
    
    if (!checkUserWebAuth(request)) return;
    Config configtxt=Config();
    
    if(!configtxt.openr()){
      senderrmesg(request, "cannot open configfile");
      return;
    }
    
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(opentag("table"));
    while(configtxt.configFile.available()){
      configtxt.readkeyvalue();
      response->print(
        opentag("tr")
        +td(configtxt.rkey)
        +td(form("/c",input("k",configtxt.rkey,"hidden")+input("v",configtxt.rval)))
        +closetag("tr"));
    }
    configtxt.close();
    response->print(closetag("table"));
    
    response->print(form("/c","Add"+input("k","")+input("v",""))+ahref("/","Back"));
    request->send(response);
  });
}
