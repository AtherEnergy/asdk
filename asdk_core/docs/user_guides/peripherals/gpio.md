# GPIO

This document serves as reference for understanding and working with GPIO module. It explains all it's features, shows how to use interrupts and callbacks along with example and reference code-snippets.

To understand the purpose of GPIO let's imagine that a user wants to control a light bulb with a switch which are not directly connected with each other but are meant to be connected by a software running on a microcontroller. To accomplish this connection the user must know how to use GPIO. The examples section of this document covers how to achieve this task.

<div align="center">
<img src="../../images/gpio/gpio_pin.png" width=200/>
<p>The above image represents single GPIO pin.</p>
</div>

A pin of the microcontroller can be used for "on" and "off" control. Where the state of the pin is changed by the user as desired. This type of control is known as General Purpose Output (GPO). Similarly a pin can be used to read the status of a button to know whether a button is pressed or released. This is input control, where the change in state of the pin must be read by the user. This type of control is known as General Purpose Input (GPI).

The GPIO module abstracts both the General Purpose Input and Output (GPIO) functionality. A microcontroller has several pins that may function as GPIO but there are cases where a pin cannot be used as GPIO. ASDK is aware about such pins. So when a user decides to use such pin, API returns an error code stating that such pin is invalid for GPIO functionality.

## GPIO as output

At the hardware level, the GPIO module contains special circuitry for output control which gets enabled on configuring the pin for output mode. The below image represents simplified version of the circuit.

<div align="center">
<img src="../../images/gpio/gpo.png" width=250 style="padding:0 15px 0 15px;"/>
<p>GPIO as output pin.</p>
</div>

A pin can be in one of the two states High (1) or Low (0). To set the output of the pin to High state use `asdk_gpio_output_set` function. Internally, the switch s1 closes & s2 remains open. Similarly to set the output to Low state use `asdk_gpio_output_clear` function. Internally, the switch s2 closes & s1 remains open.

<div align="center">
    <table>
        <tr>
            <td>
                <div align="center">
                <img src="../../images/gpio/gpo_high.png" width=250 style="padding:0 15px 0 15px;"/>
                <p>Pin state is high.</p>
                </div>
            </td>
            <td>
                <div align="center">
                <img src="../../images/gpio/gpo_low.png" width=250 style="padding:0 15px 0 15px;"/>
                <p>Pin state is low.</p>
                </div>
            </td>
        </tr>
    </table>
</div>

Below is a code snippet to configure a pin as GPO.

```c
asdk_gpio_config_t output_cfg = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
};

asdk_gpio_init(&output_cfg);
```

## GPIO as input

At the hardware level, the GPIO peripheral contains special circuitry for input control which gets enabled on configuring the pin for input mode. The below image represents simplified version of the circuit.

<div align="center">
<img src="../../images/gpio/gpi.png" width=250 style="padding:0 15px 0 15px;"/>
<p>GPIO as input pin.</p>
</div>

The `gpio_pull` parameter in the configuration sets pin state for input mode. Use `ASDK_GPIO_PULL_UP` to enable the pull-up resistor (Rup) path for High state, or use `ASDK_GPIO_PULL_DOWN` to enable pull-down resistor (Rdown) path for Low state. The image below shows pull-up configuration.

<div align="center">
    <table>
        <tr>
            <td>
                <div align="center">
                <img src="../../images/gpio/gpi_released.png" width=300 style="padding:0 15px 0 15px;"/>
                <p>Pin state is read as high.</p>
                </div>
            </td>
            <td>
                <div align="center">
                <img src="../../images/gpio/gpi_pressed.png" width=300 style="padding:0 15px 0 15px;"/>
                <p>Pin state is read as low.</p>
                </div>
            </td>
        </tr>
    </table>
</div>

The button has two pins, one is connected to the GPI pin and the other to the ground. By default, the path to ground is open so pin state remains High. Pressing the button closes the path to ground which cause transition in pin state from High to Low.

Below is a code snippet representing pull-up configuration.

