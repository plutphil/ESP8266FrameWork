String readLineFile(File& f){
  return f.readStringUntil('\n');
}

bool str2int (int &i, char const *s, int base = 0)
{
    char *end;
    long  l;
    l = strtol(s, &end, base);
    
    if (*s == '\0' || *end != '\0') {
        return false;
    }
    i = l;
    return true;
}

class Config{
  public:
  String path;
  String rkey;
  String rval;
  String line;
  File configFile;
  
  Config(String path_="/config.txt"):path(path_){
  }
  bool openr(){
    configFile = myfs->open(path, "r");
    if(!configFile){
      logdbg("error opening file read");
      return false;
    }
    return true;
  }
  bool openw(){
    configFile = myfs->open(path, "w");
    if(!configFile){
      logdbg("error opening file write");
      return false;
    }
    return true;
  }
  bool opena(){
    configFile = myfs->open(path, "a");
    if(!configFile){
      logdbg("error opening file append");
    }
    if(!configFile){
      logdbg("error opening file append write");
      configFile = myfs->open(path, "w");
      return false;
    }
    return true;
  }
  void close(){
    configFile.close();
  }
  void split(const String& line,String& k,String& v,String del="="){
    int eqi = line.indexOf(del);
    if(eqi!=-1){
      k = line.substring(0,eqi);
      v = line.substring(eqi+1);
    }else{
      k="";
      v="";
    }
  }
  void readkeyvalue(){
      line = readLineFile(configFile);
      split(line,rkey,rval); 
  }
  String get(const String& key){
    String val="";
    contains(key,val);
    return val;
  }
  bool set(const String& key,const String& val){
    String rval="";
    bool cont = contains(key,rval);
    
    if(!cont){
      add(key,val);
      return true;
    }
    if(rval.equals(val)){
      return true;
    }
    logdbg("rename");
    Serial.println(myfs->rename(path,path+".old"));
    
    openw();
    
    File oldConfigFile = myfs->open(path+".old", "r");
    if(!oldConfigFile){
      logdbg("error opening file read set");
      return false;
    }
    while(oldConfigFile.available()){
      String   line = readLineFile(oldConfigFile);
      int eqi = line.indexOf("=");
      if(eqi!=-1){
        String rkey = line.substring(0,eqi);
        String rval = line.substring(eqi+1);
        if(rkey.equals(key)){
          configFile.print(key);
          configFile.print("=");
          configFile.print(val);
          configFile.print("\n");
        }else{
          configFile.print(rkey);
          configFile.print("=");
          configFile.print(rval);
          configFile.print("\n");
        }
      }
    }
    close();
    oldConfigFile.close();
  }
  void add(const String& key,const String& val){
    opena();
    configFile.print(key);
    configFile.print("=");
    configFile.print(val);
    configFile.print("\n");
    close();
  }
  String getSetDefault(const String& key,const String& def){
    String rval="";
    bool cont = contains(key,rval);
    if(cont){
      return rval; 
    }
    add(key,def);
    return def;
  }
  int getSetDefaultInt(const String& key,const int& def){
    String rval="";
    bool cont = contains(key,rval);
    if(cont){
      int ret;
      if(str2int(ret,rval.c_str())){
        return ret; 
      }else{
        return def;
      }
    }
    add(key,String(def));
    return def;
  }
  bool contains(const String& key,String& val){
    if(!openr())return false;
    while(configFile.available()){
      readkeyvalue();
      if(rkey.equals(key)){
        close();
        val = rval;
        return true;
      }
    }
    close();
    return false;
  }
  bool getKey(const String& val,String& key){
    if(!openr())return false;
    while(configFile.available()){
      readkeyvalue();
      if(rval.equals(val)){
        close();
        key = rkey;
        return true;
      }
    }
    close();
    return false;
  }
  String getKey(const String& val){
    String key="";
    getKey(val,key);
    return key;
  }
};
Config configtxt=Config();
String getConfig(String key,String path="/config.txt"){
  return configtxt.get(key);
}
String getConfigByVal(String val,String path="/config.txt"){
  return configtxt.getKey(val);
}
void addConfig(String key,String val,String path="/config.txt"){
  configtxt.add(key,val);
}
void setConfig(String key,String val,String path="/config.txt"){
  configtxt.set(key,val);
}

String getAddDefault(String key,String val,String path="/config.txt"){
  return configtxt.getSetDefault(key,val);
}

void config_setup(){
  if (!myfs->begin()) {
    //Serial.println("Failed to mount file system");
    return;
  }
  Serial.printf("Listing directory: %s\n", "/");

  Dir root = LittleFS.openDir("/");

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.print(file.size());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}
