#include <avr/interrupt.h>


#define SetB(PORT, BIT)	((PORT) |= 1<< (BIT))
#define ClrB(PORT, BIT)	((PORT) &= 1<< (BIT))
#define TogB(PORT, BIT)	((PORT) ^= 1<< (BIT))
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))




struct tm {
	unsigned char    tm_sec;   //seconds [0,61]
	unsigned char    tm_min;   //minutes [0,59]
	unsigned char    tm_hour;  //hour [0,23]
	unsigned char    tm_mday;  //day of month [1,31]	
	unsigned char    tm_mon;   //month of year [0,11]
	unsigned int     tm_year;  //years since 1900
	unsigned char    tm_wday;  //day of week [0,6] (Sunday = 0)
	unsigned int     tm_yday;  //day of year [0,365]
	unsigned char    tm_isdst; //daylight savings flag
};

unsigned char isleap(unsigned int year);
unsigned char square1HzOutput(unsigned char enable, unsigned char pin);
void rtc_setup(void);

unsigned char getSeconds();
unsigned char getMinutes();
unsigned char getHours();
unsigned char getDayOfMonth();
unsigned char getMonth();
unsigned int getYear();
unsigned char getDayOfWeek();
unsigned int getDayOfYear();
unsigned char isDST();

void setSeconds(unsigned char seconds);
void setMinutes(unsigned char minutes);
void setHours(unsigned char hours);
void setDayOfMonth(unsigned char day);
void setMonth(unsigned char month);
void setYear(unsigned int year);
void setDayOfWeek(unsigned char day);
void setDayOfYear(unsigned int day);
void setDST(unsigned char enable);