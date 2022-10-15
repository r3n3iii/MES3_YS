// ----------------------------------------------------------------------------
// myTask.c
// ----------------------------------------------------------------------------
//***** Header Files **********************************************************
// XDC module Header Files
#include <xdc/std.h>                                                            // XDC "base types" - must be included FIRST
#include <xdc/runtime/Types.h>                                                  // XDC constants/types used in xdc.runtime pkg
#include <xdc/cfg/global.h>                                                     // For all BIOS instances created statically in RTOS .cfg file
#include <xdc/runtime/Log.h>                                                    // For any Log_info() call
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
//#include <xdc/runtime/Timestamp.h>

// TI-RTOS Kernel Header Files
#include <ti/sysbios/BIOS.h>                                                    // BIOS module API
#include <ti/sysbios/knl/Task.h>                                                // Task APIs
#include <ti/sysbios/knl/Semaphore.h>                                           // Semaphore APIs
#include <ti/sysbios/knl/Mailbox.h>
// Standard C Header Files
#include <stdint.h>                                                             // Standard integer types
#include <stddef.h>                                                             // Standard variable types and macros
#include <stdio.h>                                                              // For printf()
#include <stdbool.h>
// Peripheral Driver Header Files
#include "ti_drivers_config.h"                                                  // Syscfg Board/Drivers Header file
#include <ti/drivers/GPIO.h>                                                    // TI GPIO Driver header file
#include <ti/drivers/UART.h>

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/adc14.h>
#include <arm_math.h>



#include "myMailbox.h"

//***** Defines ******************************************************
#define DEBUG           1                // Use to print EMG data to serial. if DEBUG not define, deletes taskPrintUart
#define TKEO            1               //1 Use TKEO for threshold, if 0 use three times the std.
#define REPEAT_MODE     true
#define NO_REPEAT       false
#define EMGBUFF         20              //Length of the buffer for emg tkeo calculation
              //Length of the buffer for FSR mean calculation
#define CHANNELS        3


extern Semaphore_Handle     semEMG, semEmgRMS, semUartRx;
extern Mailbox_Handle       Print_Mbx /*, EMG_Mbx, FSR_Mbx*/;
extern Task_Handle          taskPrintUart;
static float convertToFloat(uint16_t);

//***** Global Variables ******************************************************
float32_t   adcEMG0[EMGBUFF], adcEMG1[EMGBUFF];
float32_t   adcEMG0Rms, adcEMG1Rms;



float32_t  thresholdOn0 = 0;
float32_t  thresholdOn1 = 0;
float32_t  thresholdOff0 = 0;
float32_t  thresholdOff1 = 0;


float32_t DC0 = 0;float32_t DC1 = 0;
uint16_t i=0;



bool    recalibrate = 1;
uint16_t i_emg=0;
//*****************************************************************************
// Task EMG Function - Reads ADC and stores in buffer
//*****************************************************************************
void taskEmgFxn(UArg a0, UArg a1)
{

    float32_t  adcEmg_pr0[EMGBUFF], adcEmg_pr1[EMGBUFF];
    memset(adcEmg_pr0, 0x00, EMGBUFF * sizeof(float32_t));
    memset(adcEmg_pr1, 0x00, EMGBUFF * sizeof(float32_t));


    while (1)
    {
        Semaphore_pend(semEMG, BIOS_WAIT_FOREVER);                              // Wait for semaphore from HWI callback function

        uint64_t status = 0;
        while((status & ADC_INT1)==0)                           //Spin here until ADC finish conversion for EMG channels
        {
            MAP_ADC14_toggleConversionTrigger();
            status = MAP_ADC14_getEnabledInterruptStatus();
            MAP_ADC14_clearInterruptFlag(status);
        }

        //ADC_MEM0 = EmgSensor on P5.4, ADC_MEM1 = EmgSensor on P5.0

        adcEmg_pr0[i_emg] = convertToFloat(MAP_ADC14_getResult(ADC_MEM0)-8191);            //Converting EMG signal to float an mV
        adcEmg_pr1[i_emg] = convertToFloat(MAP_ADC14_getResult(ADC_MEM1)-8191);
        i_emg++;
        if (i_emg==EMGBUFF)
        {
           memcpy(&adcEMG0, &adcEmg_pr0, sizeof(adcEmg_pr0));                   //coping buffer emg to a global variable buffer
           memcpy(&adcEMG1, &adcEmg_pr1, sizeof(adcEmg_pr1));                   //to use in RMS task function
           i_emg=0;
           Semaphore_post(semEmgRMS);                                           //Making EmgRms task ready to run
        }
    }

}
//Use for time measurement
/* uint32_t start, stop, t0, t1, t2;
   t0 = Timestamp_get32();
   t1 = Timestamp_get32();
   t2 = t1 - t0;
   start = Timestamp_get32();
   //Any Function
   stop = Timestamp_get32();
   uint32_t result = stop - start - t2;
   Log_info1("[%u] cycles", result);*/


