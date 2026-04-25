#include <Wire.h>
#include <ESP32Servo.h>

Servo baseServo; //18
Servo topServo;  //19

int16_t ax,ay,az;

float pitchOffset=0;
float rollOffset=0;

float smoothPitch=90;
float smoothRoll=90;

void setup(){

Serial.begin(115200);
delay(1500);

Wire.begin(21,22);

Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);

baseServo.attach(18);
topServo.attach(19);

baseServo.write(90);
topServo.write(90);

// calibrate level
for(int i=0;i<300;i++){
 readMPU();

 pitchOffset+=atan2(ax,az)*180/PI;
 rollOffset+=atan2(ay,az)*180/PI;

 delay(5);
}

pitchOffset/=300;
rollOffset/=300;
}

void loop(){

readMPU();

float pitch=
atan2(ax,az)*180/PI-pitchOffset;

float roll=
atan2(ay,az)*180/PI-rollOffset;


// SELF LEVEL (counter movement)
int targetTop=90 - pitch*1.2;
int targetBase=90 - roll*1.5;


targetTop=constrain(targetTop,20,160);
targetBase=constrain(targetBase,20,160);


// strong stabilization
smoothPitch=
smoothPitch*0.85+
targetTop*0.15;

smoothRoll=
smoothRoll*0.75+
targetBase*0.25;


topServo.write(smoothPitch);
baseServo.write(smoothRoll);

delay(2);

}

void readMPU(){

Wire.beginTransmission(0x68);
Wire.write(0x3B);
Wire.endTransmission(false);

Wire.requestFrom(0x68,6,true);

ax=Wire.read()<<8|Wire.read();
ay=Wire.read()<<8|Wire.read();
az=Wire.read()<<8|Wire.read();

}