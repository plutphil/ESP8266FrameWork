void registerFileEditor() {
  //delete/remove file
  server.on("/d", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("p")) {
      senderrmesg(request, "Missing Parameter p");
      return;
    }
    const char *path = request->getParam("p")->value().c_str();
    if (!LittleFS.exists(path)) {
      senderrmesg(request, "doesn't exist");
      return;
    }
    LittleFS.remove(path);
    sendmesg(request, "Success");
  });

  //rename file
  server.on("/n", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("o")) {
      senderrmesg(request, "no (o)ld param");
      return;
    }
    if (!request->hasParam("n")) {
      senderrmesg(request, "no (n)ew param");
      return;
    }
    const char *oldpath = request->getParam("o")->value().c_str();
    const char *newpath = request->getParam("n")->value().c_str();
    if (!LittleFS.exists(oldpath)) {
      senderrmesg(request, "file doesn't exist");
      return;
    }
    if (LittleFS.exists(newpath)) {
      senderrmesg(request, "file with new name already exist");
      return;
    }
    LittleFS.rename(oldpath, newpath);
    sendmesg(request, "success");
  });

  //list dir TODO subfolders
  server.on("/l", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    Dir root = LittleFS.openDir("/");

    while (root.next()) {
      response->print(root.fileName());
      response->print(",");
      response->println(root.fileSize());
    }
    request->send(response);
  });


  
  server.on("/rm", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    if (!request->hasParam("p")) {
      senderrmesg(request, "Missing Parameter p","/ls");
      return;
    }
    const char *path = request->getParam("p")->value().c_str();
    if (!request->hasParam("c")) {
      request->send(201, "text/html",form("/rm",
      input("p",request->getParam("p")->value(),"hidden")
      +inputnice("c","","Confirm","submit"),"",true));
      return;
    }
    if (!LittleFS.exists(path)) {
      senderrmesg(request, "doesn't exist","/ls");
      return;
    }
    LittleFS.remove(path);
    sendmesg(request, "Success","/ls");
  });
  
  //list dir TODO subfolders
  server.on("/ls", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    AsyncResponseStream *response = request->beginResponseStream("text/html");

    response->print("<form method='POST' action='/u' enctype='multipart/form-data'><input type='file' name='f'><br><input type='submit' value='Upload'><br></form>");


    /*
    struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    size_t blockSize;
    size_t pageSize;
    size_t maxOpenFiles;
    size_t maxPathLength;
};

    */
    FSInfo fs_info;
    LittleFS.info(fs_info);
    response->print(innertag("p","using "+String(fs_info.usedBytes)+" / "+String(fs_info.totalBytes)+" bytes "+String(fs_info.usedBytes*100.f/(float)fs_info.totalBytes)+" %"));
    response->print(innertag("p","blockSize "+String(fs_info.blockSize)+" pageSize "+String(fs_info.pageSize)));
    
    Dir root = LittleFS.openDir("/");
    
    response->print(opentag("table"));
    while (root.next()) {
      response->print(
        opentag("tr")
        +td(ahref("/"+root.fileName(),root.fileName()))
        +td(String(root.fileSize()))
        +td(ahref("/rm?p="+urlencode(root.fileName()),"X"))
        +td(ahref("/mv?p="+urlencode(root.fileName()),"N"))
        +closetag("tr"));
    }
    response->print(closetag("table"));
    response->print(ahref("/","Back"));
    request->send(response);
  });
  
  //upload file
  server.on("/u", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (!checkUserWebAuth(request)) return;
    request->send(200);
  }, [](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!checkUserWebAuth(request)) return;
    logdbg("Client:" + request->client()->remoteIP().toString() + " " + request->url());
    
    if (!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
      
    Serial.printf("%s", (const char*)data);
    
    if (final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
    
    if (!index) {
      // open the file on first call and store the file handle in the request object
      request->_tempFile = myfs->open("/" + filename, "w");
      logdbg("Upload Start: " + String(filename));
    }

    if (len) {
      // stream the incoming chunk to the opened file
      request->_tempFile.write(data, len);
      logdbg("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
    }

    if (final) {
      // close the file handle as the upload is now done
      request->_tempFile.close();
      request->redirect("/");
      logdbg("Upload Complete: " + String(filename) + ",size: " + String(index + len));
    }
  });

}
