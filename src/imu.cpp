#include <imu.h>

#define MOTION_DETECTION_THRESH 100
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
    calib_mpu();
    //TODO: remove led pin
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
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

bool check_motion(){
    Vector rawGyro = mpu.readRawGyro();
    if(((rawGyro.XAxis - ogx) > MOTION_DETECTION_THRESH) || ((rawGyro.YAxis - ogy) > MOTION_DETECTION_THRESH) || ((rawGyro.ZAxis - ogz) > MOTION_DETECTION_THRESH)){  
        delay(5);
        rawGyro = mpu.readRawGyro();
        if(((rawGyro.XAxis - ogx) > MOTION_DETECTION_THRESH) || ((rawGyro.YAxis - ogy) > MOTION_DETECTION_THRESH) || ((rawGyro.ZAxis - ogz) > MOTION_DETECTION_THRESH)){
            digitalWrite(13, HIGH);
            return true;
        }
    } else{
        digitalWrite(13, LOW);
        return false;
    }
}