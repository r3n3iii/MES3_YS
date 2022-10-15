// ----------------------------------------------------------------------------
// main.c
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
// XDC module Header Files

#include <xdc/std.h>                                                            // XDC "base types" - must be included FIRST
#include <xdc/runtime/Types.h>                                                  // XDC constants/types used in xdc.runtime pkg
#include <xdc/cfg/global.h>                                                     // For all BIOS instances created statically in RTOS .cfg file
#include <xdc/runtime/Error.h>                                                  // For error handling (e.g. Error block)
#include <xdc/runtime/System.h>                                                 // XDC System functions (e.g. System_abort(), System_printf())
#include <xdc/runtime/Log.h>                                                    // For any Log_info() call

// TI-RTOS Kernel Header Files
#include <ti/sysbios/BIOS.h>                                                    // BIOS module API
#include <ti/sysbios/knl/Task.h>                                                // Task APIs
#include <ti/sysbios/knl/Semaphore.h>                                           // Semaphore APIs
#include <ti/sysbios/knl/Clock.h>                                               // Clock APIs

// Standard C Header Files
#include <stdint.h>                                                             // Standard integer types
#include <stddef.h>                                                             // Standard variable types and macros

// Peripheral Driver Header Files
#include "ti_drivers_config.h"                                                  // Syscfg Board/Drivers Header file
#include <ti/drivers/GPIO.h>                                                    // TI GPIO Driver header file
#include <ti/drivers/UART.h>

#include "myGpio.h"                                                             // Header file for myGpio.c
#include "myAdc.h"                                                              // Header file for myAdc.c
#include "myUart.h"                                                             // Header file for myUart.c                                                         // Header file for myMailbox.c
#include "myMailbox.h"
#include "MPU6050.h"
#include <ti/drivers/Board.h>



//***** Function Prototypes ************************************************************
void taskEmgFxn(UArg a0, UArg a1);                  //Read 2x differential analog EMG Sensors

void taskMPU6050Fxn(UArg a0, UArg a1);              // Read Accelerometer[3] and Gyro[3] data

void taskCLIFxn(UArg a0, UArg a1);                 //Send Data through serial from/to PC to/from CLI.

void taskFsrMeanFxn(UArg a0, UArg a1);              //Calculate mean of Force Sensing Resistors.
void taskEmgRmsFxn(UArg a0, UArg a1);               //Task to process EMG signals
void taskUartPrintFxn(UArg a0, UArg a1);            //Task to calculate Threshold
void HeartBeatFxn(void);                            //HeartBeat function


//***** Defines ***************************************************************


//***** Global Variables ******************************************************
Task_Handle         taskEmg, taskCLI,
                    taskEmgRMS,  taskPrintUart,
                    taskMPU6050;

Task_Params         taskParams;

Semaphore_Handle    semEMG, semEmgRMS, semUartRx;

Semaphore_Params    sem_params;

Error_Block         eb;

Clock_Handle        clk1Handle;
Clock_Params        clkParams;

Mailbox_Params      mbxParams;
Mailbox_Handle      Print_Mbx, Pololu_Mbx;

//*****************************************************************************
// Main
//*****************************************************************************
void main (void)
{
    // Initialize Peripherals
    Board_init();                                                               // Call TI Driver config init function
    myGpio_init();                                                              // Configure GPIO
    myADC_init();                                                               // Configure ADC
    myUart_init();                                                              // Configure UART
    MPU6050_I2C_init();

    // Create Tasks
    Task_Params_init(&taskParams);                                              // niEbt Task Params with pri=2, stackSize = 512
    taskParams.priority     = 10;
    taskParams.stackSize    = 3072;
    taskEmg =       Task_create(taskEmgFxn, &taskParams, Error_IGNORE);         //  passed, but not checked, return is pointer to taskEmgFxn
    if (taskEmg == NULL){
        System_abort("TaskEmgRMS create failed");
    }

    taskParams.priority     = 10;
    taskParams.stackSize    = 512;
    taskMPU6050 =       Task_create(taskMPU6050Fxn, &taskParams, Error_IGNORE);
    if (taskMPU6050 == NULL){
        System_abort("TaskEmgRMS create failed");
    }


    taskParams.priority     = 9;
    taskParams.stackSize    = 1024;
    taskEmgRMS =    Task_create(taskEmgRmsFxn, &taskParams, Error_IGNORE);
    if (taskEmgRMS == NULL){
        System_abort("TaskEmgRMS create failed");
    }

    taskParams.stackSize    = 3072;
    taskCLI =      Task_create(taskCLIFxn, &taskParams, Error_IGNORE);
    if (taskCLI == NULL){
         System_abort("TaskEmgRMS create failed");
     }

    taskParams.priority     = 7;
    taskPrintUart = Task_create(taskUartPrintFxn, &taskParams, Error_IGNORE);
    if (taskPrintUart == NULL){
             System_abort("TaskEmgRMS create failed");
     }

    // Create Semaphores
    Semaphore_Params_init(&sem_params);
    sem_params.mode = Semaphore_Mode_BINARY;
    semEMG =        Semaphore_create(0, &sem_params, &eb);                             // Create Sem, count=0, params default, EB passed but not checked
    semEmgRMS =     Semaphore_create(0, &sem_params, &eb);
    semUartRx =     Semaphore_create(0, &sem_params, &eb);

    // Create Mailbox
     Mailbox_Params_init(&mbxParams);
     Print_Mbx = Mailbox_create( sizeof(MsgObjPrint) ,                              // bufsize (size of INT)
                                 2,                                              // number of buffers = 2 (just the LED state, 2nd one is unused)
                                 &mbxParams,                                     // params
                                 Error_IGNORE);                                  // error block passed and pointer checked vs. NULL below
     if (Print_Mbx == NULL)
     {                                                      // If NULL handle, abort
            System_abort("mailbox create failed");
     }

    // Create Clock
    Clock_Params_init(&clkParams);                                           // Init Param structure
    clkParams.period =500/Clock_tickPeriod;                                  // Set period to 500 clock ticks (500ms) = 1/2sec
    clkParams.startFlag = TRUE;                                              // Start clock function immediately (when created)

    clk1Handle = Clock_create(
                                (Clock_FuncPtr)HeartBeatFxn,                 // Handle to clock function (posts the semaphore)
                                500/Clock_tickPeriod,                        // Set delay timeout to same value as clock period
                                &clkParams,
                                Error_IGNORE);                               // Eb passed and then handle is checked vs. NULL in next line of code

    if (clk1Handle == NULL)                                                  // If NULL handle, abort
    {
           System_abort("clock create failed");
    }
    __delay_cycles(500);
    BIOS_start();                                                            // Start TI-RTOS (SYS/BIOS kernel) services
}
