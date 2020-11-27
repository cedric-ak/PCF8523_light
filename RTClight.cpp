/**************************************************************************
  LED.h - Library for using the PCF8523 Real time clock.
  October 13, 2020.
  v1.2
  Company: Techsitter
  Written by : CEDRIC
**************************************************************************/

#ifdef __AVR_ATtiny85__
 #include <TinyWireM.h>
 #define Wire TinyWireM
#else
#include <Wire.h>
#endif

#include "RTClight.h"
#ifdef __AVR__
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#elif defined(ARDUINO_ARCH_SAMD)
// nothing special needed
#elif defined(ARDUINO_SAM_DUE)
 #define PROGMEM
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
 #define Wire Wire1
#endif

 #if (ARDUINO >= 100)
 #include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
 // Macro to deal with the difference in I2C write functions from old and new Arduino versions.
 #define _I2C_WRITE write   ///< Modern I2C write
 #define _I2C_READ  read    ///< Modern I2C read
#else
 #include <WProgram.h>
 #define _I2C_WRITE send    ///< Legacy I2C write
 #define _I2C_READ  receive ///< legacy I2C read
#endif 
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
/**********************************************************
**Name:     PCF8523_RTC_Init
**Function: Initializes RTC chip
**Input:    none
**Output:   none
**note:     This function is to be called once prior running RTC
**********************************************************/
bool RTC_PCF8523::begin(void) {
    /*
     * 12.5pf
     * RTC time circuits running
     * 24 hour mode is selected
     * second and Alarm interrupt disabled
     * no correction interrupt generated
     */
	softReset();    //reset device before config

 	uint8_t ack;
	Wire.begin();
    PCF8523_write(CONTROL_1, 0x80);

    /*power management register
     * battery switch-over function is enabled in standard mode
     * battery low detection function is enabled
     * clear all power management interrupt flags
     */

    PCF8523_write(CONTROL_2, 0x00);  //clear all interrupts flag 

    PCF8523_write(CONTROL_3, 0x80);

    /*Clockout register
     * clockout disabled
     * timer A and B disabled
     */
    PCF8523_write(TMR_CLKOUT_CTRL, 0xF8);
	
	Wire.beginTransmission(SLAVE_ADDRESS); //if rtc present return true if not return false         
	ack = Wire.endTransmission();
	if(!ack)
		return true;
	else
		return false;
}

/**********************************************************
**Name:     setTime
**Function: Set time of the day 
**Input:    hour/minute and seconds in decimal
**Output:   none
**note:     This function is to be called once prior running RTC
**********************************************************/
void RTC_PCF8523::setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    int Register[4] = {SECONDS, MINUTES, HOURS};
    for (int timeReg = 0; timeReg < 3; timeReg++) {
        if (Register[timeReg] == SECONDS)
            PCF8523_write(Register[timeReg], decimalToBCD(second) & 0x7F);
        else if (Register[timeReg] == MINUTES)
            PCF8523_write(Register[timeReg], decimalToBCD(minute) & 0x7F);
        else if (Register[timeReg] == HOURS)
            PCF8523_write(Register[timeReg], decimalToBCD(hour) & 0x7F);
    }
}

/**********************************************************
**Name:     setDate
**Function: Set date of the year 
**Input:    day, week day, month, year in decimal
**Output:   none
**note:     week day and months are predefined in header file
 *          e.g: setDate(09, Tue, Mar, 21); Tuesday 09/03/2021 
**********************************************************/
void RTC_PCF8523::setDate(uint8_t day, uint8_t weekday, uint8_t month, uint8_t year) {
    uint8_t Register[5] = {DAYS, WEEKDAYS, MONTHS, YEARS};
    for (int timeReg = 0; timeReg < 4; timeReg++) {
        switch (Register[timeReg]) {
            case DAYS:PCF8523_write(Register[timeReg], decimalToBCD(day) & 0x7F);
                break;
            case WEEKDAYS:PCF8523_write(Register[timeReg], decimalToBCD(weekday) & 0x7F);
                break;
            case MONTHS:PCF8523_write(Register[timeReg], decimalToBCD(month) & 0x7F);
                break;
            case YEARS:PCF8523_write(Register[timeReg], decimalToBCD(year) & 0x7F);
                break;
        }     
    }
}

/**********************************************************
**Name:     timeAlarm
**Function: RTC Initialize alarm in hour/minutes or seconds
**Input:    hour and minute
**Output:   none
**note:    
**********************************************************/
void RTC_PCF8523::timeAlarm(uint8_t hour, uint8_t minute) {
    PCF8523_write(CONTROL_1, read(CONTROL_1)| 0x02);                     // Alarm interrupt enabled
    PCF8523_write(HOUR_ALARM, 0x00);
    PCF8523_write(MINUTE_ALARM, 0x00);              //enable minute alarm
    PCF8523_write(HOUR_ALARM, decimalToBCD(hour));
    PCF8523_write(MINUTE_ALARM, decimalToBCD(minute));                                    
}

