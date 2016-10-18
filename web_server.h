#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

namespace WebServer {
  void setup();
  void handle_client();
  void index();
  void json();
  void submit();
  void autotune();
}

//extern float Val_T2_Y[48]={};
//extern int i_s, i_c,i_b_c;

#endif
