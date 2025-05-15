/* ASDK: do not edit main file ********************************************* */

/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_app.h"

int main()
{
    asdk_sys_enable_interrupts();
    
    // asdk application
    asdk_app_init();
    asdk_app_loop();

    while(1);

    return 0;
}

/* ASDK: do not edit main file ********************************************* */
