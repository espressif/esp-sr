Model Selection and Loading
===========================

:link_to_translation:`zh_CN:[中文]`

This document explains how to select and load models for ESP-SR.

Model Selection
---------------

ESP-SR allows you to choose required models through the ``menuconfig`` interface. To configure models:

1. Run ``idf.py menuconfig``
2. Navigate to **ESP Speech Recognition**
3. Configure the following options:
   - **Noise Suppression Model**
   - **VAD Model**
   - **WakeNet Model**
   - **MultiNet Model**

.. figure:: ../../_static/kconfig.png
    :alt: kconfig


Updating Partition Table
------------------------
You must add a `partition.csv` file and ensure that there is enough space for the selected models. 
Add the following line to your project's ``partitions.csv`` file to allocate space for models:

.. code-block::

    model,  data,        ,         ,    6000K

- Replace ``6000K`` with your custom partition size according to the selected models.
- ``model`` is the partition label (fixed value).

Model Loading
-------------

ESP-IDF Framework
~~~~~~~~~~~~~~~~~

ESP-SR automatically handles model loading through its CMake scripts:  

1. Flash the device with all components:  
   ``idf.py flash``  
   *This command automatically loads the selected models.*

2. For code debugging (without re-flashing models):  
   ``idf.py app-flash``  

.. note::  
   The model loading script is defined in ``esp-sr/CMakeLists.txt``. Models are flashed to the partition labeled ``model`` during initial flashing.

Arduino Framework
~~~~~~~~~~~~~~~~~

To manually generate and load models:  

1. Use the provided Python script to generate ``srmodels.bin``:

   .. code-block:: bash

      python {esp-sr_path}/movemodel.py -d1 {sdkconfig_path} -d2 {esp-sr_path} -d3 {build_path}

   **Parameters:**

   - ``esp-sr_path``: Path to your ESP-SR component directory

   - ``sdkconfig_path``: Project's ``sdkconfig`` file path

   - ``build_path``: Project's build directory (typically ``your_project_path/build``)

2. The generated ``srmodels.bin`` will be located at:  
   ``{build_path}/srmodels/srmodels.bin``

3. Flash the generated binary to your device.

.. important::  
   Just regenerate ``srmodels.bin`` after changing model configurations in ``menuconfig``.