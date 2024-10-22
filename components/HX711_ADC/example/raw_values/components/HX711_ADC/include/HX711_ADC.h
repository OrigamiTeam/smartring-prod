//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU	: Arduino Nano
//-------------------------------------------------------------------------------------

#ifndef __HX711_ADC_H__
#define __HX711_ADC_H__

#define SAMPLES 4		  // no of samples in moving average data set, value must be 4, 8, 16, 32 or 64
#define IGN_HIGH_SAMPLE 0 // adds one sample to the set and ignore peak high sample, value must be 0 or 1
#define IGN_LOW_SAMPLE 0  // adds one sample to the set and ignore peak low sample, value must be 0 or 1

#define DATA_SET SAMPLES + IGN_HIGH_SAMPLE + IGN_HIGH_SAMPLE // total samples in memory

#if (SAMPLES != 4) & (SAMPLES != 8) & (SAMPLES != 16) & (SAMPLES != 32) & (SAMPLES != 64) & (SAMPLES != 128)
#error "number of SAMPLES not valid!"
#endif


#if (SAMPLES == 8)
#define DIVB 3
#elif (SAMPLES == 16)
#define DIVB 4
#elif (SAMPLES == 32)
#define DIVB 5
#elif (SAMPLES == 64)
#define DIVB 6
#elif (SAMPLES == 128)
#define DIVB 7
#elif (SAMPLES == 4)
#define DIVB 2
#endif

#ifdef __cplusplus
extern "C"
{
#endif

unsigned long IRAM_ATTR millis();

//void adc_setPins(uint8_t dout, uint8_t sck); //constructor
void adc_setGain(uint8_t gain);		 //value should be 32, 64 or 128*
void adc_begin(uint8_t dout, uint8_t sck);
void adc_begin_g(uint8_t dout, uint8_t sck, uint8_t gain);
void adc_start(unsigned int t);		   // start and tare one HX711
int adc_startMultiple(unsigned int t); //start and tare multiple HX711 simultaniously
void tare();						   // zero the scale, wait for tare to finnish
void tareNoDelay();					   // zero the scale, do tare in loop without waiting for tare to finnish
void adc_setCalFactor(float cal);	   //calibration factor, raw data is divided by this value to convert to readable data
float adc_getCalFactor();			   // returns the current calibration factor
float adc_getData();				   // returns data from the moving average data set
float getSingleConversion();		   //for testing
void powerDown();
void powerUp();
bool getTareStatus(); // returns 1 if tareNoDelay() operation is complete
long getTareOffset();
void setTareOffset(long newoffset);
uint8_t adc_update(); //if conversion is ready; read out 24 bit data and add to data set

void conversion24bit(); //if conversion is ready: returns 24 bit data and starts the next conversion
long smoothedData();

#ifdef __cplusplus
}
#endif

#endif