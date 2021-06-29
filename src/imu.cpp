#include <imu.h>

MPU6050 mpu;
float ogx, ogy, ogz;

void calib_mpu();

bool mpu_init(){
    while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G)){
        Serial.println ( "Cannot find MPU6050 - check connection!" ) ;
        delay(500);         //TODO: handle mpu init failure (nothing to do just led or something)
    }
    //Serial.println("MPU6050 Found!");

    mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
    mpu.setIntFreeFallEnabled(false);  
    mpu.setIntZeroMotionEnabled(false);
    mpu.setIntMotionEnabled(false);
    mpu.setDHPFMode(MPU6050_DHPF_5HZ);
    // Set the motion detection limits to 2mg (divide the value by 2)
    // and the minimum duration is 5ms
    mpu.setMotionDetectionThreshold(2) ;
    mpu.setMotionDetectionDuration(2) ;
    calib_mpu();
    //TODO: remove led pin
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    
    while(1){
        Vector rawGyro = mpu.readRawGyro();
        // If motion is detected - light the LED on pin 13
        // if (act.isInactivity)
        // {
        //     digitalWrite(13, HIGH);
        // } else
        // {
        //     digitalWrite(13, LOW);
        // }
        Serial.print(rawGyro.XAxis - ogx);
        Serial.print(",");
        Serial.print(rawGyro.YAxis - ogy);
        Serial.print(",");
        Serial.println(rawGyro.ZAxis - ogz);
        if(((rawGyro.XAxis - ogx) > 50) || ((rawGyro.YAxis - ogy) > 50) || ((rawGyro.ZAxis - ogz) > 50)){  
            digitalWrite(13, HIGH);
        } else{
            digitalWrite(13, LOW);
        }
        delay(20);
    }
}

void calib_mpu(){
    int last_ogx, last_ogy, last_ogz;
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            Vector rawGyro = mpu.readRawGyro();
            ogx += rawGyro.XAxis;
            ogy += rawGyro.YAxis;
            ogz += rawGyro.ZAxis;
        }
        if(i > 0){
            ogx = ogx/11.0;
            ogy = ogy/11.0;
            ogz = ogz/11.0;
        } else{
            ogx = ogx/10.0;
            ogy = ogy/10.0;
            ogz = ogz/10.0;
        }
        if(i > 0){
            if((abs(ogx - last_ogx) > 10) || (abs(ogy - last_ogy) > 10) || (abs(ogz - last_ogz) > 10)){  
                //TODO:handle calib failure condition
            }
        }
        last_ogx = ogx;
        last_ogy = ogy;
        last_ogz = ogz;
        delay(50);
    }
}