```c
asdk_gpio_config_t input_cfg = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_pull = ASDK_GPIO_PULL_UP,
};

asdk_gpio_init(&input_cfg);
asdk_gpio_state_t current_state = asdk_gpio_get_input_state(input_cfg.mcu_pin); // gives ASDK_GPIO_STATE_HIGH
```

## Features

Following set of features are made available to the user.

### Set the output state while initializing pin

Let's consider a case where a user wants to set the initial state on the output pin. To achieve this the user first configures the pin as GPO. Then an output state is assigned using one of the output functions. Below code snippet that is most commonly used where the initial state of the pin set to Low.

```c
asdk_gpio_config_t output_cfg = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
};

asdk_gpio_init(&output_cfg);
asdk_gpio_clear(output_cfg.mcu_pin);
```

It is a two step process where two function calls are required. The GPIO module allows the user to assign the initial state of the GPO pin while initializing the pin. To set the desired initial output state use the `init_state` parameter of the configuration structure. Below is an code snippet that uses this feature to yield same results as the above code snippet.

```c
asdk_gpio_config_t output_cfg = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .init_state = ASDK_GPIO_STATE_LOW,
};

asdk_gpio_init(&output_cfg);
```

### Slew rate (speed) control

    This feature relies on the underlying hardware support of the microcontroller.

Slew rate control generally refers to the rate at which the output changes with time. When using GPO, slew rate refers to how fast the pin can switch from one state to another. There are two types of slew control High frequency and Low frequency. The user can select one of them while initializing the pin. The code snippet below uses High frequency option.

```c
asdk_gpio_config_t output_cfg = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .gpio_speed = ASDK_GPIO_SPEED_HIGH_FREQ,
};

asdk_gpio_init(&output_cfg);
asdk_gpio_clear(output_cfg.mcu_pin);
```

> Note: If the underlying hardware does not support this feature then this configuration can be safely ignored as it doesn't have any effect.

## GPIO Interrupt and Callbacks

A GPIO interrupt is commonly used to detect a state change on the GPI pin. Normally, the user sets the known state on the GPI pin by either enabling pull-up or pull-down and then detects the change in state by reading the pin using `asdk_gpio_get_input_state` function. Instead, enable GPIO interrupt to detect the change in state with callback function.

Each pin when configured as GPI has an option to enable interrupt. There are three types of interrupts.
1. Rising edge - Triggers interrupt on detecting Low to High transition.
2. Falling edge - Triggers interrupt on detecting High to Low transition.
3. Both edges - Triggers interrupt for both the transitions.

ASDK implements the interrupt service routine and callbacks a user function within the context of the interrupt service routine. The user must define the callback function by referring `asdk_gpio_input_callback_t` type and assign it by calling `asdk_gpio_install_callback` function.

> The callback has been implemented as a module level. Therefore only one callback function is used for all the GPIO pins.

Below is a code snippet to enable interrupt and using callback function.

```c
asdk_gpio_config_t input_cfg_w_interrupt = {
    .mcu_pin = 10,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_pull = ASDK_GPIO_PULL_UP,
    .interrupt_config = {
        .type = ASDK_GPIO_INTERRUPT_FALLING_EDGE,
        .priority = 3,
    },
};

// callback function
void user_gpio_callback(asdk_mcu_pin_t mcu_pin, uint32_t pin_state)
{
    // state changed from High to Low
    if (mcu_pin == 10) && (pin_state == ASDK_GPIO_STATE_LOW)
    {
        // button pressed
    }
}

asdk_gpio_init(&input_cfg_w_interrupt);
asdk_gpio_install_callback(&user_gpio_callback);
```

## Examples

### Blinky

The objective is to toggle the state of an LED connected to a specific MCU pin at regular intervals, creating a blinking effect.

