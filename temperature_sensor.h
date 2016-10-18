#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MAX6675 {
 public:
  MAX6675(int8_t SCLK, int8_t CS, int8_t MISO);

  double readCelsius(void);
  private:
   int8_t sclk, miso, cs;
   uint8_t spiread(void);
};

namespace TemperatureSensor {
  extern double temperature,temp_c1,temp_c2,temp_c3;
//  extern float Val_T2_Y[48]={};
//  extern int i_s, i_c,i_b_c;
  void setup();
  void update();
  void update_occasionally();
}

#endif
