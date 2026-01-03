#include "HL_gio.h"
#include "HL_rti.h"
#include "HL_sci.h"
#include "HL_system.h"

#include "bl_config.h"
#include "sci_common.h"

#include "Application.h"

int main(void)
{
    /* Initialize RTI driver */
    gioInit();
    gioSetDirection(gioPORTB, 0xFFFF);

    /*Enable RTI to check if interrupt affect flash erase/program*/
    rtiInit();
    /* Enable RTI Compare 0 interrupt notification */
    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE0);

    /* Enable IRQ - Clear I flag in CPS register */
    /* Note: This is usually done by the OS or in an svc dispatcher */
    _enable_IRQ_interrupt_();

    /* Start RTI Counter Block 0 */
    rtiStartCounter(rtiREG1,rtiCOUNTER_BLOCK0);

    /* Initialize SCI Routines to receive Command and transmit data */
    sciInit();

    UART_putString(UART, "Hercules MCU UART BootLoader\n");

    f_ApplicationInit();

    while (1)
    {
        f_ApplicationControl();
    }
}
