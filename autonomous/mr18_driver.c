/*
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 *
 *
 * Copyright 2019, Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
/* app_handler.c: App layer handling function implementation */

#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "system.h"
#include <string.h> //to enable memset command

#include "app.h"

#include "log.h"
#include "debug.h"
#define DEBUG_MODULE "PUSH"

#include "i2cdev.h"

#ifndef APP_STACKSIZE
#define APP_STACKSIZE 300
#endif

#ifndef APP_PRIORITY
#define APP_PRIORITY 2
#endif

#define COMMAND_START_MEASURE 1
#define COMMAND_END_MEASURE 2
#define COMMAND_SET_PARAMS 3
#define TimingBudgetID 2
/*
TimingBudgetID
    0 – 15mS    - NOT WORKING PROPERLY
    1- 20mS == 50hz
    2- 33mS
    3- 50mS
    4- 100mS
    5- 200mS
    6- 500mS
*/
#define DistanceModeID 1 // 4 meters

static uint16_t result_logger[18] = {0};

static uint8_t devAddr = 0x70;
// static uint16_t result=0;
static I2C_Dev *I2Cx;

static bool isInit = false;

static void appTask(void *param);

void __attribute__((weak)) appInit()
{
  if (isInit) return;

  xTaskCreate(appTask, "app", APP_STACKSIZE, NULL,
              APP_PRIORITY, NULL);
  isInit = true;
}

static void appTask(void *param)
{
  systemWaitStart();

  appMain();

  while(1) {
    vTaskDelay(portMAX_DELAY);
  }
}

void appMain()
{
	  DEBUG_PRINT("***** I2C sensor test ***** \n");

	  I2Cx = I2C1_DEV; //i2c port 1 = deck bus

		vTaskDelay(M2T(1500));

		bool success;
		unsigned char buffer[4] = {0};
		int length=4;
		buffer[0] = COMMAND_SET_PARAMS;
		buffer[1] = TimingBudgetID;
		buffer[2] = DistanceModeID;
		buffer[3] = 0;

		success=i2cdevWrite(I2Cx, devAddr, length,(uint8_t *)&buffer );
	    //success=i2cdevReadReg8(I2Cx, devAddr, 0xC0, 2, (uint8_t *)&buffer); // 0xC0 is the "sensor id" register

	    if(success) {
	    	DEBUG_PRINT("set params successful\n");
	    }
	    else {
	    	DEBUG_PRINT("set params fail\n");
	    }

		vTaskDelay(M2T(500));


		memset(buffer, 0, sizeof(buffer));
		length=4;

		buffer[0] = COMMAND_START_MEASURE;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;

		success=i2cdevWrite(I2Cx, devAddr, length,(uint8_t *)&buffer );

	    if(success) {
	    	DEBUG_PRINT("command start measure successful\n");
	    }
	    else {
	    	DEBUG_PRINT("command start measure fail\n");
	    }

	  while(1) {
			vTaskDelay(M2T(50));
			int length=39;
			unsigned char measurement_buffer[39] = {0};
			i2cdevRead(I2Cx, devAddr, length,(uint8_t *)&measurement_buffer );
//		    if(success) {
//		    	DEBUG_PRINT("MR18-R\n");
//		    }
//		    else {
//		    	DEBUG_PRINT("MR18 measure fail\n");
//		    }
		    uint8_t i=0;
//		    for (i=0; i<length; i++){
////		    	DEBUG_PRINT("reading num %i = %i , ", i, measurement_buffer[i]);
//		    	//DEBUG_PRINT("%i , ", measurement_buffer[i]);
//		    }
		    //DEBUG_PRINT("\n");

		    //uint16_t result_buffer[18] = {0};
		    uint8_t j=0;
		    //uint16_t result = (uint16_t)(measurement_buffer[3] << 8) + (uint16_t)(measurement_buffer[4]);
		    for (i=3; i<39; i+=2){
		    	//result_buffer[j] = (uint16_t)(measurement_buffer[i] << 8) + (uint16_t)(measurement_buffer[i+1]);
		    	result_logger[j] = (uint16_t)(measurement_buffer[i] << 8) + (uint16_t)(measurement_buffer[i+1]);
		    	j++;
		    }

//		    for (i=0; i<18; i++){
//		    	//DEBUG_PRINT("%i , ", result_buffer[i]);
//		    }
		    //DEBUG_PRINT("\n");
	  }
}

LOG_GROUP_START(mr18)
LOG_ADD(LOG_UINT16, m0, &result_logger[0])
LOG_ADD(LOG_UINT16, m1, &result_logger[1])
LOG_ADD(LOG_UINT16, m2, &result_logger[2])
LOG_ADD(LOG_UINT16, m3, &result_logger[3])
LOG_ADD(LOG_UINT16, m4, &result_logger[4])
LOG_ADD(LOG_UINT16, m5, &result_logger[5])
LOG_ADD(LOG_UINT16, m6, &result_logger[6])
LOG_ADD(LOG_UINT16, m7, &result_logger[7])
LOG_ADD(LOG_UINT16, m8, &result_logger[8])
LOG_ADD(LOG_UINT16, m9, &result_logger[9])
LOG_ADD(LOG_UINT16, m10, &result_logger[10])
LOG_ADD(LOG_UINT16, m11, &result_logger[11])
LOG_ADD(LOG_UINT16, m12, &result_logger[12])
LOG_ADD(LOG_UINT16, m13, &result_logger[13])
LOG_ADD(LOG_UINT16, m14, &result_logger[14])
LOG_ADD(LOG_UINT16, m15, &result_logger[15])
LOG_ADD(LOG_UINT16, m16, &result_logger[16])
LOG_ADD(LOG_UINT16, m17, &result_logger[17])
LOG_GROUP_STOP(mr18)
