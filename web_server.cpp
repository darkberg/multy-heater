#include "web_server.h"
#include "temperature_sensor.h"
#include "thermostat.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "QueueArray.h"
#include <stdio.h>

#define DOUBLE_MESSAGES 9
String webString="";     // String to display
String webString1="";
String svgout = "";
String Curba_T1 ="";
String Curba_T2 ="";
String Curba_T3 ="";
String Curba_SP ="";
String Curba_HON ="";
String Poz_X[48]={"M 790.0","L 774.4","L 758.8","L 743.1","L 727.5","L 711.9","L 696.3","L 680.6","L 665.0","L 649.4","L 633.8","L 618.1","L 602.5","L 586.9","L 571.3","L 555.6","L 540.0","L 524.4","L 508.8","L 493.1","L 477.5","L 461.9","L 446.3","L 430.6","L 415.0","L 399.4","L 383.8","L 368.1","L 352.5","L 336.9","L 321.3","L 305.6","L 290.0","L 274.4","L 258.8","L 243.1","L 227.5","L 211.9","L 196.3","L 180.6","L 165.0","L 149.4","L 133.8","L 118.1","L 102.5","L 86.9","L 71.3","L 55.6"};
float Val_T1_Y[48]={};
float Val_T2_Y[48]={};
float Val_T3_Y[48]={};
float Val_SP_Y[48]={};
float Val_HON_Y[48]={};
int i_s=0;
int i_c=0;
int i_b_c=0;
char temp[100];
int tcount;
extern QueueArray <double> Temp_que;//,Temp_que2,Temp_que3;
bool HON;
float temp_SP=0;

namespace WebServer {
  ESP8266WebServer server(80);

void handleRoot() {
  //digitalWrite ( led, 1 );
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  TemperatureSensor::update();
  
  int temp1_i=TemperatureSensor::temp_c1;
  int temp1_f=(TemperatureSensor::temp_c1-temp1_i)*100;
  int temp2_i=TemperatureSensor::temp_c2;
  int temp2_f=(TemperatureSensor::temp_c2-temp2_i)*100;
  int temp3_i=TemperatureSensor::temp_c3;
  int temp3_f=(TemperatureSensor::temp_c3-temp3_i)*100;

//    Temp_que::QueueArray ();
//    i_s=0;
//    Val_T2_Y[i_s]=419.6-(TemperatureSensor::temp_c2*380/210);

  snprintf ( temp, 400,"<html><head><meta http-equiv='refresh' content='5'/><title>Reg & viz temperatura</title><style> body { background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: #f67400; }</style></head><body><h1>Temperatures PID Controller </h1><p>Uptime: %02d:%02d:%02d</p><h1>Temp1: %03d.%02d  C  || Temp2:  %03d.%02d  C ||  Temp3: %03d.%02d  C </h1><img src=\"/test.svg\" /></body></html>",

    hr, min % 60, sec % 60,temp1_i,temp1_f,temp2_i,temp2_f,temp3_i,temp3_f
  );

  server.send ( 200, "text/html", temp );
  //digitalWrite ( led, 0 );
}

void handleNotFound() {
  //digitalWrite ( led, 1 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
//  digitalWrite ( led, 0 );
}


