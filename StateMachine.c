

#include "StateMachine.h"

/*********************************************************************************************************************
-------------------------------------------------- Variables --------------------------------------------------
 *********************************************************************************************************************/

state_enum state = IDLE_STATE;
state_enum next_state;
event_enum event = NO_EVENT;

uint32_t state_timestamp = 0;

unsigned PARAMETRIC_active = false;
unsigned MODE2_active = false;

RTC_time_struct entered_parametric_timestamp = {0,0,0};

//A 2 dimensional array of function pointers. The functions return state_enum type and have no parameters.
state_enum (*state_table[MAX_STATES][MAX_EVENTS])(void)={

	//NO_EVENT				// START_EVENT            // STOP_EVENT				//StartM1_Event_Handler		//STARTM2_EVENT					//SLEEP_DEBUG_EVENT				//RFDUINO_GPIO_IT_EVENT				//ADXL_GPIO_IT_EVENT			//RTC_IT_EVENT				//TIMEOUT_EVENT				//PARAMS_EVENT

  { No_Event_Handler	,	Error_Event_Handler    ,   Error_Event_Handler	,	StartM1_Event_Handler	,	StartM2_Event_Handler		,	Sleep_Debug_Event_Handler	,	RFduino_GPIO_IT_Event_Handler	,	No_Event_Handler			,	No_Event_Handler		,	Error_Event_Handler		,	Error_Event_Handler},   //IDLE_STATE
  { No_Event_Handler	,	Error_Event_Handler    ,   Stop_Event_Handler	,	StartM1_Event_Handler	,	Error_Event_Handler			,	Sleep_Debug_Event_Handler	,	RFduino_GPIO_IT_Event_Handler	,	No_Event_Handler			,	No_Event_Handler		,	Error_Event_Handler		,	Error_Event_Handler},   //MODE1_STATE
  { No_Event_Handler	,	Error_Event_Handler    ,   Stop_Event_Handler	,	Error_Event_Handler		,	StartM2_Event_Handler		,	Sleep_Debug_Event_Handler	,	RFduino_GPIO_IT_Event_Handler	,	ADXL_GPIO_IT_Event_Handler	,	RTC_IT_Event_Handler		,	Error_Event_Handler		,	Error_Event_Handler},   //MODE2_STATE
  { No_Event_Handler	,	Start_Event_Handler    ,   Stop_Event_Handler	,	StartM1_Event_Handler	,	StartM2_Event_Handler		,	Sleep_Debug_Event_Handler	,	RFduino_GPIO_IT_Event_Handler	,	No_Event_Handler			,	No_Event_Handler		,	Timeout_Event_Handler	,	Params_Event_Handler},  //WAITING_FOR_COMMAND_STATE
  { No_Event_Handler	,	Error_Event_Handler    ,   Stop_Event_Handler	,	Error_Event_Handler		,	Error_Event_Handler			,	Sleep_Debug_Event_Handler	,	RFduino_GPIO_IT_Event_Handler	,	No_Event_Handler			,	RTC_IT_Event_Handler	,	Error_Event_Handler		,	Error_Event_Handler}   //PARAMETRIC_MEASUREMENT_STATE

};



/*********************************************************************************************************************
-------------------------------------------------- Functions --------------------------------------------------
 *********************************************************************************************************************/

/*************************************************************
 * When there is no event
 *************************************************************/
state_enum No_Event_Handler(void){
	switch(state){
		case IDLE_STATE:{
			SetGPIO(MCULED1_PORT, MCULED1_PIN, 1);
			Delay(500);
			SetGPIO(MCULED1_PORT, MCULED1_PIN, 0);
			Delay(500);
			EnterPowerSaving();
			break;
		}
		case MODE1_STATE:{
			//if(didElapseGivenSeconds((param_period_number/1000), entered_parametric_timestamp)){ //we need to check this here too else every RFduino IT leads here
			if(didElapseGivenSeconds((param_period_number/1000), entered_parametric_timestamp)){
			entered_parametric_timestamp = getTimeStructRTC(); 				//save the current time
										//send_int(entered_parametric_timestamp.seconds);
										Measure(1, 3000);			//measure and send

										GPIO_IntClear(RX_PIN_INT_MASK);									//enable RFDuino IT or else we won't be able to stop this state
										GPIO_IntEnable(RX_PIN_INT_MASK);
										//EnterEM3();
										//go to sleep
									}
									else {
										event = MAX_EVENTS;
										//EnterEM3();
									}
								//	EnterEM3();
									break;

						}


		case WAITING_FOR_COMMAND_STATE:{
			break;
		}


		case MODE2_STATE:{

			//FlashLeds(3);

			if(didElapseGivenSeconds(delay_between_measurement, entered_parametric_timestamp)){ //we need to check this here too else every RFduino IT leads here
													entered_parametric_timestamp = getTimeStructRTC();

													//Temporary_measurements(param_num_number, param_period_number);
													mesurements_for_testing(param_num_number, param_period_number);
													//getVDDdatas();

													GPIO_IntClear(RX_PIN_INT_MASK);									//enable RFDuino IT or else we won't be able to stop this state
													GPIO_IntEnable(RX_PIN_INT_MASK);
													EnterEM3();
										//go to sleep
									}
						else {
							event = MAX_EVENTS;
							EnterEM3();
						}

						break;

			}
				//return state; //return the same state

		case PARAMETRIC_MEASUREMENT_STATE:{
			if(didElapseGivenSeconds((param_period_number/1000), entered_parametric_timestamp)){ //we need to check this here too else every RFduino IT leads here
				entered_parametric_timestamp = getTimeStructRTC(); 				//save the current time
				Measure(param_period_number, param_num_number); 	//measure and send
				GPIO_IntClear(RX_PIN_INT_MASK);									//enable RFDuino IT or else we won't be able to stop this state
				GPIO_IntEnable(RX_PIN_INT_MASK);
				EnterEM3();
				//go to sleep
			}
			else {
				event = MAX_EVENTS;
				EnterEM3();
			}

			break;
		}
		default: break;
	}
	return state; //return the same state

}


