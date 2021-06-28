#include <Arduino.h>
#include "HX711.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

#define FORCE_THRESH 1.8  //threshold of detection of peak in grams
#define TIMES_CHECK 3     //times to check if the peak returns to normal. it should ideally be small
                          //greater means that bullet keeps applying pressure for long time.
#define AVG_DIVISOR 3     //number of average values while taking a reading
                          //80 hz means 12.5ms for one reading. thus for 4 readings = 50ms delay.

HX711 scale(5, 6);  //DT, SCK

float calibration_factor = 2230; // this calibration factor is adjusted according to my load cell
float units;
double shot_count = 0;

void setup() {
  Serial.begin(9600);
  if (!mpu.begin()) {
		Serial.println("Failed to find MPU6050 chip");
		while (1) {
		  delay(10);
		}
	}
	Serial.println("MPU6050 Found!");
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  delay(100);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Shots count: ");
  Serial.println(shot_count);

}

void loop() {
  units = scale.get_units(AVG_DIVISOR);
  if(units > FORCE_THRESH){ //detected peak
    double prev_shot = shot_count;
    for(int i=0; i<TIMES_CHECK; i++){ //check twice if peak returns to normal
      units = scale.get_units(AVG_DIVISOR);
      if(units < FORCE_THRESH){ //returned to normal 
        shot_count += 1;
        Serial.print("Shots count: ");
        Serial.println(shot_count);
        break;  //shot detected so break out of loop
      }
    } //end for
    if(prev_shot == shot_count){  //peak occured but did not return to normal 
      //error occurred. TODO: handle
    }
  }
}