  void setup() {
    server.on("/old", index);
    server.on("/submit", submit);
    server.on("/autotune", autotune);
    server.on("/json", json);
    server.on ( "/", handleRoot );
    server.on ( "/test.svg",[](){  
      svgout = "";
      svgout += "<?xml version=\"1.0\" encoding=\"utf-8\"?><svg viewBox=\"0 0 800 460\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:bx=\"https://www.boxy-svg.com/bx\"><rect width=\"800\" height=\"460\" style=\"fill: rgb(1, 1, 1);\"/>";
      svgout += "<rect x=\"40\" y=\"39.6\" width=\"750\" height=\"380\" style=\"fill-opacity: 0.2; stroke: rgb(105, 105, 104);\"/>";
      svgout += "<text x=\"168.7\" y=\"26.8\" style=\"font-size: 16px; font-family: Arial; fill: rgb(251, 251, 251); word-spacing: 0px;\"> Valori temperaturi / stare Regulator PID  </text>";
      svgout +="<text x=\"580\" y=\"25\" style=\"font-size: 12px; fill: rgb(253, 200, 39);\">T1</text><text x=\"600\" y=\"25\" style=\"font-size: 12px; fill: rgb(33, 125, 245); \">T2</text><text x=\"620\" y=\"25\" style=\"font-size: 12px; fill: rgb(153, 20, 239);\">T3</text><text x=\"640\" y=\"25\" style=\"font-size: 12px; fill: rgb(53, 255, 2);\">SP</text><text x=\"680\" y=\"25\" style=\"font-size: 12px; fill: rgb(255, 0, 0);\">Heat ON</text>";
      svgout +="<g class=\"y-axis\"><text y=\"420\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">0</text><text y=\"380\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">20</text><text y=\"340\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">40</text><text y=\"300\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">60</text><text y=\"260\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">80</text><text y=\"220\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">120</text><text y=\"180\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">140</text><text y=\"140\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">160</text><text y=\"100\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">180</text><text y=\"60\" x=\"20\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">200</text></g>";
      svgout +="<g class=\"x-axis\" transform=\"matrix(1, 0, 0, 1, 32, 12)\"><text y=\"430\" x=\"40\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-60</text><text y=\"430\" x=\"87.4\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-56</text><text y=\"430\" x=\"134.7\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-52</text><text y=\"430\" x=\"182\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-48</text><text y=\"430\" x=\"229.3\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-44</text><text y=\"430\" x=\"276.6\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-40</text><text y=\"430\" x=\"324\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-36</text><text y=\"430\" x=\"371.3\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-32</text><text y=\"430\" x=\"418.7\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-28</text><text y=\"430\" x=\"466\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-24</text><text y=\"430\" x=\"513.3\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-20</text><text y=\"430\" x=\"560.6\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-16</text><text y=\"430\" x=\"608\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-12</text><text y=\"430\" x=\"655.3\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-8</text><text y=\"430\" x=\"702.6\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">-4</text><text y=\"430\" x=\"750\" style=\"text-anchor: middle; fill: rgb(103, 102, 102); font-size: 12px;\">0</text></g>";
 //Begin curba T1      
//      svgout +="<path d=\"M 790.0 368.0 L 774.4 347.8 L 758.8 325.7 L 743.1 301.8 L 727.5 276.1 L 711.9 248.6 L 696.3 219.3 L 680.6 188.2 L 665.0 155.2 L 649.4 120.5 L 633.8 83.9 L 618.1 45.6 L 602.5 58.1 L 586.9 68.7 L 571.3 419.6 L 555.6 88.5 L 540.0 95.5 L 524.4 100.8 L 508.8 104.2 L 493.1 105.8 L 477.5 105.6 L 461.9 103.7 L 446.3 99.9 L 430.6 94.2 L 415.0 86.8 L 399.4 99.3 L 383.8 110.0 L 368.1 118.9 L 352.5 125.9 L 336.9 131.2 L 321.3 134.6 L 305.6 136.2 L 290.0 136.0 L 274.4 134.1 L 258.8 130.3 L 243.1 124.6 L 227.5 117.2 L 211.9 108.0 L 196.3 97.0 L 180.6 84.1 L 165.0 69.5 L 149.4 53.0 L 133.8 34.7 L 118.1 47.2 L 102.5 57.9 L 86.9 66.7 L 71.3 73.8 L 55.6 79.0\" style=\"stroke: rgb(253, 200, 39); stroke-width: 1; fill: none;\" bx:origin=\"0.5 0.5\"/>";
// End curba T1
//Begin curba T2 
//      svgout +="<path d=\"M 790.0  151.6 L 774.4  419.3 L 758.8  265.6 L 743.1  281.7 L 727.5  303.6 L 711.9  150.6 L 696.3  214.9 L 680.6  258.4 L 665.0  257.1 L 649.4  47.1 L 633.8  75.2 L 618.1  271.8 L 602.5  150.9 L 586.9  44.8 L 571.3  200.6 L 555.6  316.6 L 540.0  403.6 L 524.4  71.8 L 508.8  315.0 L 493.1  72.9 L 477.5  238.0 L 461.9  93.4 L 446.3  233.1 L 430.6  214.5 L 415.0  281.2 L 399.4  84.2 L 383.8  318.0 L 368.1  378.4 L 352.5  193.1 L 336.9  381.1 L 321.3  186.0 L 305.6  387.5 L 290.0  402.0 L 274.4  285.6 L 258.8  55.5 L 243.1  235.8 L 227.5  387.4 L 211.9  270.8 L 196.3  56.5 L 180.6  379.9 L 165.0  307.0 L 149.4  60.9 L 133.8  106.9 L 118.1  269.6 L 102.5  53.5 L 86.9  382.7 L 71.3  179.6 L 55.6  310.5 \" style=\"stroke: rgb(33, 125, 245); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
// End curba T2
//Begin curba T3
//svgout +="<path d=\"M 790.0 88.6 L 774.4 87.2 L 758.8 114.9 L 743.1 410.3 L 727.5 224.0 L 711.9 112.1 L 696.3 161.6 L 680.6 369.8 L 665.0 391.1 L 649.4 330.8 L 633.8 414.0 L 618.1 278.6 L 602.5 80.2 L 586.9 95.6 L 571.3 223.9 L 555.6 352.5 L 540.0 334.6 L 524.4 179.6 L 508.8 210.3 L 493.1 153.1 L 477.5 104.1 L 461.9 217.5 L 446.3 283.5 L 430.6 289.8 L 415.0 235.7 L 399.4 279.0 L 383.8 277.9 L 368.1 62.9 L 352.5 391.1 L 336.9 257.7 L 321.3 66.7 L 305.6 152.6 L 290.0 148.6 L 274.4 134.9 L 258.8 91.6 L 243.1 288.7 L 227.5 134.0 L 211.9 346.2 L 196.3 75.6 L 180.6 319.7 L 165.0 229.0 L 149.4 206.0 L 133.8 277.2 L 118.1 402.7 L 102.5 396.6 L 86.9 77.5 L 71.3 322.9 L 55.6 101.5  \" style=\"stroke: rgb(153, 20, 239); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
// End curba T3
      //realizare val curba pentru T1/2 &3
//      tcount=0;
      i_s+=1;
      if(i_s>47){
        i_s=0;
        };
      i_c=i_s;//locatia ultimei valori
      i_b_c=i_s;//pct de oprire la generare curba
      Val_T1_Y[i_s]=419.6-(TemperatureSensor::temp_c1*380/210);      
      Val_T2_Y[i_s]=419.6-(TemperatureSensor::temp_c2*380/210);
      Val_T3_Y[i_s]=419.6-(TemperatureSensor::temp_c3*380/210);    
      temp_SP=Thermostat::settings.temperature;
      Val_SP_Y[i_s]=419.6-(temp_SP*380/210); 
      HON=Thermostat::heat_on;
      if(HON){
        Val_HON_Y[i_s]=59.6;
        }
      if(!HON){
        Val_HON_Y[i_s]=400.6;
         
        }
//      Serial.print("heat on ? -->>");Serial.print(HON);Serial.print("   "); Serial.print("val heat on ? -->>");Serial.print(Val_HON_Y[i_s]);Serial.print("   ");
//      Serial.print("Setpoint ? -->>");Serial.print(temp_SP);Serial.print("   ");Serial.print(Val_SP_Y[i_s]);Serial.print("   ");
//      

      

//        Verificare index + valori
//      Serial.print("valoare i_s :");Serial.println(i_s);
//      Serial.print(" :Val_T2_Y [%d]");
//      Serial.println(Val_T2_Y[i_s]);

//Curba T1
      svgout +="<path d=\"";
      Curba_T1 ="";
      for (int y = 0; y < 48; y+= 1) {
        Curba_T1 +=" "+ String(Poz_X[y])+" ";
        if ((i_b_c - y)>=0) {
          Curba_T1 +=String(Val_T1_Y[i_b_c - y]);
        }
        if ((i_b_c - y)<0) {
          Curba_T1 +=String(Val_T1_Y[i_b_c - y+48]);
        }
      } 
      svgout+=Curba_T1+" \" style=\"stroke: rgb(253, 200, 39); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
//End Curba T1
// curba T2     
      svgout +="<path d=\"";
      Curba_T2 ="";
      for (int y = 0; y < 48; y+= 1) {
        Curba_T2 +=" "+ String(Poz_X[y])+" ";
        if ((i_b_c - y)>=0) {
          Curba_T2 +=String(Val_T2_Y[i_b_c - y]);
        }
        if ((i_b_c - y)<0) {
          Curba_T2 +=String(Val_T2_Y[i_b_c - y+48]);
        }
      } 
      svgout+=Curba_T2+" \" style=\"stroke: rgb(33, 125, 245); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
//End Curba T2      
// curba T3     
      svgout +="<path d=\"";
      Curba_T3 ="";
      for (int y = 0; y < 48; y+= 1) {
        Curba_T3 +=" "+ String(Poz_X[y])+" ";
        if ((i_b_c - y)>=0) {
          Curba_T3 +=String(Val_T3_Y[i_b_c - y]);
        }
        if ((i_b_c - y)<0) {
          Curba_T3 +=String(Val_T3_Y[i_b_c - y+48]);
        }
      } 
      svgout+=Curba_T3+" \" style=\"stroke: rgb(153, 20, 239); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
//End Curba T3 

//Curba SP
      svgout +="<path d=\"";
      Curba_SP ="";
      for (int y = 0; y < 48; y+= 1) {
        Curba_SP +=" "+ String(Poz_X[y])+" ";
        if ((i_b_c - y)>=0) {
          Curba_SP +=String(Val_SP_Y[i_b_c - y]);
        }
        if ((i_b_c - y)<0) {
          Curba_SP +=String(Val_SP_Y[i_b_c - y+48]);
        }
      } 
      svgout+=Curba_SP+" \" style=\"stroke: rgb(53, 255, 2); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
//End curba SP
//Curba SP _old
//      svgout +="<path d=\"M 790.0 93.9 L 55.6 93.9 \" style=\"stroke: rgb(53, 255, 2); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
// Enc Curba SP_old

//Curba Heat_on
      svgout +="<path d=\"";
      Curba_HON ="";
      for (int y = 0; y < 48; y+= 1) {
        Curba_HON +=" "+ String(Poz_X[y])+" ";
        if ((i_b_c - y)>=0) {
          Curba_HON +=String(Val_HON_Y[i_b_c - y]);
        }
        if ((i_b_c - y)<0) {
          Curba_HON +=String(Val_HON_Y[i_b_c - y+48]);
        }
      } 
      svgout+=Curba_HON+" \" style=\"stroke: rgb(255, 0, 0); fill: none; stroke-width: 1;\" bx:origin=\"0.5 0.5\"/>";
//End Curba Heat_on

//Curba Heat_on _OLD      
//      svgout +="<path d=\"M 790.0  419.6 L 774.4  419.6 L 758.8  419.6 L 743.1  419.6 L 727.5  419.6 L 711.9  39.6 L 696.3  419.6 L 680.6  419.6 L 665.0  39.6 L 649.4  419.6 L 633.8  419.6 L 618.1  419.6 L 602.5  419.6 L 586.9  39.6 L 571.3  39.6 L 555.6  419.6 L 540.0  39.6 L 524.4  419.6 L 508.8  419.6 L 493.1  419.6 L 477.5  39.6 L 461.9  419.6 L 446.3  39.6 L 430.6  419.6 L 415.0  419.6 L 399.4  39.6 L 383.8  39.6 L 368.1  39.6 L 352.5  419.6 L 336.9  419.6 L 321.3  39.6 L 305.6  419.6 L 290.0  39.6 L 274.4  419.6 L 258.8  39.6 L 243.1  39.6 L 227.5  39.6 L 211.9  419.6 L 196.3  419.6 L 180.6  419.6 L 165.0  39.6 L 149.4  419.6 L 133.8  39.6 L 118.1  39.6 L 102.5  419.6 L 86.9  419.6 L 71.3  419.6 L 55.6  419.6  \" style=\"stroke: rgb(255, 0, 0); fill: none; stroke-width: 0.5;\" bx:origin=\"0.5 0.5\"/>";
//End Curba Heat_on _OLD



      
      svgout += "\n</svg>\n";  
      server.send ( 200, "image/svg+xml", svgout);

    });
    server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
    server.on("/temp", [](){  // if you add this subdirectory to your webserver call, you get text below :)
     server.send(200, "text/plain", webString);            // send to someones browser when asked
    });  
    server.begin();
    //
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  Serial.println("");
  Serial.println("Temperature Reading Server");
  Serial.print("Connected to ");
  //Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    //
  }