/**********************************************************
**Name:     dayAlarm
**Function: RTC Initialize alarm in hour/minutes or seconds
**Input:    day of month, hour and min
**Output:   none
**********************************************************/
void RTC_PCF8523::dayAlarm(uint8_t day, uint8_t hour, uint8_t minute) {
    PCF8523_write(CONTROL_1, read(CONTROL_1)| 0x02);     // Alarm interrupt enabled
    PCF8523_write(DAY_ALARM, 0x00);                      //enable day alarm
    PCF8523_write(DAY_ALARM, decimalToBCD(day));  
    timeAlarm(hour, minute);                             //enable time alarm                                                
}

/**********************************************************
**Name:     setAlarm
**Function: RTC Initialize alarm in hour/minutes or seconds
**Input:    Alarm register (defined in Header file), time variable
**Output:   none
**note:     One alarm can be used at the time, not used alarm set value to 0 in software, eg: setAlarm(MINUTE_ALARM, 15, 0 ,0 ,0);
**********************************************************/
void RTC_PCF8523::wkDayAlarm(uint8_t weekDay) {
    PCF8523_write(CONTROL_1, read(CONTROL_1)| 0x02);                     // Alarm interrupt enabled
    PCF8523_write(WEEKDAY_ALARM, 0x00);                    //enable week day alarm
    PCF8523_write(WEEKDAY_ALARM, decimalToBCD(weekDay));                                                    
}

/**********************************************************
**Name:     countDown_Enable
**Function: RTC Initialize count down of timer A
**Input:    time unit(hours, minutes, seconds), time in decimal format
**Output:   none
**note:     time variable maximum 255, e.g:PCF8523_countDown_Enable(hours, 2); 
            interrupt generated every 2 hours. interrupt flag must be cleared in software 
**********************************************************/
void RTC_PCF8523::countDown_Enable_TMRA(uint8_t timeUnit, uint8_t time) {
    PCF8523_write(TMR_CLKOUT_CTRL,read(TMR_CLKOUT_CTRL)|0x02); //enable timer A pulse interrupt
    PCF8523_write(CONTROL_2, read(CONTROL_2) |0x02);       //countdown timer A interrupt is enabled
    PCF8523_write(TMR_A_FREQ_CTRL, timeUnit);
    PCF8523_write(TMR_A_REG, time);       //max 255 in decimal
}

/**********************************************************
**Name:     countDown_Enable
**Function: RTC Initialize count down of timer B
**Input:    time unit(hours, minutes, seconds), time in decimal format
**Output:   none
**note:     time variable maximum 255, e.g:PCF8523_countDown_Enable(hours, 2); 
            interrupt generated every 2 hours. interrupt flag must be cleared in software 
**********************************************************/
void RTC_PCF8523::countDown_Enable_TMRB(uint8_t timeUnit, uint8_t time) {
    PCF8523_write(TMR_CLKOUT_CTRL, read(TMR_CLKOUT_CTRL)|0x01); //enable timer B pulse interrupt
    PCF8523_write(CONTROL_2,  read(CONTROL_2) |0x01);       //countdown timer B interrupt is enabled
    PCF8523_write(TMR_B_FREQ_CTRL, timeUnit);
    PCF8523_write(TMR_B_REG, time);       //max 255 in decimal
}


/**********************************************************
**Name:     PCF8523_read
**Function: Read RTC registers, can be used to read and return time
**Input:    Desired register address 
**Output:   Returns 1 byte unsigned value from register 
**note:     this function reads time/date (pass the address of desired variable to read) 
            e.g: PCF8523_read(HOURS);  returns current hour in decimal
**********************************************************/
uint8_t RTC_PCF8523::read(uint8_t address) {    
  uint8_t dataRead ;  
	Wire.beginTransmission(SLAVE_ADDRESS);
	Wire.write(address);
	Wire.endTransmission();      // stop transmitting
	Wire.requestFrom(SLAVE_ADDRESS,1);
	while (Wire.available()) {  // slave may send less than requested
    dataRead = Wire.read();   // receive a byte as character
   }
   if(address > 0x02 && address < 0x0A)   //if we reading time or date convert from BCD to Dec, if not return register value as is.
      return BCDtoDecimal(dataRead);
 else 
  return dataRead;
}

/**********************************************************
**Name:     INTF_CLR
**Function: RTC interrupt flags clear
**Input:    watchdog timer INT flag (WTAF), count down timer A INT flag (CTAF), count down timer B INT flag(CTBF) 
**Output:   
**note:     
**********************************************************/
int RTC_PCF8523::INTF_CLR(int interruptFlag) {    
    PCF8523_write(CONTROL_2, (read(CONTROL_2)) & interruptFlag); 
    delay(5);
    //return;
}