//*****************************************************************************
// Task EMG RMS Function
//*****************************************************************************
void taskEmgRmsFxn(UArg arg0, UArg arg1)
{

    MsgObjPrint  msgPrint;

    float32_t tkeoBuff0[EMGBUFF];     memset(tkeoBuff0, 0x00, (EMGBUFF) * sizeof(float32_t));
    float32_t tkeoBuff1[EMGBUFF];     memset(tkeoBuff1, 0x00,(EMGBUFF )* sizeof(float32_t));
    float32_t abs_tkeoBuff0[EMGBUFF]; memset(abs_tkeoBuff0, 0x00, (EMGBUFF) * sizeof(float32_t));
    float32_t abs_tkeoBuff1[EMGBUFF]; memset(abs_tkeoBuff1, 0x00, (EMGBUFF) * sizeof(float32_t));
    float32_t tkeoMean0=0;
    float32_t tkeoMean1=0;
    float32_t LPmean0=0;
    float32_t LPmean1=0;


    //bool onset= 0;
    //bool offset= 0;
    float32_t adcEMGRms[2];

    memset(adcEMGRms, 0x00, 2 * sizeof(float32_t));
    //Task_sleep(1000);
    while(1)
    {
        Semaphore_pend(semEmgRMS,BIOS_WAIT_FOREVER );
        //arm_rms_f32(adcEMG0, EMGBUFF, &adcEMGRms[0]);
        //arm_rms_f32(adcEMG1, EMGBUFF, &adcEMGRms[1]);

        arm_abs_f32(adcEMG0, abs_tkeoBuff0, EMGBUFF);
        arm_abs_f32(adcEMG1, abs_tkeoBuff1,  EMGBUFF);
        arm_mean_f32(abs_tkeoBuff0,EMGBUFF,&tkeoMean0);
        arm_mean_f32(abs_tkeoBuff1,EMGBUFF,&tkeoMean1);

        LPmean0 = LPmean0*0.5+tkeoMean0*0.5;
        LPmean1 = LPmean1*0.5+tkeoMean1*0.5;

        msgPrint.emgPlot[0]=LPmean0;
        msgPrint.emgPlot[1]=LPmean1;

        if(thresholdOn0>LPmean0)
            thresholdOn0=LPmean0;
        else
           thresholdOn0= 0.99 * thresholdOn0 + 0.01 * LPmean0;

        if(thresholdOn1>LPmean1)
            thresholdOn1=LPmean1;
        else
            thresholdOn1= 0.99 * thresholdOn1 + 0.01 * LPmean1;


        #ifdef DEBUG
            Mailbox_post(Print_Mbx, &msgPrint, BIOS_WAIT_FOREVER);
        #else
            Task_delete(&taskPrintUart);
        #endif

    }
}


//*****************************************************************************
// Convert to float Function
//*****************************************************************************
static float convertToFloat(uint16_t result)
{
    int32_t temp;

    if(0x8000 & result)
    {
        temp = (result >> 2) | 0xFFFFC000;
        return ((temp * 2.5f) / 8191);
    }
    else
        return ((result >> 2)*2.5f) / 8191;
}


//*****************************************************************************
// fnTaskUART Function - Task for this function is created statically.
// See the project's .cfg file.
//*****************************************************************************

void taskCLIFxn(UArg arg0, UArg arg1)
{

    while(1)
    {


        Task_sleep(10);
    }

}

//*****************************************************************************
// fnTaskUARTPrint Function - .

//*****************************************************************************

void taskUartPrintFxn(UArg arg0, UArg arg1)
{
    MsgObjPrint  msg;
    while(1)
    {

    Mailbox_pend(Print_Mbx, &msg, BIOS_WAIT_FOREVER);
    long res = (long)(msg.emgPlot[0]*10000); long res1 = (long)(msg.emgPlot[1]*10000); long thrs = (long)(thresholdOn0*10000); long thrs1 = (long)(thresholdOn1*10000);

    //printf_new(EUSCI_A0_BASE, "%l, %l, %l %l \r\n",res,res1, thrs, thrs1);   //for visualization using ENERGIA Serial Plotter
    printf("%.5f, %.5f, %.5f\r\n",msg.emgPlot[0],msg.emgPlot[1],thresholdOn1);
    //printf_new(EUSCI_A0_BASE, "$%i %i; \r\n",res,res1);  //for recording using EMGPlot software
    }

}

//*****************************************************************************
// taskMPU6050Fxn Function - .

//*****************************************************************************


void taskMPU6050Fxn(UArg a0, UArg a1){

    while(1)
    {
        Task_sleep(10);
    }

}
