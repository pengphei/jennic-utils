About Jennic-ctrl
===================

This is the tool for programing flash of NXP Jennic Zigbee Chips.

Building
----------

this project is based on scons building tools. and this tool is based on libftdi.
So what we need to preinstall is as below:

* scons
* libftdi

and the building commands as below:

.. code-block:: sh
   
  scons

Usage
------------

.. code-block:: sh
    
  sudo ./build/jennic-ctrl -P pc -C ftdi -f ./helloworld.loftq-main.hex


