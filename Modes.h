

#ifndef SRC_MODES_H_
#define SRC_MODES_H_

/*************************************************
 * Includes
 **************************************************/

#include "GPIO.h"
#include "SI7021.h"
#include "ADC.h"
#include "RFDuino.h"
#include "EmMode.h"
#include "RTC.h"
#include "EEPROM.h"
#include "Flash.h"
/*************************************************
 * Functions
 **************************************************/

void EnterPowerSaving();
void EnterEM2(void);
void MotionSensing();
void PowerSavingModeNotification(unsigned mode);
void ContinousMeasurement();
void SendAndSaveDatas(int n, int period);
//void SendAndSaveDatasPeriod(int n, int period);
void readADConly();
void getVDDdatas();
double hgmm_ebay(uint32_t f);
uint32_t AVG(int n,int f);
double forceNewton(uint32_t f);
double hgmm (uint32_t f);
double quickMeas(uint32_t f);
double forceing(uint32_t f);
void mesurements_for_testing(int n, int p);
/*************************************************
 * Defines
 **************************************************/

#define MODE0		0
#define MODE1		1
#define MODE2		2
#define MODE3		3

#define PARAMETRIC_MEASUREMENT_STORE_SIZE	100

#endif /* SRC_MODES_H_ */
