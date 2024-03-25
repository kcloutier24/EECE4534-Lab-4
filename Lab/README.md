

# See the lab instructions [LabInstructions.md](LabInstructions.md) 



## Design Discussion

In #2 and #3, you have implemented equivalent functionality with two different methods. 

- What is the difference in impact to the overall system load between the two modes of implementation? 

With the interrupt, the load on the system is higher because it takes lower priority when it is called, rather than the polling which is a higher priority. The interruption also impacts the timing and causes a slower time


- Compare the impact on performance as you as a developer, try to increase the overall responsiveness of interactive user inputs/outputs in an embedded system.


The overall responsiveness can be increased by changing the 1ms timer period. Upon putting the board through a stress test the switches and LEDs demonstrated a visual instantaneous ratio of on/lit to off/unlit. It was observed that upon increasing the load on the system the delay was more noticeable for the timer but not the interrupt.