/*************************************************************
 * Handles the start event
 *************************************************************/
state_enum Start_Event_Handler(void){
	MODE2_active = false;
		//ResetParams(); //Reset parameters just in case
		event = NO_EVENT;
		EnterPowerSaving();
		return IDLE_STATE;}


/*************************************************************
 * Handles the stop event
 *************************************************************/
state_enum Stop_Event_Handler(void){
	PARAMETRIC_active = false;
	MODE2_active = false;
	ResetParams(); //Reset parameters just in case
	event = NO_EVENT;
	EnterPowerSaving();
	return IDLE_STATE;
}

/*************************************************************
 * Error event handler
 *************************************************************/
state_enum Error_Event_Handler(void){
	PARAMETRIC_active = false;
	MODE2_active = false;
	event = NO_EVENT;
	return Stop_Event_Handler();
}

/*************************************************************
 * Mode1: Continuous acceleration and battery measurement
 *************************************************************/
state_enum StartM1_Event_Handler(void){
	event = NO_EVENT;
	return MODE1_STATE;
}


/*************************************************************
 * Mode2: Motion Sensing
 *************************************************************/
state_enum StartM2_Event_Handler(void){
	MODE2_active = true;
	//FlashLeds(3);
	entered_parametric_timestamp.seconds = -1;
	event = NO_EVENT;
	return MODE2_STATE;
}


/*************************************************************
 * Sends sleep command to the device then enters EM4
 *************************************************************/
state_enum Sleep_Debug_Event_Handler(void){
	EnterEM4();
	event = NO_EVENT;
	return IDLE_STATE;
}


/*************************************************************
 * Handles the IT from the RFduino
 *************************************************************/
state_enum RFduino_GPIO_IT_Event_Handler(void){
	state_timestamp = getTime();
	event = NO_EVENT;
	return WAITING_FOR_COMMAND_STATE;
}


/*************************************************************
 * Handles the IT from the ADXL
 *************************************************************/
state_enum ADXL_GPIO_IT_Event_Handler(void){
	event = NO_EVENT;
	return MODE2_STATE;
}


/*************************************************************
 * Handles the IT from the RTC
 *************************************************************/
state_enum RTC_IT_Event_Handler(void){

	if(didElapseGivenSeconds((delay_between_measurement), entered_parametric_timestamp)) event=NO_EVENT;
	else {
		event = MAX_EVENTS;
		EnterEM3();
	}
	//return PARAMETRIC_MEASUREMENT_STATE;
	return MODE2_STATE;
}



/*************************************************************
 * Checks if a timeout has occured
 *************************************************************/
void TimeoutChecker(){
	switch(state){
		case WAITING_FOR_COMMAND_STATE:{
			if( getTime() - state_timestamp >= WAITING_FOR_COMMAND_STATE_TIMEOUT) event = TIMEOUT_EVENT;
			break;
		}
		default: break;
	}
}


/*************************************************************
 * Handles timeout events
 *************************************************************/
state_enum Timeout_Event_Handler(void){

	switch(state){
		case WAITING_FOR_COMMAND_STATE:{
			//If we reached this timeout that means we didnt get any RX data --> so we have to reenable GPIO IT here
			GPIO_IntClear(RX_PIN_INT_MASK);
			GPIO_IntEnable(RX_PIN_INT_MASK);
			break;
		}
		default: break;
	}

	event = NO_EVENT;

	if(PARAMETRIC_active) 	return PARAMETRIC_MEASUREMENT_STATE;
	if(MODE2_active)		return MODE2_STATE;

	return IDLE_STATE;
}


/*************************************************************
 * Handles the incoming parameters
 *************************************************************/
state_enum Params_Event_Handler(void){
	PARAMETRIC_active = true;
	event = NO_EVENT;
	return PARAMETRIC_MEASUREMENT_STATE;
}
