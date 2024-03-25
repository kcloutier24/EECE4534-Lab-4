# Lab 4: GPIO Mirror Kernel Module



# GPIO Mirror Kernel Module Polling
We will explore aspects of a kernel module, by implementing a GPIO Mirroring module.

## Project description

This module will perform the following tasks:

- Setup the LED and Switch GPIOs
- Poll the state of the switches
- Mirror the current state of the Switches using the LEDs through polling


## Make instructions

- Type make into a Linux command line
- Transfer the file (gmirror.ko) over to the Zedboard via SCP


## Usage

- Use `insmod gmirror.ko led_gpio_base=881 switch_gpio_base=893` to insert the module with the correct chip bases
- To get the chip values go to `/sys/class/gpio` after entering the board or QEMU and use `ls -l` to see symbolic links to the devices reveal the base address of each gpio chip.
- Use `dmesg` to see the messages printed in the main window and to check that everything is working as intended
- Flip the switches to see the LEDs work

## Visuals


![Beautiful](/pulsecap/hrtimer_poll.png)

Above is a pulsecap visual received 

## Observations


- It is noted to get a better graph use an oscilloscope and probe it physically as probing is not possible
- With each switch triggered the corresponding LED is lit up 
- The responsiveness of this implementation is instantaneous and does change immediately to the human eye
- The reason for this is due to the timer polling faster than the human eye can see
- We did not make it faster than 1 ms due to the observation that it was already instantaneous



# GPIO Mirror with Interrupts


## Project description
This module will perform the following tasks:

- Mirror the current state of the Switches using the LEDs via interrupts
- The interrupt solutions are available in the `interrupt` branch
- The interrupt obtains the interrupt number because it is in the device tree, which is given when the interrupt is allocated 
- 


## Behvaior and observations

- With each switch triggered the corresponding LED is lit up 
- The responsiveness of this implementation is instantaneous and does change immediately
- Upon triggering an interrupt it immediately prints that an interrupt has been called and the LED state is changed











# Design Discussion

In #2 and #3, you have implemented equivalent functionality with two different methods. 

- What is the difference in impact to the overall system load between the two modes of implementation? 

With the interrupt, the load on the system is higher because it takes lower priority when it is called, rather than the polling which is a higher priority. The interruption also impacts the timing and causes a slower time


- Compare the impact on performance as you as a developer, try to increase the overall responsiveness of interactive user inputs/outputs in an embedded system.


The overall responsiveness can be increased by changing the 1ms timer period. Upon putting the board through a stress test the switches and LEDs demonstrated a visual instantaneous ratio of on/lit to off/unlit. It was observed that upon increasing the load on the system the delay was more noticeable for the timer but not the interrupt.













