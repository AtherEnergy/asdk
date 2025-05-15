# ADC

This document serves as reference for understanding and working with the ADC module. It explains all it's features, shows how to use interrupts and callbacks along with example and reference code-snippets.

ADC, or Analog-to-Digital Converter, is a crucial peripheral in a microcontroller unit (MCU). It serves as a bridge between the analog world and the digital world that the MCU understands.

In the real world, many data we encounter are analog, like temperature, pressure, sound, light intensity, etc. These data are continuous and can have any value within a certain range. However, an MCU operates in the digital realm, where data is represented as 0s and 1s. This is where the ADC comes in. It converts these analog signals into digital values that the MCU can process. For example, if we have a temperature sensor that gives out a voltage proportional to the temperature, the ADC can convert this voltage (analog data) into a digital value that the MCU can use to determine the actual temperature.

A typical block diagram of ADC(Analog to Digital Converter) is shown below:

<div align="center">
<img src="../../images/adc/adc_block_diag.png" width=640/>
<p>The above image represents ADC block diagram</p>
</div>

The key components of ADC peripheral are explained below:
1. **Analog Input** : This is the analog signal that you want to convert into a digital value. It could come from various sources like a sensor, a potentiometer, etc.
2. **Sample and Hold Circuit** : This circuit takes a 'snapshot' of the analog input at a specific point in time and holds this value constant for a certain period. This is necessary because the conversion process takes a finite amount of time, during which the input signal must remain stable.
3. **Analog-to-Digital Converter Core** : This is the heart of the ADC. It converts the held analog value into a digital value. There are several methods to do this, such as successive approximation, sigma-delta, etc. The method used depends on the specific ADC.
4. **Clock** : This provides the timing for the ADC. The frequency of the clock determines the maximum sampling rate of the ADC.
The actual components and their arrangement might vary depending on the specific ADC module or microcontroller. 


## How ADC works?

An Analog-to-Digital Converter (ADC) works by converting continuous analog signals into discrete digital values that a microcontroller unit (MCU) can process. Here's a simplified explanation of the process:

1. **Sampling** :The ADC starts by taking a 'snapshot' of the analog signal at a specific point in time. This is known as sampling. The rate at which the ADC samples the analog signal is known as the sampling rate.
2. **Quantization** :After sampling the signal, the ADC then quantizes the sampled value. Quantization is the process of mapping the sampled analog values to a set of discrete digital values. The number of possible digital values depends on the resolution of the ADC. For example, an 8-bit ADC can map the sampled values to one of 256 possible digital values (2^8 = 256).
3. **Encoding** :The final step is encoding, where the quantized value is converted into a binary format that the MCU can understand.

This process repeats at the sampling rate, allowing the ADC to convert a continuous analog signal into a stream of digital values. The accuracy of the conversion depends on both the sampling rate and the resolution of the ADC. A higher sampling rate can capture more detail of the original analog signal, while a higher resolution can represent the sampled values more accurately.

## ADC Jargons

- **ADC Resolution** :The resolution of an ADC refers to the number of discrete values it can produce over the range of analog values. The resolution is usually expressed in bits. For example, an ADC with a resolution of 8 bits can encode an analog input to one in 256 different levels, since 2^8 = 256. The values range from 0 to 255. If the analog input is a voltage, then each level represents about 19.53 millivolts in a 5-volt range.

    The resolution determines the smallest change in voltage that the ADC can detect. A higher resolution means a smaller minimum voltage change can be detected, and thus more precise measurements can be made.

- **ADC Conversion Time** :The conversion time of an ADC is the amount of time it takes to convert an analog input into a digital output. This time includes the sampling time (the time it takes to take a snapshot of the analog signal) and the quantization time (the time it takes to convert the sampled analog value into a digital value).

    The conversion time is inversely proportional to the sampling rate, which is the number of samples taken per second. A higher sampling rate means a shorter conversion time, but it also requires more processing power and storage capacity.

    The conversion time also depends on the resolution of the ADC. Higher resolution ADCs require longer conversion times because they have to perform more calculations to determine the precise digital value.

In asdk for the ADC module the configuration structure is divided into two parts:
* **Essential Input** :In this case the ```asdk_adc_init()``` requires only three parameters to be initialized as input in the configuration structure as shown in the below code snippet:
    ```c
    asdk_mcu_pin_t adc_pin[] = {MCU_PIN_45, MCU_PIN_49, MCU_PIN_52};

    uint32_t adc_data[3];

    asdk_adc_config_t adc_conf = {
        .pin_count = 3,         // 3 pins are used as ADC
        .pin_nums = adc_pin,    // An array representing those 3 pins
        .enable_interrupt = true,
    };

    asdk_adc_init(&adc_conf);
    ```
    Other parameters of the ```asdk_adc_config_t``` configuration structure is given default values which can be overwritten by the user if they want the adc to run as per their requirement.

