# Controller Area Network (CAN)

This document serves as reference for understanding and working with CAN module. It explains all it's features, shows how to use interrupts and callbacks along with example and reference code-snippets.

A Controller Area Network (CAN) is a robust, efficient communication system that is widely used in automotive applications. It's a message-based protocol that allows microcontrollers and devices to communicate with each other.

In a CAN network, multiple nodes (devices or microcontrollers) are connected to a common communication line (the CAN bus). Each node is capable of both sending and receiving messages. The messages are not sent to a specific node, but rather, every node on the network sees every message. Each node then decides whether to ignore the message or take action based on the message's identifier.

Let's consider a scenario in an electric vehicle where the CAN network is used:

<div align="center">
<img src="../../images/can/can_network.png" width=400/>
<p>Simplified CAN network</p>
</div>

1. Dashboard (Node A): This node displays information to the driver, such as speed, battery level, light status, and error messages.
2. Vehicle Control Unit (Node B): This node controls various aspects of the vehicle's operation, such as interpreting driver inputs and managing the vehicle's systems.
3. Motor Controller (Node C): This node controls the operation of the vehicle's electric motor based on commands from the Vehicle Control Unit.

When you flip the light switch, Node B (Vehicle Control Unit) senses this input and sends a message onto the CAN bus with an identifier that signifies "Light Switch Flipped" and data representing the new state of the light switch (on or off).

All nodes on the network see this message. Node A (Dashboard) might respond by updating the light indicator on the dashboard. Node C (Motor Controller) might not take any action in this case, as the state of the light switch does not directly affect the operation of the motor.

Configuring the CAN peripherals varies from one microcontroller to another, not only in terms of configurations settings but also in terms of features can capabilities of the hardware that are supported.

<div align="center">
<img src="../../images/can/can_peripheral.png" width=640/>
<p>CAN peripheral module representing configuration</p>
</div>

ASDK simplifies configuring CAN peripheral by grouping configuration in two data structures.

1. Controller configuration - mandatory. It includes settings for operating mode (Classic CAN or CAN-FD), Maximum Data Length Code (DLC) and bitrate configuration.
2. Filtering configuration - optional. It includes hardware filter settings for reception of CAN messages, Rx FIFO acceptance settings and dedicated mailbox for transmission and reception settings.

## Features

### Transmission status check

The transmission status checking feature, implemented through the `asdk_can_is_tx_busy` API, provides a way to check the status of a given mailbox in the CAN controller.

A mailbox is a buffer that holds a CAN message for transmission or has received a CAN message. Each mailbox can hold one CAN message at a time. When a message is placed in a transmit mailbox, the CAN controller attempts to transmit it on the CAN bus. If the bus is busy due to other nodes transmitting, the controller waits until the bus is free. During this time, the mailbox is considered "busy". For example, if a mailbox is busy, you might choose to wait until the mailbox is free or you might place the new message in a different mailbox.

### Hardware Filtering

    This feature relies on the underlying hardware support of the microcontroller.

The CAN filtering feature is implemented through the `asdk_can_hw_filter_t` data structure, provides a way to specify which CAN messages should be accepted by the CAN controller.

Filtering is typically done based on the identifier of the CAN messages. The controller compares the identifier of each incoming message with the identifiers specified in the filter. If the identifier matches one of the filter identifiers, the message is accepted and stored in a receive mailbox. If it doesn't match, the message is ignored.

### Sleep mode

    This feature relies on the underlying hardware support of the microcontroller.

The sleep feature is implemented through the `asdk_can_sleep` API, provides a way to put the CAN peripheral into a low-power mode when it's not in use.

Sleep mode is a state where the CAN controller is not actively participating in communication on the CAN bus. It doesn't transmit any messages and it doesn't acknowledge any messages from other nodes. However, it does monitor the bus, and it can wake up and start participating in communication again if it detects activity.

### Best Pre-scaler and Bit time settings

    This feature relies on the underlying hardware support of the microcontroller.

