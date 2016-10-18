#include "temperature_sensor.h" //#include "max6675.h"
#include <pgmspace.h>
#include <stdlib.h>
#include "QueueArray.h"

#define UPDATE_TEMPERATURE_SECONDS 1

int thermoDO_1 = 16;//D0=16;
int thermoCS1_1 = 5;//D1=5;
int thermoCLK_1 = 4;//D2=6;
int thermoCS1_2 = 0;//D3   = 0;
int thermoCS1_3 = 2;//D4   = 2;

//extern  float Val_T2_Y[48]={};
//  extern int i_s, i_c,i_b_c;

namespace TemperatureSensor {
  double temperature,temp_c1,temp_c2,temp_c3;
//  extern QueueArray <double> Temp_que;
  unsigned long next_update_millis = 0;



//  MAX6675 thermocouple(thermoCLK_1, thermoCS1_1, thermoDO_1);
  MAX6675 thermocouple1(thermoCLK_1, thermoCS1_1, thermoDO_1);
  MAX6675 thermocouple2(thermoCLK_1, thermoCS1_2, thermoDO_1);
  MAX6675 thermocouple3(thermoCLK_1, thermoCS1_3, thermoDO_1);

//  float Val_T2_Y[48]={};
//  int i_s, i_c,i_b_c;



  void setup() {
//    sensors.begin();
    temp_c1 =  thermocouple1.readCelsius()  ;  // Read temperature as Celsius
//    temperature =  thermocouple.readCelsius()  ;
    temp_c2 =  thermocouple2.readCelsius() ;   // Read temperature as Celsius
    temperature =temp_c2;
    temp_c3 =  thermocouple3.readCelsius() ;   // Read temperature as Celsius
//    Temp_que::QueueArray ();
//    i_s=0;
//    Val_T2_Y[i_s]=419.6-(temp_c2*380/210);
  }

  void update() {
//    sensors.requestTemperatures();
//    temperature = sensors.getTempCByIndex(0);
    temp_c1 =  thermocouple1.readCelsius()  ;  // Read temperature as Celsius
//    Temp_que::push(temp_c1);
//    temperature =  thermocouple.readCelsius()  ;
    temp_c2 =  thermocouple2.readCelsius() ;   // Read temperature as Celsius
    temperature =temp_c2;
    temp_c3 =  thermocouple3.readCelsius() ;   // Read temperature as Celsius
    
  }

  void update_occasionally() {
    unsigned long now =0; //millis();

    if (now >= next_update_millis) {
      update();
      next_update_millis = now + UPDATE_TEMPERATURE_SECONDS * 1000;

//      i_s+=1;
//      if(i_s>47){
//        i_s=0;
//        };
//        i_c=i_s;//locatia ultimei valori
//        i_b_c=i_s;//pct de oprire la generare curba
//        Val_T2_Y[i_s]=419.6-(temp_c2*380/210);
//    Temp_que::enqueue(temp_c2);

      
    }
  }
}




MAX6675::MAX6675(int8_t SCLK, int8_t CS, int8_t MISO) {
  sclk = SCLK;
  cs = CS;
  miso = MISO;

  //define pin modes
  pinMode(cs, OUTPUT);
  pinMode(sclk, OUTPUT); 
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);
}

double MAX6675::readCelsius(void) {

  uint16_t v;

  digitalWrite(cs, LOW);
 // delay(1);

  v = spiread();
  v <<= 8;
  v |= spiread();

  digitalWrite(cs, HIGH);

  if (v & 0x4) {
    // uh oh, no thermocouple attached!
    return 0; //return 0 for browser inchoerence? //NaN
    //return -100;
  }

  v >>= 3;

  return v*0.25;
}

byte MAX6675::spiread(void) { 
  int i;
  byte d = 0;

  for (i=7; i>=0; i--)
  {
    digitalWrite(sclk, LOW);
   // _delay_ms(1);
    //delay(1);
    if (digitalRead(miso)) {
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    digitalWrite(sclk, HIGH);
   // delay(1);
  }

  return d;
}
