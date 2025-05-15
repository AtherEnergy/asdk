# Pulse Width Modulation (PWM)

This document serves as reference for understanding and working with PWM module. It explains all it's features, shows how to use the PWM to generate the pulses with variable widths(duty cycle) with reference codes and snippets.

PWM stands for Pulse Width Modulation. It's a technique used in digital systems (like computers and microcontrollers) to simulate an analog signal using a digital source.

Imagine you have a light bulb connected to a switch. If you turn the switch on and off very quickly, faster than the human eye can see, the light bulb will appear dimmer than if the switch was left on all the time. That's because the light bulb is only on for a portion of the time.

The "pulse width" is how long the switch is left on. If you leave the switch on longer (a wider pulse), the light bulb will appear brighter. If you leave the switch on for a shorter time (a narrower pulse), the light bulb will appear dimmer.

So, by controlling the width of the pulse (how long the switch is left on), you can control things like the brightness of a light, the speed of a motor, or the position of a servo. That's the basic idea behind PWM.

## Typical Block Diagram of Pulse Width Modulation(PWM) is shown below:

<div align="center">
<img src="../../images/pwm/pwm_block_diag.png" width=640/>
<p>The above image represents PWM Block Diagram</p>
</div>


Key components of PWM block diagram are explained below:
1. **Clock Source** : This is the input to the system. The frequency of this clock determines the frequency of the PWM signal.
2. **Prescaler** :A prescaler in a PWM (Pulse Width Modulation) system is a component that divides the frequency of the clock source before it is fed into the counter.
The prescaler is typically used to decrease the frequency of the clock signal, allowing for a wider range of PWM frequencies. For example, if the clock source is running at 16 MHz and the prescaler is set to divide by 8, the effective clock frequency fed to the counter would be 2 MHz.
2. **Counter** : This counts up to a certain value (the period) and then resets to zero. The value of the counter is continuously compared to the PWM duty cycle.
3. **Comparator** : This compares the current counter value to the duty cycle. When the counter is less than the duty cycle, the PWM signal is high. When the counter is greater than the duty cycle, the PWM signal is low.
4. **PWM Signal** : This is the output of the system. The duty cycle of this signal is controlled by the comparator.

The block diagram might also include additional components for more advanced features, such as dead time insertion or fault protection.

<div align="center">
<img src="../../images/pwm/pwm_duty.png" width=640/>
<p>The above image represents PWM</p>
</div>

The above plot captures two features:Frequency and Duty Cycle.Let's understand what each term means:

1. **PWM Frequency** : The frequency of a PWM (Pulse Width Modulation) signal refers to how often the signal completes a full cycle. It is typically measured in Hertz (Hz), which is a unit meaning cycles per second. A higher frequency means the signal cycles on and off more quickly. The appropriate frequency depends on the application. For example, to control a LED light you might use a frequency of a few hundred Hz, while to control a motor you might need a frequency of several kilohertz.

2. **Duty Cycle**  : The duty cycle of a PWM signal is the percentage of one period in which the signal is active or high. It's expressed as a percentage. For example, a duty cycle of 50% means the signal is on half the time and off half the time. A duty cycle of 100% means the signal is always on, and a duty cycle of 0% means the signal is always off. By adjusting the duty cycle, you can control things like the brightness of a LED (a higher duty cycle means a brighter light) or the speed of a motor (a higher duty cycle means a faster motor).

Hence,  
* Perid(T) = Ton +  Toff.
* Frequency = 1/(Period(Ton+Toff)).
* Duty Cycle = Ton/(Ton+Toff).


## PWM Jargons

Below are some common PWM(Pulse Width Modulation) jargons:

* **Duty Cycle** :The percentage of one period in which the signal is active or high. It's expressed as a percentage. For example, a duty cycle of 50% means the signal is on half the time and off half the time.

* **Frequency** :The frequency of a PWM signal refers to how often the signal completes a full cycle. It is typically measured in Hertz (Hz), which is a unit meaning cycles per second.

* **Period** :The duration of one complete cycle of the PWM signal. It is the sum of the time the signal is on (Ton) and the time the signal is off(Toff).

These terms are commonly used when working with PWM signals in microcontroller-based systems.

Below is a code snippet to configure a pin as PWM.

```c

#define PWM_PIN MCU_PIN_47

static asdk_pwm_config_t pwn_config = {
    .mcu_pin = PWM_PIN,                              // Pin for PWM 
    .pwm_mode = ASDK_PWM_MODE_PWM,                   // Use PWM mode only
    .pwm_run_mode = ASDK_PWM_RUN_MODE_CONTINUOUS,    // Output continuously
    .direction = ASDK_PWM_COUNT_DIRECTION_UP,        // Use up counter
    .interrupt = {.enable = false},                  // Disable PWM callbacks
    .pwm_output_align = ASDK_PWM_OUTPUT_LEFT_ALIGN,  // Left aligned
    .pwm_clock = {                                   // Use 1 Mhz clock for PWM module
        .clock_frequency = 1000000,
        .prescaler = ASDK_PWM_PRESCALER_DIVBY_1,
    },
    .pwm_frequency_in_Hz = 1000,                     // Desired PWM frequency
};


asdk_pwm_init(ASDK_PWM_MODULE_CH_38, &pwn_config);
```