```c
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"

#define DELAY_250_MS 250

static asdk_gpio_config_t user_led_1 =
{
    .mcu_pin = MCU_PIN_77,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .gpio_init_state = ASDK_GPIO_STATE_HIGH,
    .gpio_pull = ASDK_GPIO_PUSH_PULL
};

int main()
{
    /* ... */

    asdk_gpio_init(&user_led_1);

    for (;;)
    {
        asdk_gpio_output_set(user_led_1.mcu_pin);
        ASDK_DELAY_MS(DELAY_250_MS);

        asdk_gpio_output_clear(user_led_1.mcu_pin);
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}
```


### Button detection

#### Polling method

This example showcases how to configure an MCU pin as an input, read its state in continuous loop, and control an LED based on the button's status.

```c
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"

#define DELAY_1_S 1000

static asdk_gpio_config_t user_led_1 =
{
    .mcu_pin = MCU_PIN_77,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .gpio_init_state = ASDK_GPIO_STATE_HIGH,
    .gpio_pull = ASDK_GPIO_PUSH_PULL
};

static asdk_gpio_config_t user_button_0 =
{
    .mcu_pin = MCU_PIN_29,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_init_state = ASDK_GPIO_STATE_HIGH,
    .gpio_pull = ASDK_GPIO_PULL_UP
};

int main()
{
    uint8_t input_state = 0;    // variable to store the state of the mcu pin

    /* ... */

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);

    // Initialising the gpio for input via button
    asdk_gpio_init(&user_button_0);

    for (;;)
    {
        // Get the state of the input mcu pin
        asdk_gpio_get_input_state(user_button_0.mcu_pin, &input_state);

        // The default state of button is logic 1 (HIGH), hence the gpio is cleared (turned off)
        if(input_state)
        {
            asdk_gpio_output_clear(user_led_1.mcu_pin);
            
        }
        else    // when led is pressed, input state is logic 0 (LOW), hence the gpio is set (turned on) 
        {
            asdk_gpio_output_set(user_led_1.mcu_pin);
        }
        ASDK_DELAY_MS(DELAY_1_S);
    }

    return 0;
}

```

#### Interrupt method

This example illustrates how to configure an MCU pin as an input and utilise interrupts to respond to specific events, such as a falling edge. By employing interrupts, your MCU application can efficiently handle button presses and trigger corresponding actions, enhancing responsiveness and reducing the need for continuous polling.

```c
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_system.h"
#include "asdk_clock.h"
#include "asdk_gpio.h"

#define DELAY_250_MS 250

static asdk_gpio_config_t user_led_1 =
{
    .mcu_pin = MCU_PIN_77,
    .gpio_mode = ASDK_GPIO_MODE_OUTPUT,
    .gpio_init_state = ASDK_GPIO_STATE_HIGH,
    .gpio_pull = ASDK_GPIO_PUSH_PULL
};

static asdk_gpio_config_t user_button_0 =
{
    .mcu_pin = MCU_PIN_29,
    .gpio_mode = ASDK_GPIO_MODE_INPUT,
    .gpio_pull = ASDK_GPIO_PULL_UP,
    .interrupt_config = {
        .type = ASDK_GPIO_INTERRUPT_FALLING_EDGE,
        .priority = 1
    }
};

void user_button_interrupt_callback(asdk_mcu_pin_t mcu_pin, uint32_t param)
{
    // 
    // All GPIO interrupts have a single callback.
    // Hence it is essential to check the gpio which caused this callback and
    // take necessary action for that interrupt.
    // 
    if(mcu_pin == user_button_0.mcu_pin)
    {
        // every time a falling edge is detected on user_button_0, the led pin is toggled
        asdk_gpio_output_toggle(user_led_1.mcu_pin);    
    }
}

int main()
{
    /* ... */

    // Initialising the gpio for led toggling
    asdk_gpio_init(&user_led_1);

    // Initialising the gpio for input via button
    asdk_gpio_init(&user_button_0);

    // Assigning callback function for all gpio interrupts
    asdk_gpio_assign_callback(&user_button_interrupt_callback);

    for (;;)
    {
        // Simulation of some process occurring, since we have nothing else to do
        ASDK_DELAY_MS(DELAY_250_MS);
    }

    return 0;
}

```