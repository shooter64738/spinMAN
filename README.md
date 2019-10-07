# spinMAN
Spindle controller/manager for embedded CNC controllers, like GRBL, Smoothie, and Tiny G.

This project has been COMPLETLEY re-written. It currently runs on the Atmel 328P chip.
You may also open and run it in MS Visual Studio in simulation mode.
I will be adding the Atmel SAMD21 ARM chip support in the near future.

## Principles of operation:
### Velocity Mode
1. Your CNC controler (whatever it may be) uses a PWM signal to command a spindle speed
2. Instead of using that PWM signal directly to drive the spindle, spinMAN uses that pwm
signal to determine a speed. If the input signal is 1hz, thats 1rpm. 100hz is 100 rpm. 
spinMAN can reliably determine frequency up to 2mhz, which would be a speed request of 
2,000,000rpm. Pretty rediculous, but its there.
3. Most motion controllers do not issue a PWM signal in this fashion, but controls like 
grbl have enough flexibility that you can change the source code to calculate the required
frequency for a given 'S' command without making extensive changes or additions to the 
motion controllers core.

### Position Mode
1. spinMAN operates like a basic servo system.
2. Pulse inputs are counted and accumualted. If the pulse count is not zero spinMAN attempts
to drive the motor to move to that pulse count. The pusle count for that sample are cleared
and the count is read again.
3. *If your motion controller can determine the number of pulses needed to get the spindle to
a specific position, jsut send that count fo pulses to the input pin and the spindle will rotate
that far.


### Torque Mode
1. spinMAN operates like a basic spindle drive.
2. **The current (amps) load is determiend from the motor driver and a torque output is calculated.
spinMAN will attempt to hold that torque value regardless of speed. **

*Keep in mind that the pulse count of the spindle is governed by the encoder connected to it. 
If using a 400cpr encoder, 400 pulses from the motion controller would be 1 rotation.
**I am using a power solutions motor driver which has feedback via I2C to get the voltage and
current applied to the motor along with driver temp and a few other things. 
