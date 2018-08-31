

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

/**************************************************
 *  Includes
**************************************************/

#include "em_adc.h"
#include "em_cmu.h"
#include "GPIO.h"

#define VMCU			3.3
#define	ADC_MAX_VALUE	4096.0
#define VBAT			3.0
#define ADC_BUFFER_SIZE	64

#define ADC_FORCE0		0
#define ADC_FORCE1		1
#define ADC_FORCE2		2
#define ADC_FORCE3		3
#define ADC_FORCE4		4
#define ADC_BATTERY		5

#define ADC_SCAN_CH_NUMBER      5

/**************************************************
 *  Functions
**************************************************/
void InitADC();
uint32_t GetADCvalue(void);
double ADC_to_Voltage(uint32_t ADCvalue);
double Voltage_to_force(double volt);
uint32_t GetADCvalue_Force(unsigned channel);
double getVDD(unsigned int average);
double BatteryVoltageMeasurement();
//double ADC_to_Voltage_forBattery(uint32_t ADCvalue);
//uint16_t Convert32to16bit(uint32_t source);
//double BatteryVoltageMeasurement();
#endif /* SRC_ADC_H_ */
