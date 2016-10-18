//
//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3;
//static const uint8_t D10  = 1;
//


/*
 * lista comenzi browser""">>>>>>>>>>
 * http://192.168.1.151/json
 * http://192.168.1.151/autotune?enabled=false
 * http://192.168.1.151/autotune?enabled=true
 * 
 * http://192.168.1.151/submit?pid_kd=12
 * 
 * pid_kp=19.56?pid_ki=0.71?pid_kd=134.26
 * 
 * 
 */


#include "wifi_settings.h"
#include "web_server.h"
#include "temperature_sensor.h"
#include "thermostat.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "PID_v1.h"
#include "PID_AutoTune_v0.h"
#include "QueueArray.h"
//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>


int thermoDO = 16;//D0=16;
int thermoCS1 = 5;//D1=5;
int thermoCLK = 4;//D2=6;
int thermoCS2 = 0;//D3   = 0;
int thermoCS3 = 2;//D4   = 2;

MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);
MAX6675 thermocouple3(thermoCLK, thermoCS3, thermoDO);

float temp_c1, temp_c2, temp_c3;  // Values read from sensor
unsigned long next_update_millis = 0;
unsigned long now =0; //millis();
String PrintString="";     // String to display

//extern QueueArray <double> Temp_que;//,Temp_que2,Temp_que3;
extern float temp_c1a;//=[6];
extern float temp_c2a;//=[6];
extern float temp_c3a;//=[6];

//float Val_T2_Y[48]={};
//int i_s, i_c,i_b_c;
 
void setup() {
  WiFi.begin(WIFI_SETTINGS_ESSID, WIFI_SETTINGS_PASSWORD);
  WiFi.waitForConnectResult();
  ArduinoOTA.begin();

  Thermostat::setup();
  TemperatureSensor::setup();
  WebServer::setup();

//    Temp_que::QueueArray ();

    temp_c1 =  thermocouple1.readCelsius()  ;  // Read temperature as Celsius
    temp_c2 =  thermocouple2.readCelsius() ;   // Read temperature as Celsius
    temp_c3 =  thermocouple3.readCelsius() ;   // Read temperature as Celsius
    // Check if any reads failed and exit early (to try again).
//    Temp_que1.enqueue (temp_c1);
//    Temp_que.enqueue (temp_c2);
//    Temp_que1.enqueue (temp_c3);


//mutat in temp_sensor .setup();
//    i_s=0;
//    Val_T2_Y[i_s]=419.6-(temp_c2*380/210);

    
    if (isnan(temp_c1)) {
      Serial.println("Failed to read from Temp_c1 sensor!");
      return;
    }
    if (isnan(temp_c2)) {
      Serial.println("Failed to read from Temp_c2 sensor!");
      return;
    }    
    if (isnan(temp_c3)) {
      Serial.println("Failed to read from Temp_c3 sensor!");
      return;
    }  
}
 
void loop() {
  ArduinoOTA.handle();

  WebServer::handle_client();
  TemperatureSensor::update_occasionally();
  Thermostat::update();
  if (millis() >= next_update_millis) {
//      update();
    temp_c1 =  thermocouple1.readCelsius()  ;  // Read temperature as Celsius
    temp_c2 =  thermocouple2.readCelsius() ;   // Read temperature as Celsius
    temp_c3 =  thermocouple3.readCelsius() ;   // Read temperature as Celsius
    TemperatureSensor::temperature=temp_c2;


//    i_s+=1;
//    if(i_s>47){
//      i_s=0;
//      };
//      i_c=i_s;//locatia ultimei valori
//      i_b_c=i_s;//pct de oprire la generare curba
//      Val_T2_Y[i_s]=419.6-(temp_c2*380/210);


//    Temp_que1.enqueue (temp_c1);
//    Temp_que.enqueue (temp_c2);
//    Temp_que1.enqueue (temp_c3);
//    
    
    // Check if any reads failed and exit early (to try again).
    PrintString=String(millis()) +" || Temperature 1: "+String(temp_c1)+" C "+" || Temperature 2: "+String(temp_c2)+" C "+ " ||  Temperature 3: "+String(temp_c3)+" C"; 
    Serial.println(PrintString);
//    Serial.println(Temp_que1.count());

    
    if (isnan(temp_c1)) {
      Serial.println("Failed Temp_c1!");
      return;
    }
    if (isnan(temp_c2)) {
      Serial.println("Failed Temp_c2");
      return;
    }    
    if (isnan(temp_c3)) {
      Serial.println("Failed Temp_c3 !");
      return;
    }  
      next_update_millis = millis() + 1000;
    }
  
}
