#include <rtc.h>

#define TRUE   1
#define FALSE  0

unsigned long before = 0;

InternalRTC RTC(0, 20, 15, 27, 11, 2012, 2, 300, 0);



void setup(){
  Serial.begin (19200);
  pinMode(11,OUTPUT);
  
}

void loop() {

  if (millis()-before > 1000) {
    before = millis();
  
    Serial.print("time is:");
    Serial.print(RTC.getHours());  
    Serial.print(":");
    Serial.print(RTC.getMinutes());
    Serial.print(":");
    Serial.println(RTC.getSeconds());
  }
  
  if (Serial.available() > 0) {
    char temp = Serial.read();
    
    if (temp == 'n') {
       RTC.square1HzOutput(TRUE, 1);
       Serial.println("on");
    }
    if (temp == 'f') {
       RTC.square1HzOutput(FALSE, 1);  
       Serial.println("off");
    }
  }

}


