


#include "Commands.h"


/*********************************************************************************************************************
-------------------------------------------------- Variables --------------------------------------------------
 *********************************************************************************************************************/

char* CMD[NUMBER_OF_COMMANDS];
char param_period[MAX_PERIOD_DIGITS_MS+1] = {0};
int param_period_number = 1000;
char param_num[MAX_NUM_DIGITS+1] = {0};
int param_num_number = 5;
char PARAM[NUMBER_OF_PARAMS];
unsigned set_params[NUMBER_OF_PARAMS] = {false};


/*********************************************************************************************************************
-------------------------------------------------- Static Functions --------------------------------------------------
 *********************************************************************************************************************/



/***************************************************
Checks if a character is a number or not
***************************************************/
static int isNum(char ch){
  int temp = ch - '0';
  if(temp<0 || temp>9) return false;
  return true;
}


/***************************************************
Extracts the given number of characters from
the source to the destination.
***************************************************/
static int ExtractCharactersTo(char* source, unsigned start_i, unsigned stop_i, char* destination, unsigned limit){
  int y = 0;
  for(int i=start_i; i<stop_i; ++i){
    if(y>=limit){event = STOP_EVENT; return false;} //if we reached the limit leave the for loop
    destination[y] = source[i];
    y++;
  }

  for(int y; y<limit; y++){ //The rest of the characters are null chars
    destination[y] = 0;
  }

  event = PARAMS_EVENT;
  return true;
}


/***************************************************
Returns n^exp
***************************************************/
static int power(int n, unsigned exp){

	int n_base = n; //This should not change
	if(exp == 0) return 1;
		for(int i=0; i<(exp-1); ++i){
		n*=n_base;
	}
	return n;
}


/***************************************************
Converts the given number of characters to int
***************************************************/
static void CharToInt(char* source, unsigned n, int* destination_number){
	for(int i=0; i<n; ++i){
		(*destination_number) += (source[i]-'0')*power(10,(n-i-1));
	}
}


/***************************************************
Returns true if it is a parameter
***************************************************/
static unsigned isParam(char ch){
  for(int i =0; i<NUMBER_OF_PARAMS; ++i){
    if(ch==PARAM[i]) return true;
  }
  return false;
}


/***************************************************
Initializes the parameters
***************************************************/
static void InitPARAM(){
  PARAM[PARAM_PERIOD]   =   'p';
  PARAM[PARAM_NUM]      =   'n';
}


/***************************************************
Returns index of the parameter if it is a valid one
***************************************************/
static int paramIndex(char ch){
  for(int i =0; i<NUMBER_OF_PARAMS; ++i){
    if(ch == PARAM[i]) return i;
  }
  return -1;
}


#ifdef CHECK_FOR_SIMILAR_COMMANDS
static int CheckForSimilarCommands(unsigned char* data){
	int ret = 0;

	int len = strlen((char*)data);

	for(int i=0; i<len; ++i){
		if(data[i] == '1') 	ret = CMD_STARTM1; 	//If it contains a '1' it is the startm1 command
		if(data[i] == '2') 	ret = CMD_STARTM2; 	//If it contains a '2' it is the startm2 command
		if(data[i] == 'O') 	ret = CMD_STOP;		//If it contains a 'O' it is the stop command
	}

	return ret;
}
#endif

/*********************************************************************************************************************
-------------------------------------------------- Functions --------------------------------------------------
 *********************************************************************************************************************/

/***************************************************
Initializes the command strings
***************************************************/
void InitCMD(){
   CMD[CMD_START] 		= 	"$START$";
   CMD[CMD_STOP] 		= 	"$STOP$";
   CMD[CMD_SLEEP] 		= 	"$SLEEP$";
   CMD[CMD_STARTM1]		=	"$STARTM1$";
   CMD[CMD_SLEEP_DEBUG] =   "$SLEEP_DEBUG$";
   CMD[CMD_STARTM2]		=	"$STARTM2$";

   InitPARAM();
}


/***************************************************
Verifies the command string
***************************************************/
int VerifyCommand(unsigned char* data){
  int ret = 0;

  for(int i=0; i<NUMBER_OF_COMMANDS; ++i){
    if(strcmp((char*)data, CMD[i])==0){
    	ret = i;
    	break; //Exit for loop
    }
  }

#ifdef CHECK_FOR_SIMILAR_COMMANDS
  if(ret == 0){ //No results so far
	 ret = CheckForSimilarCommands(data);
  }
#endif

  return ret;
}



/***************************************************
Executes the given command
***************************************************/
void ExecuteCommand(int cmd){

  switch(cmd){

    case CMD_START:{
       event = START_EVENT;
      break;
    }

    case CMD_STOP:{
      event = STOP_EVENT;
      break;
    }

    case CMD_STARTM1:{
    	event = STARTM1_EVENT;
    	break;
    }

    case CMD_SLEEP_DEBUG:{
     event = SLEEP_DEBUG_EVENT;
     break;
   }

    case CMD_STARTM2:{
    	event = STARTM2_EVENT;
    	break;
    }

    default:{
      break;
    }

  }

}


/***************************************************
Saves the given parameter
***************************************************/
void SaveParam(char* data, int n){
  if(data[0] == PARAM_CHAR){
    int start_i = 0;
    unsigned start_i_found = false;
    int stop_i = 0;

    //check the parameter character stream
    for(int i=0; i<n; ++i){

      //if we havent found the place of start_index && it is a parameters char ---> we found the start_i
      if(!start_i_found && isParam(data[i])){ start_i = i; start_i_found =true;}

      //if we are not standing on the same char as before && we already found the start && it is also a param ---> we found the stop_i
      if(i!=start_i && start_i_found && isParam(data[i])) stop_i = i;

      //If its not the starting character && it is not a param char && it is also not a number ---> it is invalid so we stop here
      if(i!=start_i && !isParam(data[i]) && !isNum(data[i])) stop_i = i;

      //if we are at the end of the stream
      if(i==n-1) stop_i = i;

      if(start_i_found && stop_i > start_i){
         switch(paramIndex(data[start_i])){

           case PARAM_PERIOD:{
            if(ExtractCharactersTo(data, start_i+1, stop_i, param_period, MAX_PERIOD_DIGITS_MS)){
            	set_params[PARAM_PERIOD] = true;
            	CharToInt(param_period, (stop_i-start_i-1), &param_period_number);
            }
            break;
           }

           case PARAM_NUM:{
            if(ExtractCharactersTo(data, start_i+1, stop_i, param_num, MAX_NUM_DIGITS)){
            	set_params[PARAM_NUM] = true;
            	CharToInt(param_num, (stop_i-start_i-1), &param_num_number);
            }
            break;
           }

           default:  break; //Unkown parameter
         }
         start_i_found = true;
         start_i = stop_i; //The next paramter is at the end of the previous one
      }
    }

  }
  else{
    //Not a parameter
  }

}


/***************************************************
The parameters should be reset
before receiving new ones
***************************************************/
void ResetParams(){
  for(int i=0; i<MAX_PERIOD_DIGITS_MS+1; ++i){
    param_period[i] = 0;
  }
  for(int i=0; i<MAX_NUM_DIGITS+1; ++i){
   param_num[i] = 0;
  }

  for(int i=0; i<NUMBER_OF_PARAMS; ++i){
    set_params[i] = false;
  }

  param_period_number = 0;
  param_num_number = 0;

}
