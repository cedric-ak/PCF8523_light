/*RTC library for PCF8523
 * Multiple interrupt example
 */

#include <RTClight.h>
RTC_PCF8523 rtc;
bool interrupt = false;
void setup() {
  Serial.begin(115200);
  rtc.begin();
  RTC_Init();
  pinMode(2, INPUT_PULLUP);   //INT0 pullup
  rtc.countDown_Enable_TMRA(COUNT_DOWN_MINUTE, 2);
  rtc.countDown_Enable_TMRB(COUNT_DOWN_SECOND, 20);
  rtc.timeAlarm(17,58);  //(hh:mm) in 24h format
//  rtc.dayAlarm(25,17,04);    // day, hr, mm  
      
}

void loop() {
  attachInterrupt (0, alarmMatch, FALLING);
  if (interrupt) {
    if (TIMER_A_INTERRUPT) { //read TMER A interrupt flag
      rtc.CLR_ALL_INTF();    //clear interrupt timer A countdown
      Serial.println("TMRA interrupt");
    }
    if (TIMER_B_INTERRUPT) { //read TMER B interrupt flag
     rtc.CLR_ALL_INTF();     //clear interrupt timer B countdown
      Serial.println("TMRB interrupt");
    }
    if (ALARM_INTERRUPT) {
      Serial.println("Time Alarm interrupt");
      rtc.CLR_ALL_INTF();    //clear alarm interrupt flag
    }
    
//    for (int x = 0;  x < 20; x++) {     //read and print all registers for debug
//      Serial.print(x, HEX);
//      Serial.print(":");
//      Serial.println(rtc.read(x), BIN);
//    }
    interrupt = false;
  }

}

void RTC_Init(void) {
  if (! rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }
//    rtc.setTime(16, 48, 50);            //hour, minute, Second (HH:MM:SS)
//    rtc.setDate(9, Tue, Oct, 20);       //Tuesday 9th/10/2020
  rtc.DateTime(F(__DATE__), F(__TIME__)); //use compile time
}

void alarmMatch() {
  interrupt = true;
}