The user does not have to configure the clock pre-scaler value because ASDK presumes the best clock pre-scaler value to ensure that the total time quanta per bit time remains constant. This feature is again subjected to the hardware support of the microcontroller. So please refer to the corresponding [platform user guide](../../reference/platforms/) to find out total quanta value based on the desired baudrate.

Bit time settings helps for compensating delays and phase errors on the CAN bus. For more information refer please refer this [article](https://www.can-cia.org/fileadmin/resources/documents/proceedings/2012_taralkar.pdf).

The sampling point, refers to the time at which the bus level is read and interpreted as a '0' or a '1'. This is done during the bit time. It needs to be at a point where the signal is stable and not in a transition phase. It's usually set somewhere around  75%-80% the middle of the bit time to allow for propagation delays and phase shifts.

> Sampling Point (%) = (1 + Propagation segment + Phase segment 1) / (1 + Propagation + Phase segment 1 + Phase segment 2) * 100%

For example, on `cyt2b75_m0plus` and `cyt2b75_m4` platform  the total time quanta is 40. Therefore for 87.5% sampling point the bit time settings is as follows.
1. Phase segment1 = 5
2. Phase segment2 = 5
3. Propagation segment = 29

## CAN Interrupt and callbacks

CAN interrupts indicate that a certain event has occurred. This could be the reception of a new message, the completion of a message transmission, the detection of an error, etc. To detect and handle the events the user has to enable interrupts.

ASDK implements the interrupt service routine and callbacks a user function within the context of the interrupt service routine. The user must define the callback function by referring `asdk_gpio_input_callback_t` type and assign it by calling `asdk_can_callback_t` function.

## Examples

### Initializing CAN

```c
#include "asdk_platform.h"

#define CAN_TX 4
#define CAN_RX 5

asdk_can_config_t can_cfg = {
    .mcu_pins = {
        .tx_pin = CAN_TX,
        .rx_pin = CAN_RX
    },
    .controller_settings = {
        .mode = ASDK_CAN_MODE_STANDARD,
        .max_dlc = ASDK_CAN_DLC_8,
        .bitrate_config.can = {
            .baudrate = ASDK_CAN_BAUDRATE_500K,
            // total tq = 40, sampling point = 87.5%
            .bit_time = {
                .sync_jump_width = 5,
                .phase_segment1 = 5,
                .phase_segment2 = 5,
                .prop_segment = 29,
            },
        },
        .can_id_type = ASDK_CAN_ID_STANDARD,
        .interrupt_config = {
            .use_interrupt = true,
            .priority = 3,
        },
    },
};

asdk_can_init(ASDK_CAN_MODULE_CAN_CH_1, &can_cfg);
```

### Transmitting a message

```c
// Create a message structure
asdk_can_message_t tx_msg;
uint8_t tx_buffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};

// Set the message parameters
tx_msg.can_id = 0x123;      // CAN ID
tx_msg.dlc = 2;             // length of payload
tx_msg.message = tx_buffer;   // payload

// Transmit the message
if (asdk_can_write(ASDK_CAN_CHANNEL_0, 2, &message) != ASDK_CAN_OK) {
    // Handle error
}
```

### Receiving a message

```c
uint8_t rx_buffer[8];            // buffer to hold the received payload
bool message_received = false; // flag to indicate if new message is received

asdk_can_message_t rx_msg = {  // assign the buffer to store the received message
    .message = rx_buffer;
};

void can_app_callback(uint8_t can_ch, asdk_can_event_t event, asdk_can_message_t *can_message)
{
    if (event == ASDK_CAN_RX_EVENT)
    {
        message_received = true;

        // read the message and store in rx_msg
        rx_msg.can_id = can_message->can_id;
        rx_msg.dlc = can_message->dlc;
        memcpy(&rx_msg.message, can_message->message, can_message->dlc);
    }
}

int main()
{
    /* ... */
    
    /* code to initialize CAN */

    asdk_can_install_callback(&can_app_callback);

    while(1)
    {
        if (message_received)
        {
            message_received = false;

            /* process the received message with rx_msg.message */

            // rx_msg.can_id  - CAN ID of received message
            // rx_msg.dlc     - length of received message
            // rx_msg.message - the message, an array
        }
    }
}
```
