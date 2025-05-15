===================================
General Purpose Input/Output (GPIO)
===================================

.. toctree::
   :maxdepth: 3

.. contents:: Overview
    :local:
    :backlinks: entry
    :depth: 2

Functions
=========

.. doxygengroup:: asdk_gpio_fun_group
   :project: ASDK
   :content-only:

Callback Functions
==================

.. doxygengroup:: asdk_gpio_cb_group
    :project: ASDK
    :content-only:

Data structures
===============

Enumerations
------------

.. doxygengroup:: asdk_gpio_enum_group
    :project: ASDK
    :content-only:

Structures
----------

.. doxygengroup:: asdk_gpio_ds_group
    :project: ASDK
    :members:
    :content-only:


Sequence Diagrams
=================

Writing output pin state
------------------------

.. raw:: html

    <div align="center">
    <img src="../../images/gpio/gpo_sd.png" width=640 style="padding:0 15px 0 15px;"/>
    <p>GPIO as output pin.</p>
    </div>

Reading input pin state
-----------------------

.. raw:: html

    <div align="center">
    <img src="../../images/gpio/gpi_sd.png" width=640 style="padding:0 15px 0 15px;"/>
    <p>GPIO as input pin.</p>
    </div>

Reading input pin state with interrupt (callback)
-------------------------------------------------

.. raw:: html

    <div align="center">
    <img src="../../images/gpio/gpi_int_sd.png" width=640 style="padding:0 15px 0 15px;"/>
    <p>GPIO as input pin with interrupt handling.</p>
    </div>