## Features

Following set of features are made available to the user in the PWM peripheral.These features are subject to hardware support.

* **Duty Cycle Control** :This allows the control of the percentage of time the PWM signal is high (ON state) within a period. This is used to control the amount of power delivered to a device.ASDK has the provision of updating the duty cycle of the PWM during run time just by updating the appropriate parameter in the `asdk_pwm_set_frequency` API.

* **Frequency Control** :This allows the control of how often the PWM signal completes a full cycle. This is used in applications where the rate of power delivery needs to be controlled.ASDK has given the support of Frequency control of a  PWM signal by calling the `asdk_pwm_set_frequency` API with appropriate parameters.

* **PWM Mode** :In ASDK user can select the PWM Mode if it needs to be PWM or PWM with dead time insertion.'

* **Dead Time** :This is a safety feature used in applications where complementary PWM signals are used. It prevents both signals from being high at the same time, which could cause a short circuit.In ASDK user can configure the dead time clock cycles in the complementary PWM signals.

* **PWM Alignment** :PWM Alignment refers to how the PWM signal is generated with respect to the PWM period. There are typically two types of alignments: Edge-aligned and Center-aligned.
    * **Edge-Aligned PWM** : In this mode, the PWM signal starts at the beginning of the period and ends when the count matches the duty cycle. This is the most common type of PWM and is used in a wide variety of applications.
    * **Center-Aligned PWM** : In this mode, the PWM signal is high in the center of the period and low at the edges. The signal goes high when the count matches half the period minus half the duty cycle, and goes low when the count matches half the period plus half the duty cycle. This type of PWM is often used in power electronics and motor control, as it can reduce harmonic distortion.

## PWM Interrupt and Callbacks

PWM interrupts and callbacks  are used to execute specific tasks or functions when certain events occur in the PWM module. In the context of PWM, interrupts can be generated for various events such as when the PWM counter reaches a certain value, or when a PWM period ends.

Each pin when configured as PWM has an option to enable interrupt. There are two types of interrupts.
1. Interrupt on Match - Triggers interrupt when there's a match happens with the compare register.
2. Interrupt on terminal count- Triggers interrupt when the PWM counter reaches its final value.

ASDK implements the interrupt service routine and callbacks a user function within the context of the interrupt service routine. The user must define the callback function by referring `asdk_pwm_callback_t` type and assign it in the configuration structure of the PWM during initialization.

> The callback has been implemented at channel level. User will get the callback event and the count at which the event occurred.

## Examples

### Controlling the duty cycle of an LED pin

The objective is to control the duty cycle of LED connected to a specific MCU pin, creating a breathing led effect. Let's break down the steps involved in implementing this example.

In this code the brightness of LED is gradually increased and decreased by adjusting the PWM duty cycle.

```c
#include "asdk_platform.h"
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_pwm.h"

#define MAX_DUTY_CYCLE 100
#define MIN_DUTY_CYCLE 0
#define DELAY_MS 10

#define PWM_PIN MCU_PIN_47            // Replace with your MCU PIN number
#define PWM_CH  ASDK_PWM_MODULE_CH_38 // Replace with your PWM Channel number

static asdk_pwm_config_t user_led_pwm_control_config = {
    .mcu_pin = PWM_PIN,                              // Pin for PWM 
    .pwm_mode = ASDK_PWM_MODE_PWM,                   // Use PWM mode only
    .pwm_run_mode = ASDK_PWM_RUN_MODE_CONTINUOUS,    // Output continuously
    .direction = ASDK_PWM_COUNT_DIRECTION_UP,        // Use up counter
    .interrupt = {.enable = false},                  // Disable PWM callbacks
    .pwm_output_align = ASDK_PWM_OUTPUT_LEFT_ALIGN,  // Left aligned
    .pwm_clock = {                                   // Use 1 Mhz clock for PWM module
        .clock_frequency = 1000000,
        .prescaler = ASDK_PWM_PRESCALER_DIVBY_1,
    },
    .pwm_frequency_in_Hz = 1000,                     // Desired PWM frequency
};

int main()
{
    /* Code to initialize PWM */
    
    /* Start PWM output */

    asdk_pwm_start(PWM_CH);

    while (1)
    {
        /* Increase brightness */

        for (int i = MIN_DUTY_CYCLE; i <= MAX_DUTY_CYCLE; i++) {
            user_led_pwm_control_config.pwm_duty_cycle_in_percent = i;

            // Update the duty cycle
            asdk_pwm_set_duty(PWM_CH, user_led_pwm_control_config.duty_cycle);
            ASDK_DELAY_MS(DELAY_MS);
        }

        /* Decrease brightness */

        for (int i = MAX_DUTY_CYCLE; i >= MIN_DUTY_CYCLE; i--) {
            user_led_pwm_control_config.duty_cycle = i;

            // Update the duty cycle
            asdk_pwm_set_duty(PWM_CH, user_led_pwm_control_config.duty_cycle);
            ASDK_DELAY_MS(DELAY_MS);
        }
    }

    return 0;
}

```