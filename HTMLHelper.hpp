String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
    }
    return encodedString;
    
}
String htmstr(const String& s){
  return "\""+s+"\"";
}
String htmattr(const String& k,const String& v){
  return " "+k+"="+htmstr(v);
}
String opentag(const String& t,const String& as=""){
  return "<"+t+as+">";
}
String closetag(const String& t){
  return "</"+t+">";
}
String metarefresh(int time,String url=""){
  String cont = String(time);
  if(url!=""){
    cont+=";url="+url;
  }
  return opentag("meta",htmattr("http-equiv","refresh")+htmattr("content",cont));
}
const String br ="<br>";
String input(const String& name="",const String& value="",const String& type=""){
  String out="<input";
   if(name!=""){
    out+=htmattr("name",name);
   }
   if(type!=""){
    out+=htmattr("type",type);
   }
   if(value!=""){
    out+=htmattr("value",value);
   }
   out+=">";
   return out;
}
String inputnice(const String& name,const String& title="",const String& value="",const String& type=""){
  return title+input(name,value,type)+br;
}
String submit(){
  return input("","","submit");
}
String innertag(const String& t,const String& inner,const String& as=""){
  return opentag(t,as)+inner+closetag(t);
}
String ahref(const String& href,const String& inner){
  return innertag("a",inner,htmattr("href",href));
}
String h1(const String& text){
  return innertag("h1",text);
}
String table(const String& text){
  return innertag("table",text);
}
String tr(const String& text){
  return innertag("tr",text);
}
String td(const String& text){
  return innertag("td",text);
}
String th(const String& text){
  return innertag("th",text);
}



String form(const String& action,const String& inputs,const String& method="",bool nosubmit=false){
   String out="<form"+htmattr("action",action);
   if(method!=""){
    out+=htmattr("method",method);
   }
   out+=">";
   out+=inputs;
   if(!nosubmit)
    out+=submit();
   out+=closetag("form");
   return out;
}
