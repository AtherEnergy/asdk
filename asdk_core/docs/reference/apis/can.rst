=============================
Controller Area Network (CAN)
=============================

.. toctree::
   :maxdepth: 3

.. contents:: Overview
    :local:
    :backlinks: entry
    :depth: 2

Functions
=========

.. doxygengroup:: asdk_can_fun_group
   :project: ASDK
   :content-only:

Callback Functions
==================

.. doxygengroup:: asdk_can_cb_group
    :project: ASDK
    :content-only:


Data structures
===============

Enumerations
------------

.. doxygengroup:: asdk_can_enum_group
    :project: ASDK
    :content-only:

Structures
----------

.. doxygengroup:: asdk_can_ds_group
    :project: ASDK
    :members:
    :content-only:

Sequence Diagrams
=================

Initializing CAN and Transmitting a message
-------------------------------------------

.. raw:: html

    <div align="center">
    <img src="../../images/can/can_init_transmit_sd.png" width=640 style="padding:0 15px 0 15px;"/>
    <p>Initalize CAN and transmit a message</p>
    </div>

Read CAN message and Error events with interrupt (callback)
-----------------------------------------------------------

.. raw:: html

    <div align="center">
    <img src="../../images/can/can_receive_callback_sd.png" width=640 style="padding:0 15px 0 15px;"/>
    <p>Read the received CAN message in callback and handle error events</p>
    </div>
