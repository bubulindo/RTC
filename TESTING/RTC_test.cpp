#include "RTC.h"
extern "C" {
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
}


//sadly... this does not work unless used on a big chip
#if ! (defined( __AVR_ATmega1280__) || defined(__AVR_ATmega2560__))
#error "Does not work unless used on Arduino Mega boards... it doesn't work there either"
#endif

RTC::RTC(){

RTC(1970,1,1,0,0,0,THURSDAY);//Epoch

}

RTC::RTC(unsigned int year, unsigned char month, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second, unsigned char day_of_week){
//1.	Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2. 
//2.	Select clock source by setting AS2 as appropriate. 
//3.	Write new values to TCNT2, OCR2x, and TCCR2x. 
//4.	To switch to asynchronous operation: Wait for TCN2UB, OCR2xUB, and TCR2xUB. 
//5.	Clear the Timer/Counter2 Interrupt Flags.
//6.	Enable interrupts, if needed.

cli();

//set AS2 bit to choose async operation
ASSR = 0;
SetB(ASSR, AS2);
SetB(ASSR, EXCLK);
//ASSR |= (1<<AS2) || (1<<EXCLK);//async op

TCNT2 = 0; 
while (TestB(ASSR, TCN2UB)); //while update is busy

TCCR2A = 0; //no waveform generation 
while (TestB(ASSR, TCR2AUB)); //while update is busy

TCCR2B = 3; // no force compare, 128 prescaler for an interrupt on each second with 32768 Hz clock
while (TestB(ASSR, TCR2BUB)); //while update is busy

//enable interrupt
SetB (TIMSK2, TOIE2); //overflow interrupt enable
	
// enable global interrupts
sei();//could be already done somewhere else. 

clock.tm_sec = second;
clock.tm_min = minute; 
clock.tm_hour = hour; 
clock.tm_mday = day;
clock.tm_mon = month; 
clock.tm_year = year; 
clock.tm_wday = day_of_week; 
clock.tm_yday = getDayOfYear();//it has been initialized already
clock.tm_isdst = 0;

}

RTC::~RTC(){
//stop timer
TCCR2A = 0; 
TCCR2B = 0; 
TIMSK2 = 0; 


}

//retrieve time
tm RTC::getClock(){

return clock; 	

}



//overload set time
void RTC::setTime( unsigned char hour, unsigned char minute, unsigned char second){
	clock.tm_hour = hour;
	clock.tm_min = minute;
	clock.tm_sec = second;
}

void RTC::setDate(unsigned char day, unsigned char month, unsigned int year, unsigned char day_of_week){
	clock.tm_mday = day;
	clock.tm_mon = month;
	clock.tm_year = year; 
	clock.tm_wday = day_of_week;
	clock.tm_yday = getDayOfYear(day, month, year);
}


/*
//return time in a specific format
char * RTC::printTime(char timeSeparator, char dateSeparator){
	char string[];//DD.MM.YY hh.mm.ss[/0] 
	
	//call both C functions and then concatenate. :) 
}

char * RTC::printDate(char separator){
//DD.MM.YY[/0]
}

char * RTC::printHours(char separator){
//hh.mm.ss[/0]
}
//return true if leapYear
*/

unsigned char RTC::isLeapYear(){

	if( clock.tm_year%400 == 0 || ( clock.tm_year % 100 != 0 && clock.tm_year % 4 == 0))
		return 1;
	else
		return 0; 
}

unsigned char RTC::isLeapYear(unsigned int year){

	if( year%400 == 0 || ( year % 100 != 0 && year % 4 == 0))
		return 1;
	else
		return 0; 
}

unsigned int RTC::getDayOfYear(unsigned char day, unsigned char month, unsigned int year)
{	
	int number=day; // The day is added on the spot.
		
	if (month > 2 && isLeapYear(year))//past february
	{
		number++;
	}
	
	while( month !=0)
	{
		number += Months[month-2];//then add all the days of the previous months. Correct for array index.
		
		month--;
	}
	
	return number;
}

unsigned int RTC::getDayOfYear()
{
return clock.tm_yday;

}




ISR(TIMER2_OVF_vect){
	unsigned char temp = 0; //possibility to use register keyword???

	//a minute has passed??	
	clock.tm_sec++;
	if (clock.tm_sec == 60) //another minute? 
	{
		clock.tm_sec = 0; 
		clock.tm_min++;
		//an hour has passed?		
		if(clock.tm_min == 60)
		{
			clock.tm_min = 0;
			clock.tm_hour++;
			//a day has passed? 
			if (clock.tm_hour == 24)
			{
				clock.tm_hour = 0; 
				//if it's February and it is a leap year				
				if ((clock.tm_mon == 2)/* && (isLeapYear(clock.tm_year))*/) temp = 1;//leap year
				clock.tm_mday++; 
				clock.tm_wday++; 				
				//if it is a new week				
				if (clock.tm_wday == DAYSPERWEEK) clock.tm_wday = 0; //new week as started
				
				//if it is a new month
				if (clock.tm_mday > Months[clock.tm_mon-1] + temp)
				{
					clock.tm_mday = 1;//new month
					//if it is a new year					
					clock.tm_mon++;
					if (clock.tm_mon == 13)
					{
						clock.tm_year++;
						clock.tm_yday=0;
						clock.tm_mon = 1;
					};
				};
			};
		};
	};
}


	