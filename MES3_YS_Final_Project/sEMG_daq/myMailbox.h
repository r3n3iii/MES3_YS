// ----------------------------------------------------------------------------
// myMailbox.h
// ----------------------------------------------------------------------------

#ifndef MYMAILBOX_H_
#define MYMAILBOX_H_
#define __FPU_PRESENT 1
#include <ti/sysbios/knl/Mailbox.h>                                             // For mailbox APIs
#include <arm_math.h>


#define EMG_CHANNELS 2

//***** Global Variables ******************************************************
typedef struct MsgObjPrint {
    float32_t  emgPlot[EMG_CHANNELS];

} MsgObjPrint;


#endif /* MYMAILBOX_H_ */

