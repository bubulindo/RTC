#include "rtc.h"

const unsigned char days_per_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

struct tm time; //time keeping struct
 /*{
	unsigned char    tm_sec;   //seconds [0,61]
	unsigned char    tm_min;   //minutes [0,59]
	unsigned char    tm_hour;  //hour [0,23]
	unsigned char    tm_mday;  //day of month [1,31]	
	unsigned char    tm_mon;   //month of year [0,11]
	unsigned int     tm_year;  //years since 1900
	unsigned char    tm_wday;  //day of week [0,6] (Sunday = 0)
	unsigned int     tm_yday;  //day of year [0,365]
	unsigned char    tm_isdst; //daylight savings flag
} time;*/

unsigned char getSeconds(){
	return time.tm_sec;
	}
unsigned char getMinutes(){
	return time.tm_min;
	}
unsigned char getHours(){
	return time.tm_hour;
	}
unsigned char getDayOfMonth(){
	return time.tm_mday;
	}
unsigned char getMonth(){
	return time.tm_mon;
	}
unsigned int getYear(){
	return time.tm_year;
	}
unsigned char getDayOfWeek(){
	return time.tm_wday;
	}
unsigned int getDAyOfYear(){
	return time.tm_yday;
	}
unsigned char isDST() {
	return time.tm_isdst;
	}

void setSeconds(unsigned char seconds){
	time.tm_sec = seconds;
}
void setMinutes(unsigned char minutes){
	time.tm_min = minutes; 
}
void setHours(unsigned char hours){
	time.tm_hour = hours;
}
void setDayOfMonth(unsigned char day){
	time.tm_mday = day;
}
void setMonth(unsigned char month){
	time.tm_mon = month; 
}
void setYear(unsigned int year){
	time.tm_year = year;
}
void setDayOfWeek(unsigned char day){
	time.tm_wday = day; 
}
void setDayOfYear(unsigned int day){
	time.tm_yday = day; 
}
void setDST(unsigned char enable){
	time.tm_isdst = enable;
}


unsigned char isleap(unsigned int year) {

	if(year%400 ==0 || (year%100 != 0 && year%4 == 0))
		return 1;
	else
		return 0; 

}


unsigned char square1HzOutput(unsigned char enable, unsigned char pin) {	
	
#if defined(TCCR2) 	//ATmega16
	
	
if (enable) {
//	SetB(DDRD, 7); //set OC2 as output
	SetB(TCCR2, COM20);
} else { 
	ClrB(TCCR2, COM20);
	
#elif defined(TCCR2A) && defined (TCCR2B)

if (enable) {
//	SetB(DDRD, 7); //set OC2 as output
	
	if (pin == 1)
		SetB(TCCR2A, COM2A0);
	else
		SetB(TCCR2A, COM2B0);
		
} else { 
	if (pin ==1 )
		ClrB(TCCR2A, COM2A0);
	else 
		ClrB(TCCR2A, COM2B0);
#endif

} 
	return 1; 
  
}


//void increase_time() {

ISR(TIMER2_OVF_vect){//overflow.

unsigned char leap;


if (time.tm_mon == 1 && isleap(time.tm_year)){
	leap = 1;
} else {
	leap = 0;
}
	
if(++time.tm_sec >= 60) {//minute overflow
	time.tm_sec = 0;
    if (++time.tm_min >= 60) {//hour overflow
		time.tm_min = 0; 
		if(++time.tm_hour >= 24) {//day overflow
			time.tm_hour = 0; 
			time.tm_yday++; //day of the year
			if(++time.tm_mday > days_per_month[time.tm_mon] + leap) { //month overflow  ///CHECK FOR FEBRUARY!!!!!!
				time.tm_mday = 1;
				if (++time.tm_mon >11) { //year overflow.
					time.tm_year++;
					time.tm_yday = 0;
					time.tm_mon = 0;
				}
			}
		}
    }
}
}// end ISR()



void rtc_setup(void){
  time.tm_sec   = 10;
  time.tm_min   = 10;
  time.tm_hour  = 13;
  time.tm_mday  = 28;
  time.tm_mon   = 1;
  time.tm_year  = 2005;
  time.tm_wday  = 0;
  time.tm_yday  = 90;
  time.tm_isdst = 1;

	
	
#if defined(TIMSK) && defined (TCCR2) && defined (TCCR2UB) && defined (OCR2) && defined (OCR2UB) && defined (TIFR) //ATmega16
	TIMSK = 0; //setting up...
	SetB(ASSR, AS2); //choose external crystal. This corrupts contents on the rest of the registers.
  
  //set up timer 2
	TCCR2 = 0b00001101; // CTC With toggle activated   TCCR2A = 0b00000010 CTC with no toggle activated
	while (CHECK_BIT(ASSR, TCR2UB) == 0); //check that the write is done correctly. 
	
	OCR2 = 255; //same as normal overflow. Overflow would happen every second.
	while (CHECK_BIT(ASSR, OCR2UB) == 0); //check that the write is done correctly. 
	
	TCNT2 = 0; 
	while (CHECK_BIT(ASSR, TCN2UB) == 0); //check that the write is done correctly. 
	
	TIFR = 0; //clear interrupt flags. 
	SetB(TIMSK, TOIE2); //won't be like this. interrupt must be activated. 

#elif defined(TIMSK2) && defined (TCCR2B) && defined (TCR2AUB) && defined (OCR2A) && defined (OCR2AUB) && defined (TIFR2)  //ATmega168 && ATmega328 $$ ???	

	TIMSK2 = 0;
	SetB(ASSR, AS2); //choose external crystal. This corrupts contents on the rest of the registers.

  //set up timer 2
  	TCCR2A = 0b00000010; //CTC mode to create an output if needed. :) 
  	while (CHECK_BIT(ASSR, TCR2AUB) == 0); //check that the write is done correctly. 
  	
	TCCR2B = 0b00000101; // CTC With toggle activated   TCCR2A = 0b00000010 CTC with no toggle activated
	while (CHECK_BIT(ASSR, TCR2BUB) == 0); //check that the write is done correctly. 
	
    //not needed here... 
	OCR2A = 255; //same as normal overflow. Overflow would happen every second. 
	while (CHECK_BIT(ASSR, OCR2AUB) == 0); //check that the write is done correctly. 
	
	TCNT2 = 0; 
	while (CHECK_BIT(ASSR, TCN2UB) == 0); //check that the write is done correctly. 

	TIFR2 = 0; //clear interrupt flags. 
	
	SetB(TIMSK2, TOIE2); //won't be like this. interrupt must be activated.   

#else
	#error "Chip not supported by library."
#endif 

  /*--------------PORTS-------------------------*/
  
  
   
  //setup port C as outputs (hours) 24 = 5 bits ////    12 hours = 4 bits
  //DDRC = 0x3F; //not touching the 32kHz input.
}



