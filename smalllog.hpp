
#define DBG_CODE
#define DBG_STR

#ifdef DBG_STR
#define logdbg(x) Serial.println(x)
#else
#ifdef DBG_CODE
#define logdbg(x) 
#else
#define logdbg(x)
#endif
#endif
