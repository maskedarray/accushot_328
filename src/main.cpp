#include <Arduino.h>
#include "HX711.h"
#include <imu.h>

#define FORCE_THRESH 1.8  //threshold of detection of peak in grams
#define TIMES_CHECK 6     //times to check if the peak returns to normal. it should ideally be small
                          //greater means that bullet keeps applying pressure for long time.
#define AVG_DIVISOR 3     //number of average values while taking a reading
                          //80 hz means 12.5ms for one reading. thus for 4 readings = 50ms delay.


HX711 scale(5, 6);  //DT, SCK

float calibration_factor = 2230; // this calibration factor is adjusted according to my load cell
float units;
double shot_count = 0;

void setup() {
  Serial.begin(115200);
	mpu_init();       //initialize imu sensor
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  delay(100);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Shots count: ");
  Serial.println(shot_count);

}
long prev_time;
void loop() {
  if(!check_motion()){
    units = scale.get_units(AVG_DIVISOR);   //get average of AVG_DIVISOR readings and convert to grams
    if((units > FORCE_THRESH) && (millis() - prev_time > 60)){ //detected peak
      double prev_shot = shot_count;
      for(int i=0; i<TIMES_CHECK; i++){ //check TIMES_CHECK if peak returns to normal
        units = scale.get_units(AVG_DIVISOR);
        if(units < FORCE_THRESH){ //returned to normal 
          shot_count += 1;
          prev_time = millis();
          Serial.print("Shots count: ");
          Serial.println(shot_count);
          break;  //shot detected so break out of loop
        }
      } //end for
      if(prev_shot == shot_count){  //peak occured but did not return to normal 
        //error occurred. TODO: handle
      }
    }
  } else{
    delay(5000);
  }
}