  void handle_client() {
    server.handleClient();
  }

  String bool_to_string(float bool_value) {
    return String(bool_value ? "true" : "false");
  }

  String bool_to_checked(float bool_value) {
    return String(bool_value ? " checked" : "");
  }

  void index() {

    webString1="<!DOCTYPE html><html><head><title>oops PID</title></head><body><table><tr><td>Sensor temperature:</td><td>" + String(TemperatureSensor::temperature);
    webString1+="</td></tr><tr><td>PID output:</td><td>" + String(Thermostat::pid_output_average)+ "</td></tr><tr><td>Heat on:</td><td><input type=\"checkbox\" disabled(" + bool_to_checked(Thermostat::heat_on)+")";
    webString1+="></td></tr><tr><td>Autotune running:</td><td><input type=\"checkbox\" disabled(" + bool_to_checked(Thermostat::autotune_running)+")";
    webString1+="></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr><tr><td>Thermostat enabled:</td><td><input id=\"enabled\" name=\"enabled\" type=\"checkbox\"(";
    webString1+= bool_to_checked(Thermostat::settings.enabled) + ")></td></tr><tr><td>Temperature:</td><td><input id=\"temperature\" name=\"temperature\" type=\"number\" step=\"0.1\" value="+ String(Thermostat::settings.temperature);
    webString1+="></td></tr><tr><td>Heat on threshold:</td><td><input id=\"heat_on_threshold\" name=\"heat_on_threshold\"type=\"number\" step=\"0.1\" value="; 
    webString1+=String(Thermostat::settings.heat_on.threshold) + "></td></tr><tr><td>Heat on delta:</td><td><input id=\"heat_on_delta\" name=\"heat_on_delta\" type=\"number\" step=\"0.1\" value=";
    webString1+=String(Thermostat::settings.heat_on.delta) + "></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr><tr><td>PID Kp:</td><td><input id=\"pid_kp\" name=\"pid_kp\" type=\"number\" step=\"0.1\" value=";
    webString1+=String( Thermostat::settings.pid.kp) + "></td></tr><tr><td>PID Ki:</td><td><input id=\"pid_ki\" name=\"pid_ki\" type=\"number\" step=\"0.1\" value=";
    webString1+=String(Thermostat::settings.pid.ki) + "></td></tr><tr><td>PID Kd:</td><td><input id=\"pid_kd\" name=\"pid_kd\" type=\"number\" step=\"0.1\"  value=" ;
    webString1+=String(Thermostat::settings.pid.kd) + "></td></tr><tr><td>PID window seconds:</td><td><input id=\"pid_window_seconds\" name=\"pid_window_seconds\" type=\"number\" step=\"0.1\" value=";
    webString1+="></td></tr><tr><td colspan=\"2\"></td></tr><tr><td>PID autotune lookback minutes:</td><td><input id=\"pid_autotune_lookback_minutes\" name=\"pid_autotune_lookback_minutes\" type=\"number\" step=\"0.1\" value=" ;    
//    webString1+=String(Thermostat::settings.pid.window_seconds) + "(\"></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr><tr><td>PID autotune lookback minutes:</td><td><input id=\"pid_autotune_lookback_minutes\" name=\"pid_autotune_lookback_minutes\" type=\"number\" step=\"0.1\" value=\")" ;
    webString1+="></td></tr><tr><td>PID autotune noise band:</td><td><input id=\"pid_autotune_noise_band\" name=\"pid_autotune_noise_band\" type=\"number\" step=\"0.1\" value=" ;
//
//    webString1+=String(Thermostat::settings.pid_autotune.lookback_minutes) +"(\"></td></tr><tr><td>PID autotune noise band:</td><td><input id=\"pid_autotune_noise_band\" name=\"pid_autotune_noise_band\" type=\"number\" step=\"0.1\" value=\"(" ;
    webString1+=String(Thermostat::settings.pid_autotune.noise_band) + "></td></tr><tr><td colspan=\"2\"></td></tr><tr><td colspan=\"2\"><button id=\"submit\">Submit</button></td></tr><tr><td colspan=\"2\"><button id=\"start_autotune\">Start autotune</button></td></tr><tr><td colspan=\"2\"><button id=\"stop_autotune\">Stop autotune</button></td></tr></table></body></html>";
  
    webString1+="<script>document.getElementById(\"submit\").onclick = function() {var xhttp = new XMLHttpRequest();xhttp.open(\"POST\", \"/submit\", true);xhttp.send(\"enabled=\" + document.getElementById(\"enabled\").checked +\"&temperature=\" + document.getElementById(\"temperature\").value";
    webString1+="+\"&pid_kp=\" + document.getElementById(\"pid_kp\").value +\"&pid_ki=\" + document.getElementById(\"pid_ki\").value +\"&pid_kd=\" + document.getElementById(\"pid_kd\").value +\"&pid_window_seconds=\" + document.getElementById(\"pid_window_seconds\").value +\"&heat_on_threshold=\" + document.getElementById(\"heat_on_threshold\").value";
    webString1+="+\"&heat_on_delta=\" + document.getElementById(\"heat_on_delta\").value +\"&pid_autotune_lookback_minutes=\" + document.getElementById(\"pid_autotune_lookback_minutes\").value +\"&pid_autotune_noise_band=\" + document.getElementById(\"pid_autotune_noise_band\").value)}";
    webString1+="document.getElementById(\"start_autotune\").onclick = function() {var xhttp = new XMLHttpRequest(); xhttp.open(\"POST\", \"/autotune\", true); xhttp.send(\"enabled=true\");}";
    webString1+="document.getElementById(\"stop_autotune\").onclick = function() {var xhttp = new XMLHttpRequest(); xhttp.open(\"POST\", \"/autotune\", true); xhttp.send(\"enabled=false\");}</script>";

    server.send(200, "text/html", webString1);            // send to someones browser when asked
  }

