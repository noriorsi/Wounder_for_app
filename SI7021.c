
#include "SI7021.h"
#include "RFDuino.h"
/******************************************************************************************************************
 *  Static variables
******************************************************************************************************************/

//static 	uint32_t 	humData;
//static 	int32_t  	tData;
static I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_new;

/*********************************************************************
 * Initializes SI7021 temperature and humidity sensor
*********************************************************************/
void InitSI7021(){
	/* Enable GPIO clock */
	  CMU_ClockEnable(cmuClock_GPIO, true);

	  /* Enable si7021 sensor isolation switch */
	  //GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1);
	  //GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 1);
	  //GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 1);

	  I2CSPM_Init(&i2cInit);
}



/*********************************************************************
 * Uses the on-board SI7021 to measure humidity and temperature
*********************************************************************/
/*void MeasureHumAndTemp(I2C_TypeDef *i2c, uint32_t *humData, int32_t *tData){
	Si7013_MeasureRHAndTemp(i2c, SI7021_ADDR, humData, tData);
}*/

void SI7021_Measure(uint32_t *humData, int32_t *tData){
	Si7013_MeasureRHAndTemp(i2cInit.port, si7021_addr, humData, tData);
}


/*********************************************************************
 * Uses the on-board SI7021 to measure humidity and temperature
*********************************************************************/
/*void DisplayTempData(){
	//MeasureHumAndTemp(i2cInit.port, &humData, &tData);
	MeasureHumAndTemp(&humData, &tData);
	double TEMP = tData / 1000.0;
	send_double(TEMP);
	//send_int(tData);

}*/
/*
void DisplayHumData(){
	//MeasureHumAndTemp(i2cInit.port, &humData, &tData);
	MeasureHumAndTemp(&humData, &tData);
	double HUM = humData / 1000.0;
	send_double(HUM);
		//send_string ("\n");
}

*/

/*********************************************************************
 * Performs a temperature and humidity measurement on the SI7021
*********************************************************************/
/*void SI7021_Measure(uint32_t *humData, int32_t *tData){
	Si7013_MeasureRHAndTemp(i2cInit.port, SI7021_ADDR, humData, tData);
}*/





