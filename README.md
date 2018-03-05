# spinMAN
Spindle controller/manager for embedded CNC controllers, like GRBL, Smoothie, and Tiny G.

The purpose of this project is to create a spindle managment solution for embedded systems. This project was designed to run on the Atmel 328 chip. (generic Arduino)

Primary principle of operation is to read an input value. (either via serial or analog input). When analog input is used the voltage will vary from 0-5 volts, depending on desired speed. If serial is used, a value indicating the RPM value of the spindle is sent. A 2 channel optical (90 degree offset) encoder should be utilized in your hardware, regardless of the control method used. These are available online for $10-$20. A quadrature encoder is not needed for this purpose. (More on this later)

The firmware reads the desired spindle speed value and outputs a PWM signal to the spindle driver control module. I have found 2 cheap and effective methods to do this. 
1. Use a brushed DC motor control module (available online for $10-$20). The controller should utilize a potentiometer that varies a drive signal from 0-5 volts. This can be directly controlled with the PWM output signal from the Atmel chip. this method works well fro controlling a DC servo, which has a significant amount of low end torque. I have tested this method with rigid tapping in 6mm alluminum plate, using a M5 and M6 tap.
2. Use a brushless DC motor controller. The hobby type speed controllers will work, and are relativeley cheap, but to get maximum RPM out of the brushless you will want to run the motor near its maximum rated voltage. Generally this is 36v-50v, so you will need a high voltage hobby esc, which is difficult to find ata  reasonable price. HOWEVER, an e-bike/scotter controller IS realtively low cost, typically runs 36v-48v
