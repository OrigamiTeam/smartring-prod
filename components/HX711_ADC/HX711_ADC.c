//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU	: Arduino Nano
//-------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "esp_system.h"
#include <sys/time.h>
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include <HX711_ADC.h>

uint8_t sckPin;	 //HX711 pd_sck pin
uint8_t doutPin; //HX711 dout pin
uint8_t GAIN;
float calFactor;
long dataSampleSet[DATA_SET + 1]; // data set, make voltile if interrupt is used
long tareOffset;
int readIndex = 0;
long timeStamp;
uint8_t isFirst = 1;
uint8_t tareTimes;
const uint8_t divBit = DIVB;
bool doTare;
bool startStatus;
uint8_t convRslt;
bool tareStatus;

//***********MILLIS***************************************
unsigned long IRAM_ATTR adc_millis()
{
	return (unsigned long)(esp_timer_get_time() / 1000ULL);
}
//********************************************************

/*void adc_setPins(uint8_t dout, uint8_t sck) //constructor
{
	doutPin = dout;
	sckPin = sck;
}*/

void adc_setGain(uint8_t gain) //value should be 32, 64 or 128*
{
	if ((int)gain == NULL)
		gain = 128;

	if (gain < 64)
		GAIN = 2; //32, channel B
	else if (gain < 128)
		GAIN = 3; //64, channel A
	else
		GAIN = 1; //128, channel A
}

void adc_begin(uint8_t dout, uint8_t sck)
{
	doutPin = dout;
	sckPin = sck;

	gpio_pad_select_gpio(sckPin);
	gpio_set_direction(sckPin, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(doutPin);
	gpio_set_direction(doutPin, GPIO_MODE_INPUT);
	adc_setGain(128);
	powerUp();
}

void adc_begin_g(uint8_t dout, uint8_t sck, uint8_t gain)
{
	doutPin = dout;
	sckPin = sck;

	gpio_pad_select_gpio(sckPin);
	gpio_set_direction(sckPin, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(doutPin);
	gpio_set_direction(doutPin, GPIO_MODE_INPUT);
	adc_setGain(gain);
	powerUp();
}

/*  start(t): will do conversions continuously for 't' +400 milliseconds (400ms is min. settling time at 10SPS). 
*   Running this for 1-5s before tare() seems to improve the tare accuracy */
void adc_start(unsigned int t)
{
	t += 400;
	while (adc_millis() < t)
	{
		adc_getData();
	}
	tare();
	tareStatus = 0;
}

int adc_startMultiple(unsigned int t)
{
	if (startStatus == 0)
	{
		if (isFirst)
		{
			t += 400; //min time for HX711 to be stable
			timeStamp = adc_millis();
			isFirst = 0;
		}
		if (adc_millis() < timeStamp + t)
		{
			//adc_update(); //do conversions during stabi time
			adc_update();
			return 0;
		}
		else
		{ //do tare after stabi time
			doTare = 1;
			adc_update();
			if (convRslt == 2)
			{
				doTare = 0;
				convRslt = 0;
				startStatus = 1;
			}
		}
	}
	return startStatus;
}

void tare()
{
	uint8_t rdy = 0;
	doTare = 1;
	tareTimes = 0;
	while (rdy != 2)
	{
		rdy = adc_update();
	}
}

bool getTareStatus()
{
	bool t = tareStatus;
	tareStatus = 0;
	return t;
}

void tareNoDelay()
{
	doTare = 1;
	tareTimes = 0;
}

void adc_setCalFactor(float cal) //raw data is divided by this value to convert to readable data
{
	calFactor = cal;
}

float adc_getCalFactor() //raw data is divided by this value to convert to readable data
{
	return calFactor;
}

//call adc_update() in loop
//if conversion is ready; read out 24 bit data and add to data set, returns 1
//if tare operation is complete, returns 2
//else returns 0
uint8_t adc_update()
{
	//#ifndef USE_PC_INT
	uint8_t dout = gpio_get_level(doutPin); //check if conversion is ready
	if (!dout)
	{
		conversion24bit();
		//if(s) Serial.print(s);
	}
	else
		convRslt = 0;
	return convRslt;
	//#endif
}

float adc_getData() // return fresh data from the moving average data set
{
	//long k = 0;
	long data = 0;
	data = smoothedData() - tareOffset;
	data = (data >> divBit);
	float x = (float)data / calFactor;
	return x;
}

long smoothedData()
{
	long data = 0;
	long L = 0xFFFFFF;
	long H = 0x00;
	for (uint8_t r = 0; r < DATA_SET; r++)
	{
		if (L > dataSampleSet[r])
			L = dataSampleSet[r]; // find lowest value
		if (H < dataSampleSet[r])
			H = dataSampleSet[r]; // find highest value
		data += dataSampleSet[r];
	}
	if (IGN_LOW_SAMPLE)
		data -= L; //remove lowest value
	if (IGN_HIGH_SAMPLE)
		data -= H; //remove highest value
	return data;
}

void conversion24bit() //read 24 bit data and start the next conversion
{
	unsigned long data = 0;
	uint8_t dout;
	convRslt = 0;

	// Begin of critical section.
	// Critical sections are used as a valid protection method
	// against simultaneous access in vanilla FreeRTOS.
	// Disable the scheduler and call portDISABLE_INTERRUPTS. This prevents
	// context switches and servicing of ISRs during a critical section.
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	portENTER_CRITICAL(&mux);

	for (uint8_t i = 0; i < (24 + GAIN); i++)
	{					 //read 24 bit data + set gain and start next conversion
		ets_delay_us(1); // required for faster mcu's?
		gpio_set_level(sckPin, 1);
		ets_delay_us(1);
		if (i < (24))
		{
			dout = gpio_get_level(doutPin);
			data = data << 1;
			if (dout)
			{
				data++;
			}
		}
		gpio_set_level(sckPin, 0);
	}

	// End of critical section.
	portEXIT_CRITICAL(&mux);

	data = data ^ 0x800000; // if out of range (min), change to 0
	if (readIndex == DATA_SET - 1)
	{
		readIndex = 0;
	}
	else
	{
		readIndex++;
	}
	if (data > 0)
	{
		convRslt++;
		dataSampleSet[readIndex] = (long)data;
		if (doTare)
		{
			if (tareTimes < DATA_SET)
			{
				tareTimes++;
			}
			else
			{
				tareOffset = smoothedData();
				tareTimes = 0;
				doTare = 0;
				tareStatus = 1;
				convRslt++;
			}
		}
	}
}

void powerDown()
{
	gpio_set_level(sckPin, 0);
	gpio_set_level(sckPin, 1);
}

void powerUp()
{
	gpio_set_level(sckPin, 0);
}

//get the tare offset (raw data value output without the scale "CalFactor")
long getTareOffset()
{
	return tareOffset;
}

//set new tare offset (raw data value input without the scale "CalFactor")
void setTareOffset(long newoffset)
{
	tareOffset = newoffset;
}

//for testing only:
//if ready: returns 24 bit data and starts the next conversion, else returns -1
float getSingleConversion()
{
	long data = 0;
	uint8_t dout = gpio_get_level(doutPin); //check if conversion is ready
	if (!dout)
	{
		//data = conversion24bit();
		data = data - tareOffset;
		float x = (float)data / calFactor;
		return x;
	}
	else
		return -1;
}
