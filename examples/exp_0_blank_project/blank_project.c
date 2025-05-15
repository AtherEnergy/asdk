/* Platform dependent includes */
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"


int main()
{
    asdk_sys_enable_interrupts();
    
    asdk_clock_config_t clk_cfg = {
    .clk_source = ASDK_CLOCK_SRC_INT,
    .pll = {
        .input_frequency = 8000000,
        .output_frequency = 160000000,
    },
    .xtal_trim = {0}};

    asdk_clock_init(clk_cfg);


    for (;;)
    {
        ;
    }

    return 0;
}