* **Other Inputs** :Other inputs of the ADC can be used by the user if they need to run the ADC as per their requirement.
Below is a code snippet to use a pin as ADC with all inputs being customized as per requirement:

    ```c
    asdk_adc_config_t adc_conf = {
        .pin_count = 3,                                             // 3 pins are used as ADC
        .pin_nums = adc_pin,                                        // An array representing those 3 pins
        .enable_interrupt = true,                                   // Enable callback on completing conversion
        .adc_resolution = ASDK_ADC_RESOLUTION_12_BITS,              // 12-bit resolution of ADC
        .reference_voltage_src = ASDK_ADC_REF_VOLTAGE_SRC_EXTERNAL, // Use external voltage reference
        .adc_sampling_freq = ASDK_ADC_SAMPLING_FREQ_250KHz,         // Sampling frequency
        .enable_adc_calibration = true,                             // Calibration enabled
        .interrupt_priority = 8,
    };

    asdk_adc_init(&adc_conf);
    ```

## Features

Following set of features are made available to the user.User can use these features subject to the condition that hardware supports those features.

### ADC Resolution
The resolution of an ADC refers to the number of discrete digital values that it can produce. It is usually expressed in bits. For example, an 8-bit ADC can produce 256 (2^8) different values. In ASDK, the user can set the desired resolution of the ADC Conversion considering the hardware supports that using the configuration structure during initialization. Refer `adc_resolution` in the configuration data structure.

### Sampling Rate
This is the speed at which the ADC can convert an analog signal into a digital value. It is usually expressed in samples per second (SPS) or Hertz (Hz). In ASDK, the user can set the desired sampling rate of the ADC Conversion considering the hardware supports that using the configuration structure during initialization. Refer `adc_sampling_freq` in the configuration data structure.

### Reference Voltage
The reference voltage of an ADC is voltage ADC uses as reference to measure the analog input voltage. ADC can measure at maximum voltage of reference voltage. Any voltage above the reference voltage will be read as the maximum digital value. In ASDK, the user can set the reference voltage as internal or external by using the configuration structure of the ADC. Refer `reference_voltage_src` in the configuration data structure.

## ADC  Interrupt and Callbacks

An ADC (Analog-to-Digital Converter) interrupt is a feature that allows the ADC to notify the CPU when it has finished converting an analog input to a digital value. This is especially useful in scenarios where the CPU doesn't need to constantly check if the ADC conversion is complete, freeing up the CPU to perform other tasks. Refer `enable_interrupt` in the configuration data structure.

ASDK implements the interrupt service routine and callbacks a user function within the context of the interrupt service routine. The user must define the callback function by referring `asdk_adc_callback_t` type and assign it by calling `asdk_adc_install_callback` function.

> The callback will come when a channel completes conversion.The user can then use the callback function to check which channel has finished conversion which can be used for reading the conversion result.

## Examples

### ADC 

Let's implement one example wherein we'll use three pins as ADC and store the result.Through this example user can configure three pins to be used as ADC which can be used to measure  three different analog voltages might be temperature, current, voltage etc..


```c
#include "asdk_platform.h"

/* ASDK app includes */
#include "asdk_clock.h"
#include "asdk_gpio.h"
#include "asdk_adc.h"

/* Conversion status flag */
volatile bool adc_complete = false;

/* Error code for the APIs */
volatile asdk_errorcode_t adc_error = ASDK_SUCCESS;

/* Array of pin numbers to be used as ADC pins */
asdk_mcu_pin_t adc_pin[] = {MCU_PIN_45, MCU_PIN_49, MCU_PIN_52};

/* Buffer to store the result of conversion */
uint32_t adc_data[3];

int main()
{
    /* ... */

    /* code to initialize ADC */

    /* Install the callback */

    error_code = asdk_adc_install_callback(adc_callback);
    if (ASDK_SUCCESS != error_code)
    {
        adc_error = error_code;
        return error_code;
    }

    for (;;)
    {
        /* start ADC conversion for pin 45 */

        adc_complete = false;
        asdk_adc_start_conversion_non_blocking(MCU_PIN_45);
        // wait until conversion is complete
        while (true != adc_complete);
        // read the converted value
        asdk_adc_read_conversion_value_non_blocking(MCU_PIN_45, &adc_data[0]);

        /* start ADC conversion for pin 49 */

        adc_complete = false;
        asdk_adc_start_conversion_non_blocking(MCU_PIN_49);
        while (true != adc_complete);
        asdk_adc_read_conversion_value_non_blocking(MCU_PIN_49, &adc_data[1]);

        /* start ADC conversion for pin 52 */

        adc_complete = false;
        asdk_adc_start_conversion_non_blocking(MCU_PIN_52);
        while (true != adc_complete);
        asdk_adc_read_conversion_value_non_blocking(MCU_PIN_52, &adc_data[2]);
    }

    return 0;
}


/*Callback function will be called when a channel conversion is complete*/
void adc_callback(asdk_adc_callback_t params)
{
    adc_complete = true;
}


```

