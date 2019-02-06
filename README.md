# spinMAN
Spindle controller/manager for embedded CNC controllers, like GRBL, Smoothie, and Tiny G.

# I will be HAL'ifying this project shortly. it will be embedded into the Talos-CNC project as well. I hope to have this done before the end of February 2019. Again, I see cloners, so if anyone would like to help with these projects I would appreciate it.

The purpose of this project is to create a spindle managment solution for embedded systems. This project was designed to run on the Atmel 328 chip. (generic Arduino)

Primary principle of operation is to read an input value. (either via serial or analog input). When analog input is used the voltage will vary from 0-5 volts (PWM), depending on desired speed. If serial is used, a value indicating the RPM value of the spindle is sent. A 2 channel optical (90 degree offset) encoder should be utilized in your hardware, regardless of the control method used. These are available online for $10-$20. This is typically a quadrature encoder, but some refer to them as a 2channel encoder (More on this later)

When host is refered to, It refers either to a pc, raspberry pi, etc, or the grblRTC project. When controller or machine controller is refered to, it refers to grbl genericly. Although I believe it would work with any simple embedded soltuion, I only have a grbl controller for testing and development, and because msot other embedded solution copied grbl to being with, I feel like staying with something I am familiar with is teh easiest route.

The firmware reads the desired spindle speed value and outputs a PWM signal to the spindle driver control module. I have found 2 cheap and effective methods to do this. 
1. Use a brushed DC motor control module (available online for $10-$20). The controller should utilize a potentiometer that varies a drive signal from 0-5 volts. This can be directly controlled with the PWM output signal from the Atmel chip. this method works well for controlling a brushed DC servo or motor, which has a significant amount of low end torque. I have tested this method with rigid tapping in 6mm alluminum plate, using a M5 and M6 tap.
2. Use a brushless DC motor controller. The hobby type speed controllers will work, and are relativeley cheap, but to get maximum RPM out of the brushless you will want to run the motor near its maximum rated voltage. Generally this is 36v-50v, so you will need a high voltage hobby esc, which is difficult to find at a reasonable price. HOWEVER, an e-bike/scotter controller IS realtively low cost, typically runs 36v-48v and these can be driven by a 0-5 PWM signal on the 'throttle' input. These generally used a hall sensored motor, and it is easiest to start with a hall sensored motor. If you find the perfect motor and it is sensorless you can add hall sensors later. The sensors are essential for low end torque in uses like rigid tapping and slow spindle speeds on a heavy cut. 

The Basics (In my case the host is an arduino 2560 running grblRTC, but a pc or Pi could be used as well)
1. The host commands the spindle to start at a set RPM. 
2. The encoder reads 0RPM so acceleration is applied to the spindle.
3. After the spindle reaches the specified RPM the spindle responds to the host with an OK.
4. The host begins sending commands to the controller to cut/mill.
5. When the spindle speed drops below the specified RPM value, the PWM signal is increased to the spindle motor controller to keep the RPM steady. This works like a governor. 
6. A load readout is sent to the host to indicate how much load the spindle is under. This is a calculated value with 0% being when the spindle runs freely. As the load on the spindle increases and the PWM signal goes up the load will raise to 100%. If the spindle is at 100% load, this indicates that it is being driven as hard as it can. Driving it beyond 100% means the spindle motor cannot handle the load and RPM loss will occur.

Spindle synchronization
1. The host commands the spindle to synchronize with an axis. The axis can be X, Y, or Z. 
2. Since we are using a 2 channel optical encoder, we can determine the spindle direction by simply checking which signal line goes high first. If A is high first, then B, the rotation is CW. If B is high first, then A, the rotation is CCW.
3. There are 2 options from here:
<br>a. Using the grblRTC as the host, pulse values are sent from the spindle control to the host and the direction line is set high/low depending on spindle direction. If the spindle is moved at all the axis (X,Y,Z) will move at the specified rate per rotation, and in the corresponding direction of rotation. This pulse value is repeated directly to the stepper controller NOT through the machine controller. When spindle synchronization is turned off, the axis is returned to the location it began at before spindle synchronization was active.This keeping the machine controller in synch with where teh axis is acatully at.<br>b. Using a pc/pi as a host, when spindle synchronization is active, the angle and direction of the spindle movement is sent to the host. The host can then echo a jog command to the machine controller. As the spindle is rotated the axis will jog and move at a rate corresponding to the spindle rotation speed. It is not necessary to return the axis to its starting point since the machien controller will know the axis current location.

When using spindle synchronization it is VERY VERY important, that you realize the spindle may rotate at a speed that the linear axis cannot keep up with. If you use synchronization for rigid tapping, running the spindle at 6000 RPM may require the Z axis to move at 200ipm or more. If the linear axis cannot move that fast the machine will bind 2 axis's together.