/**********************************************************
**Name:     CLR_ALL_INTF
**Function: RTC interrupt flags clear
**Input:    watchdog timer INT flag (WTAF), count down timer A INT flag (CTAF), count down timer B INT flag(CTBF) 
**Output:   
**note:    
**********************************************************/
void RTC_PCF8523::CLR_ALL_INTF(void) {    
    INTF_CLR(CTAF);   //clear interrupt timer A countdown
    INTF_CLR(CTBF);   //clear interrupt timer B countdown
    INTF_CLR(AF);     //clear alarm interrupt flag
}

/**********************************************************
      
**********************************************************/
void RTC_PCF8523::PCF8523_write(uint8_t regAdd, uint8_t data) {
	
	Wire.beginTransmission(SLAVE_ADDRESS);
	Wire.write(regAdd);
	Wire.write(data);
	Wire.endTransmission();     // stop transmitting
}

uint8_t RTC_PCF8523::decimalToBCD(int DecValue) {
    return (((DecValue / 10) << 4) | (DecValue % 10));
}

uint8_t RTC_PCF8523::BCDtoDecimal(int BCDvalue) {
    return (((BCDvalue >> 4)*10) + (BCDvalue & 0xF));
}

/**************************************************************************/
/*!
    @brief  Convert a string containing two digits to uint8_t, e.g. "09" returns 9
    @param p Pointer to a string containing two digits
*/
/**************************************************************************/
static uint8_t conv2d(const char* p) {
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

/**************************************************************************/
/*!
    @brief  A convenient constructor for using "the compiler's time":
            This version will save RAM by using PROGMEM to store it by using the F macro.
            DateTime now (F(__DATE__), F(__TIME__));
    @param date Date string, e.g. "Dec 26 2009"
    @param time Time string, e.g. "12:34:56"
*/
/**************************************************************************/
void RTC_PCF8523::DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    char buff[11];
    memcpy_P(buff, date, 11);
     uint8_t y = conv2d(buff + 9); 
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
uint8_t m;
    switch (buff[0]) {
        case 'J': m = (buff[1] == 'a') ? 1 : ((buff[2] == 'n') ? 6 : 7); break;
        case 'F': m = 2; break;
        case 'A': m = buff[2] == 'r' ? 4 : 8; break;
        case 'M': m = buff[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    uint8_t d = conv2d(buff + 4);
    memcpy_P(buff, time, 8);
    uint8_t hh = conv2d(buff);
    uint8_t mm = conv2d(buff + 3);
    uint8_t ss = conv2d(buff + 6);
	setTime(hh, mm, ss);
	setDate(d,0,m,y);
	
}

/** Number of days in each month */
const uint8_t daysInMonth []= { 31,28,31,30,31,30,31,31,30,31,30,31 };

/**************************************************************************/
/*!
    @brief  Given a date, return number of days since 2000/01/01, valid for 2001..2099
    @param y Year
    @param m Month
    @param d Day
    @return Number of days since 1/1/2000
*/
/***************************************************************************/

static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)    //if leap year +1day
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;      //Number of days since 1/1/2000
}

/**************************************************************************/
/*!
    @brief  Given a number of days, hours, minutes, and seconds, return the total seconds
    @param days Days
    @param h Hours
    @param m Minutes
    @param s Seconds
    @return Number of seconds total
*/
/**************************************************************************/
static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

/**************************************************************************/
/*!
    @brief  Return unix time, seconds since Jan 1, 1970.
    @return Number of seconds since Jan 1, 1970
*/
/**************************************************************************/
uint32_t RTC_PCF8523::unixtime(void) const {
  uint32_t epoch;
  uint16_t days = date2days(read(YEARS), read(MONTHS), read(DAYS));
  epoch = time2long(days, read(HOURS), read(MINUTES), read(SECONDS)); 
  epoch += SECONDS_FROM_1970_TO_2000;  //  + seconds from 1970 to 2000

  return epoch;
}

/**************************************************************************/

/*!@brief  Given a number of year, month, day,, hour, minute and second, return the total seconds from Jan 1 2020
   @param y year
    @param m Month
    @param d Day
    @param hh Hours
    @param mm Minutes
    @param ss Seconds
    @brief  Return unix time, seconds since Jan 1, 2020.
    @return Number of seconds since Jan 1, 2020
*/
/**************************************************************************/

uint32_t RTC_PCF8523::unixtimeCalc(uint8_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss){
  uint32_t epoch, epochMin;
  uint16_t days = date2days(y, m, d);
  epoch = time2long(days,hh,mm,ss); 
  epoch += SECONDS_FROM_1970_TO_2000;  // + seconds from 1970 to 2000     
  return epochMin = (epoch - SECONDS_FROM_1970_TO_2020);  
}

void RTC_PCF8523::softReset(void){
  PCF8523_write(CONTROL_1, 0x58); 
  delay(5);
}