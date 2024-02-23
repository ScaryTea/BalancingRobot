# Self balancing 2-motored robot project

![Demo image](https://pp.userapi.com/c849036/v849036018/64b5d/rldk3gHqxjk.jpg)

Robot uses complementary filter to combine data from accelerometer
and gyroscope, software PID regulation algorithm with some other 
logic implemented in get_PID() function for balancing.
Robot has IR reciever, so it can be controlled via IR transmitter.
However, the quality of movement is still poor.

Robot is able to correct the zero position, if the center of mass is displaced.

### Components used: 
* STM32F103 microcontroller
* MPU6050 accelerometer & gyroscope
* TSOP1838 IR reciever
* L298N driver for 2 motors
* ~8V supply voltage
* 2 motors with 1:48 reductors

#### Bare metal code, **no arduino libraries used**.

Inspired by [Mikhail Momot](https://www.youtube.com/watch?v=3N-8OLf_ofs)'s project.
