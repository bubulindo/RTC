/*
* RTC code for Arduino V0.0
* This library has been "tested" in ATmega16 and ATmega168. It should work on ATmega32 
* and ATmega328 as those are pin compatible and have pretty much the same insides. 
* 
* To use this on the ATmega168 and 328, you need to disable the external cristal and 
* use the internal cristal. 
*
* This code is provided as is and hasn't been tested on the chip for more than 6 hours, 
* so it is quite natural that the clock isn't counting the days accurately. That needs 
* further testing.
* 
* TO DO on version 1.0: 
* - Test the entire time counting library. 
*
* - Since the code will run on an 8 bit processor with limited resources, trim down the 
* 	unused variables such as: DST, day of year, etc. Purely because these should be
*	calculated, not set. So no point in following POSIX.
*
* - provide a bare bones implementation that will only count seconds since startup. 
*
* - overload the constructor to allow initialization with a struct tm.
*
* - allow the operator to set/get the time with a struct tm. 
*
* - check that the clock is stable before starting to count. This happens on reset and 
*	startup.
*
* - fix bug with the starting and stopping of the 1Hz wave. 
*/




#include "rtc.h"


//Variables needed... 
const unsigned char days_per_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

//time keeping structure.
struct tm time;


//constructor. 
//set time from scratch. 
InternalRTC::InternalRTC(unsigned char Seconds, unsigned char Minutes, unsigned char Hours,
	unsigned char dayOfMonth, unsigned char Month, unsigned int Year, 
	unsigned char dayOfWeek, unsigned int dayOfYear, unsigned char isDST) {
	
//ATmega16 and 32	
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

//ATmega168, 328, 644... 
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

//FIX ME... verify of the clock is running correctly... 
//and only then initialize the variables. 

	time.tm_sec   = Seconds;
	time.tm_min   = Minutes;
	time.tm_hour  = Hours;
	time.tm_mday  = dayOfMonth;
	time.tm_mon   = Month;
	time.tm_year  = Year;
	time.tm_wday  = dayOfWeek;
	time.tm_yday  = dayOfYear;
	time.tm_isdst = isDST;
}


/*
* Return how many seconds counted so far. 
*/

unsigned char InternalRTC::getSeconds(){
	return time.tm_sec;
	}

/*
* Return how many minutes counted so far. 
*/	
unsigned char InternalRTC::getMinutes(){
	return time.tm_min;
	}
	
/*
* Return how many Hours counted so far. 
*/
unsigned char InternalRTC::getHours(){
	return time.tm_hour;
	}
	
/*
* Return the day of month
*/
unsigned char InternalRTC::getDayOfMonth(){
	return time.tm_mday;
	}
/*
* Return how many months counted so far. 
*/
unsigned char InternalRTC::getMonth(){
	return time.tm_mon;
	}
/*
* Return how many years counted so far. 
*/
unsigned int InternalRTC::getYear(){
	return time.tm_year;
	}
/*
* Return day of week. 
*/
unsigned char InternalRTC::getDayOfWeek(){
	return time.tm_wday;
	}
/*
* Return day of year
*/
unsigned int InternalRTC::getDayOfYear(){
	return time.tm_yday;
	}
/*
* Return Day light saving time.
*/
unsigned char InternalRTC::isDST() {
	return time.tm_isdst;
	}

/*
* Set seconds
*/
void InternalRTC::setSeconds(unsigned char seconds){
	time.tm_sec = seconds;
}
/*
* Set minutes 
*/
void InternalRTC::setMinutes(unsigned char minutes){
	time.tm_min = minutes; 
}
/*
* Set hours 
*/
void InternalRTC::setHours(unsigned char hours){
	time.tm_hour = hours;
}
/*
* Set day of Month 
*/
void InternalRTC::setDayOfMonth(unsigned char day){
	time.tm_mday = day;
}
/*
* Set month 
*/
void InternalRTC::setMonth(unsigned char month){
	time.tm_mon = month; 
}
/*
* Set year 
*/
void InternalRTC::setYear(unsigned int year){
	time.tm_year = year;
}
/*
* Set day of week. 
*/
void InternalRTC::setDayOfWeek(unsigned char day){
	time.tm_wday = day; 
}
/*
* Set day of year 
*/
void InternalRTC::setDayOfYear(unsigned int day){
	time.tm_yday = day; 
}
/*
* Set daylight saving time.  
*/
void InternalRTC::setDST(unsigned char enable){
	time.tm_isdst = enable;
}

/*
* accessory function for calculating leap years and compensating it in the counting
* interrupt.
*/
unsigned char isleap(unsigned int year) {

	if(year%400 ==0 || (year%100 != 0 && year%4 == 0))
		return 1;
	else
		return 0; 
}

/*
* Activate a 1Hz square wave on the OC2 pins of the microcontroller. 

*/
unsigned char InternalRTC::square1HzOutput(unsigned char enable, unsigned char pin) {	
	
#if defined(TCCR2) 	//ATmega16
	if (enable) {
	//	SetB(DDRD, 7); //set OC2 as output
		SetB(TCCR2, COM20);
	} else { 
		ClrB(TCCR2, COM20);
	} 
	return 1; 
	
#elif defined(TCCR2A) && defined (TCCR2B)

	if (enable == 1) {
		//if (pin == 1)
			SetB(TCCR2A, COM2A0);
		//else
			//SetB(TCCR2A, COM2B0);
	} else { 
		//if (pin == 1 )
			ClrB(TCCR2A, COM2A0);
		//else 
			//ClrB(TCCR2A, COM2B0);
	} 
	return 1; 
#endif
}

/*
* time counting interrupt. 
*/

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



