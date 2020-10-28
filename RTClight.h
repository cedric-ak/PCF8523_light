/*
  LED.h - Library for using the PCF8523 Real time clock.
  October 13, 2020.
  v1.0
*/

#ifndef RTCLIGHT_H
#define	RTCLIGHT_H


#define SLAVE_ADDRESS   0x68   //device address

    /*Control register*/
#define CONTROL_1       0X00
#define CONTROL_2       0x01
#define CONTROL_3       0x02
    /*Time and date Registers*/
#define SECONDS         0x03
#define MINUTES         0x04
#define HOURS           0x05
#define DAYS            0x06
#define WEEKDAYS        0x07
#define MONTHS          0x08
#define YEARS           0x09
    /*Alarm Registers*/
#define MINUTE_ALARM    0x0A
#define HOUR_ALARM      0x0B
#define DAY_ALARM       0x0C
#define WEEKDAY_ALARM   0x0D
#define COUNT_DOWN_HOUR       0x06
#define COUNT_DOWN_MINUTE     0x03
#define COUNT_DOWN_SECOND     0x02
    /*Offset Register*/
#define OFFSET          0x0E
    /*CLOCKOUT & Timer Register*/
#define TMR_CLKOUT_CTRL 0x0F
#define TMR_A_FREQ_CTRL 0x10
#define TMR_A_REG       0x11
#define TMR_B_FREQ_CTRL 0x12
#define TMR_B_REG       0x13

/** Constants */
#define SECONDS_PER_DAY       86400L  ///< 60 * 60 * 24
#define SECONDS_FROM_1970_TO_2000 946684800  ///< Unixtime for 2000-01-01 00:00:00, useful for initialization
#define SECONDS_FROM_1970_TO_2020 1577836800 ///< Unixtime for 2020-01-01 00:00:00, useful for initialization

 /*PCF8523 interrupt flags */
/* #define WTAF				  0x04  
#define CTAF				  0x02
#define CTBF			      0x00 */

enum days { Sun = 0, Mon = 1, Tue = 2, Wed = 3, Thu = 4, Fri = 5, Sat = 6};
enum months {Jan =1, Feb =2, Mar =3, Apr =4, May =5, Jun =6, Jul =7, Aug =8, Sep =9, Oct =10, Nov =11, Dec =12};
enum TMR_SCLK_FREQ {hours = 7, minutes = 3, seconds = 2}; //count down time unit source clock frequency
enum interrupt_Flag {WTAF = 4, CTAF = 2, CTBF = 0};       //watchdog timer, count down timer A, count down timer B

class RTC_PCF8523
{
public:
	bool begin(void);
	void setTime(uint8_t hour, uint8_t minute, uint8_t second);
	void setDate(uint8_t day, uint8_t weekday, uint8_t month, uint8_t year);
	void countDown_Enable(uint8_t timeUnit, uint8_t time);
	void setAlarm(uint8_t alarmReg, uint8_t minute, uint8_t hour, uint8_t day, uint8_t weekDay);
	void PCF8523_write(uint8_t regAdd, uint8_t data);
	int PCF8523_rtc_INTF_CLR(int interruptFlag);
	uint8_t decimalToBCD(int DecValue);
	uint8_t BCDtoDecimal(int BCDvalue);
	uint8_t read(uint8_t address);
	uint32_t unixtime(void) const;
	void DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time);
	uint32_t unixtimeCalc(uint8_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);
	
/* private:
	enum days { Sun = 0, Mon = 1, Tue = 2, Wed = 3, Thu = 4, Fri = 5, Sat = 6};
	enum months {Jan =1, Feb =2, Mar =3, Apr =4, May =5, Jun =6, Jul =7, Aug =8, Sep =9, Oct =10, Nov =11, Dec =12};
	enum TMR_SCLK_FREQ {hours = 7, minutes = 3, seconds = 2}; //count down time unit source clock frequency
	enum interrupt_Flag {WTAF = 4, CTAF = 2, CTBF = 0};       //watchdog timer, count down timer A, count down timer B */
};


#endif	/* PCF8523_H */