  void json() {
    server.send(200, "application/json", String(R"({"temperature SP": )" + String(Thermostat::settings.temperature)
    + R"(,"sensor_temperature": )" + String(TemperatureSensor::temperature)
    + R"(,"enabled": )" + bool_to_string(Thermostat::settings.enabled)
    + R"(,"heat_on": )" + bool_to_string(Thermostat::heat_on)
    + R"(,"pid_kp": )" + Thermostat::settings.pid.kp
    + R"(,"pid_ki": )" + Thermostat::settings.pid.ki
    + R"(,"pid_kd": )" + Thermostat::settings.pid.kd
    + R"(,"pid_output": )" + Thermostat::pid_output_average

//urmatoarele 3 baga in WDT reset
//    + R"(,"settings.pid.window_seconds": )" + Thermostat::settings.pid.window_seconds
//    + R"(,"pid_autotune.lookback_minutes": )" + Thermostat::settings.pid_autotune.lookback_minutes
//    + R"(,"pid_autotune.noise_band": )" +    Thermostat::settings.pid_autotune.noise_band
    
    
    + "}"));
  }

  void autotune() {
    if (server.arg("enabled").equals("true")) {
      Thermostat::start_autotune();
      server.send(200, "text/plain", "Autotune started.");
    } else if (server.arg("enabled").equals("false")) {
      Thermostat::cancel_autotune();
      server.send(200, "text/plain", "Autotune stopped.");
    } else {
      server.send(
          400, "text/plain", "\"enabled\" must be \"true\" or \"false\".");
    }
  }



