/*
 *  ======== ti_freertos_config.c ========
 *  Configured FreeRTOS module definitions
 *
 *  DO NOT EDIT - This file is generated
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOSConfig.h"

/* C files contributed by /freertos/FreeRTOS */
#include <../list.c>
#include <../queue.c>
#include <../tasks.c>
#include <../timers.c>
#include <../croutine.c>
#include <../event_groups.c>
#include <../stream_buffer.c>
#include <../portable/MemMang/heap_4.c>

/* C files contributed by /freertos/dpl/Settings */
#include <dpl/AppHooks_freertos.c>
#include <dpl/DebugP_freertos.c>
#include <dpl/EventP_freertos.c>
#include <dpl/MessageQueueP_freertos.c>
#include <dpl/MutexP_freertos.c>
#include <dpl/QueueP_freertos.c>
#include <dpl/SemaphoreP_freertos.c>
#include <dpl/StaticAllocs_freertos.c>
#include <dpl/SwiP_freertos.c>
#include <dpl/SystemP_freertos.c>
#include <dpl/TaskP_freertos.c>
#include <dpl/ClockPLPF3_freertos.c>
#include <dpl/HwiPCC27XX_freertos.c>
#include <dpl/TimestampPLPF3_freertos.c>
#include <dpl/PowerCC27XX_freertos.c>
#include <startup/startup_cc27xx_ticlang.c>

/* C files contributed by /freertos/exception */
#include <exception/ExceptionArmV8M.c>

/* C files contributed by /ti/posix/freertos/Settings */
#define TI_POSIX_FREERTOS_MEMORY_ENABLEADV

#include <ti/posix/freertos/clock.c>
#include <ti/posix/freertos/memory.c>
#include <ti/posix/freertos/mqueue.c>
#include <ti/posix/freertos/pthread_barrier.c>
#include <ti/posix/freertos/pthread.c>
#include <ti/posix/freertos/pthread_cond.c>
#include <ti/posix/freertos/pthread_mutex.c>
#include <ti/posix/freertos/pthread_rwlock.c>
#include <ti/posix/freertos/sched.c>
#include <ti/posix/freertos/semaphore.c>
#include <ti/posix/freertos/sleep.c>
#include <ti/posix/freertos/timer.c>
#include <ti/posix/freertos/PTLS.c>
#include <ti/posix/freertos/aeabi_portable.c>


/* Wrapper functions for using the queue registry regardless of whether it is enabled or disabled */
void vQueueAddToRegistryWrapper(QueueHandle_t xQueue, const char * pcQueueName)
{
    /* This function is intentionally left empty as the Queue Registry is disabled */
}

void vQueueUnregisterQueueWrapper(QueueHandle_t xQueue)
{
    /* This function is intentionally left empty as the Queue Registry is disabled */
}


#include <exception/Exception.h>

/* FreeRTOS handlers */
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

/* Custom handler functions */

/* The entry point for the application startup code */
extern void resetISR(void);

/* Linker variables that marks the top and bottom of the stack */
extern unsigned long __STACK_END;

/* Initial vector table definition */
__attribute__((section(".resetVecs"), retain)) void (*const resetVectors[])(void) = {
    /* The initial stack pointer */
    (void (*)(void))((unsigned long)&__STACK_END),
    /* The reset handler */
    resetISR,
    /* The NMI handler */
    Exception_handlerSpin,
    /* The hard fault handler */
    Exception_handlerSpin,
    /* The MPU fault handler */
    Exception_handlerSpin,
    /* The bus fault handler */
    Exception_handlerSpin,
    /* The usage fault handler */
    Exception_handlerSpin,
    /* The secure fault handler */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* SVCall handler */
    SVC_Handler,
    /* Debug monitor handler */
    Exception_handlerSpin,
    /* Reserved */
    Exception_handlerSpin,
    /* The PendSV handler */
    PendSV_Handler,
    /* The SysTick handler */
    SysTick_Handler,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
    /* Peripheral interrupt */
    Exception_handlerSpin,
};
