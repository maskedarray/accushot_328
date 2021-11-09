#include <Arduino.h>
#include "HX711.h"
#include <imu.h>
#include <filters.h>
#include "LedControl.h"
//#include <EnableInterrupt.h>

#define FORCE_THRESH 15 * 10  //threshold of detection of peak in grams
#define TIMES_CHECK 4     //times to check if the peak returns to normal. it should ideally be small
                          //greater means that bullet keeps applying pressure for long time.
#define AVG_DIVISOR 1     //number of average values while taking a reading
                          //80 hz means 12.5ms for one reading. thus for 4 readings = 50ms delay.

LedControl lc=LedControl(11,13,10,1);
HX711 scale(5, 6);  //DT, SCK

// Creating high-pass filter; maximum order is 2 
const float cutoff_freq   = 25.0;   //Cutoff frequency in Hz
const float sampling_time = 0.0125; //Sampling time in seconds.
Filter fhp(cutoff_freq, sampling_time, IIR::ORDER::OD2, IIR::TYPE::HIGHPASS);
void setCurrentDigits();
void setSetDigits();

float calibration_factor = 2230; // this calibration factor is adjusted according to my load cell
float units;
double shot_count = 0;
double shot_set = 10;

void up_int(){
  if(shot_set < 99)
    shot_set++;
  setSetDigits();
}

void dn_int(){
  if(shot_set > 1)
    shot_set = shot_set -1;
  setSetDigits();
}

void setup() {
  Serial.begin(115200);
	//mpu_init();       //initialize imu sensor
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  delay(100);
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  delay(100);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Shots count: ");
  Serial.println(shot_count);
  setCurrentDigits();
  setSetDigits();

  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  // enableInterrupt(3,dn_int,FALLING);
  // enableInterrupt(4,up_int,FALLING);
}
long prev_time;
void loop() {
  // if(!check_motion()){
    units = scale.get_units(AVG_DIVISOR);   //get average of AVG_DIVISOR readings and convert to grams
    units = fhp.filterIn(units*10);
    //Serial.print(units);
    // Serial.print(",");
    // Serial.print(300);
    // Serial.print(",");
    // Serial.println(-300);
    if(((units > FORCE_THRESH) || (units < (-FORCE_THRESH))) && (millis() - prev_time > 100)){ //detected peak
      double prev_shot = shot_count;
      // for(int i=0; i<TIMES_CHECK; i++){ //check TIMES_CHECK if peak returns to normal
        // units = scale.get_units(AVG_DIVISOR);
        // if(units < FORCE_THRESH){ //returned to normal 
          shot_count += 1;
          prev_time = millis();
          Serial.print("Shots count: ");
          Serial.println(shot_count);
          setCurrentDigits();
          // break;  //shot detected so break out of loop
        // }
      // } //end for
      // if(prev_shot == shot_count){  //peak occured but did not return to normal 
        //error occurred. TODO: handle
      // }
    }

    if(!digitalRead(A3)){
      dn_int();
      delay(300);
      if(!digitalRead(A3)){
        delay(700);
        while(!digitalRead(A3)){
          dn_int();
          delay(150);
        }
      }
    }

    if(!digitalRead(A4)){
      up_int();
      delay(300);
      if(!digitalRead(A4)){
        delay(700);
        while(!digitalRead(A4)){
          up_int();
          delay(150);
        }
      }
    }
  // } else{
  //   delay(5000);
  // }
}

void setCurrentDigits(){
  String strval = String(int(shot_count));
  if(strval.length() == 1){
    int dig1 = (int)strval[0] - 48;
    lc.setDigit(0,2,dig1,false);
  } else {
    int dig1 = (int)strval[0] - 48;
    int dig2 = (int)strval[1] - 48;
    lc.setDigit(0,2,dig1,false);
    lc.setDigit(0,3,dig2,false);
  }
}

void setSetDigits(){  
  String strval = String(int(shot_set));
  if(strval.length() == 1){
    int dig1 = (int)strval[0] - 48;
    lc.setDigit(0,0,dig1,false);
  } else {
    int dig1 = (int)strval[0] - 48;
    int dig2 = (int)strval[1] - 48;
    lc.setDigit(0,0,dig1,false);
    lc.setDigit(0,1,dig2,false);
  }
}