  void submit() {
    String response_ok;
    String response_error;
    Thermostat::settings_struct new_settings = Thermostat::settings;

    for (int args_i = 0; args_i < server.args(); args_i++) {
      String line_ok;
      String line_error;
      String arg_name = server.argName(args_i);
      String arg_value = server.arg(args_i);

      if (arg_name.equals("enabled")) {
        if (arg_value.equals("true") or arg_value.equals("false")) {
          new_settings.enabled = server.arg("enabled").equals("true");

          line_ok = String(
              "Thermostat " +
              String(new_settings.enabled ? "enabled" : "disabled"));
        } else {
          line_error = String(
              "must be either \"true\" or \"false\", not " + arg_value);
        }
      } else {
        double arg_value_double = (double)arg_value.toFloat();

        if (arg_value_double < 0) {
          line_error = String(
              "must be a positive number (given " + arg_value + ").");
        } else if (arg_name.equals("temperature")) {
          new_settings.temperature = arg_value_double;
          line_ok += "Temperature";
        } else if (arg_name.equals("pid_kp")) {
          new_settings.pid.kp = arg_value_double;
          line_ok += "PID Kp";
        } else if (arg_name.equals("pid_ki")) {
          new_settings.pid.ki = arg_value_double;
          line_ok += "PID Ki";
        } else if (arg_name.equals("pid_kd")) {
          new_settings.pid.kd = arg_value_double;
          line_ok += "PID Kd";
        } else if (arg_name.equals("pid_window_seconds")) {
          new_settings.pid.window_seconds = arg_value_double;
          line_ok += "PID window seconds";
        } else if (arg_name.equals("heat_on_threshold")) {
          new_settings.heat_on.threshold = arg_value_double;
          line_ok += "Heat on threshold";
        } else if (arg_name.equals("heat_on_delta")) {
          new_settings.heat_on.delta = arg_value_double;
          line_ok += "Heat on delta";
        } else if (arg_name.equals("heat_on_delta")) {
          new_settings.heat_on.delta = arg_value_double;
          line_ok += "Heat on delta";
        } else if (arg_name.equals("pid_autotune_lookback_minutes")) {
          new_settings.pid_autotune.lookback_minutes = arg_value_double;
          line_ok += "PID autotune lookback minutes";
        } else if (arg_name.equals("pid_autotune_noise_band")) {
          new_settings.pid_autotune.noise_band = arg_value_double;
          line_ok += "PID autotune noise band";
        }

        if (line_ok.length()) {
          line_ok += " set to " + String(arg_value_double);
        }
      }

      if (! line_ok.length() and ! line_error.length()) {
        line_error = String("invalid argument");
      }

      if (line_error.length()) {
        if (response_error.length()) {
          response_error += "\n";
        }
        response_error += String(arg_name + ": " + line_error);
      }

      if (line_ok.length()) {
        if (response_ok.length()) {
          response_ok += "\n";
        }
        response_ok += String(line_ok + ".");
      }
    }

    if (response_error.length()) {
      server.send(400, "text/plain", response_error);
      return;
    } else if (response_ok.length()) {
      Thermostat::apply_settings(new_settings);
      server.send(200, "text/plain", response_ok);
    } else {
      server.send(400, "text/plain", "No arguments submitted.");
    }
  }




  
} /// end 


