//#include <RTClib.h>

#include <RTClight.h>
RTC_PCF8523 rtc;
uint8_t timeUnit[3] = {COUNT_DOWN_HOUR, COUNT_DOWN_MINUTE, COUNT_DOWN_SECOND};

#define seconds 2
#define minutes 1
#define hours   0

void setup() {
  Serial.begin(115200);
  rtc.begin();
  RTC_Init();
  setCountdown(timeUnit[seconds], 10);   //initialize countdown interrupt to every 10 seconds
  pinMode(2, INPUT_PULLUP);              //external interrupt pin pullup (INT0)
}

void loop() {

  attachInterrupt (0, alarmMatch, FALLING);
  Serial.println("waiting for interrupt");

  Serial.print(rtc.read(DAYS));
  Serial.print('/');
  Serial.print(rtc.read(MONTHS));
  Serial.print('/');
  Serial.println(rtc.read(YEARS));
  Serial.println(" ");

  Serial.print(rtc.read(HOURS));
  Serial.print(':');
  Serial.print(rtc.read(MINUTES));
  Serial.print(':');
  Serial.println(rtc.read(SECONDS));
  Serial.print("Epoch value :");
  Serial.println(rtc.unixtime());         //number of seconds since 1 Jan 1970

  delay(3000);
}


void RTC_Init(void) {
  if (! rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }
  //  rtc.setTime(12, 39, 45);            //hour, minute, Second (HH:MM:SS)
  //  rtc.setDate(9, Tue, Oct, 20);       //Tuesday 9th/10/2020
  rtc.DateTime(F(__DATE__), F(__TIME__)); //use compile time        
}

void setCountdown(char TimeUnit, uint8_t Time) {
  rtc.countDown_Enable(TimeUnit, Time);
}

void alarmMatch(void) {
  Serial.println("interrupt");  //print every 10 seconds
}
