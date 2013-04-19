# General information #

We generally wrote the code using pair programming. The code was reviewed by everyone so that the team understands the whole code and not just the part each of us has done.

We used git to keep track of the versions and used Github as the remote repository.

Url to the Github repository: https://github.com/HenriHonkanen/Embedded-Systems-Project

## To Build and run ##

`make
make flash`

# Introduction #

We decided to create one module per electronical component.
Here we described how we implemented them.

## Servo ##
*Mode of operation:* PWM

*Frequency:* 250Hz

*Interface functions:*

*	InitServo(int startValue): Initialize the servo with a startValue i.e. the degree of the turn (initial value is usually 0 as the car shouldn't turn initially) as an argument so that it is ready to turn properly
*	StopServo(void): stop the servo (not used so much)
*	MoveServo(int targetValue): Move the servo with a targetValue i.e. the degree of the turn as an argument so that it turns to the target direction 

## Motor ##
*Mode of operation:* PWM

*Frequency:* 9988Hz

*Interface functions:*

*	InitMotor(void): Initialize the motor so that it is ready to run properly
*	SetMotorSpeed(int percentage): Set up the motor speed with the percentage of 1/3 (for safety) of the maximum speed of the motor as an argument

## Tachometer ##
*Interface functions:*

*	SetUpTachometer(void): set up the tachometer with an external timer i.e. timer5, and add set up timer3 for counting purpose.
*	GetTachometerTimerInSeconds(void): return the timer3 elapsed time in seconds.

Tachometer is functioning as such that each time the microcontroller receives a pulse of the motor's turn it will cause a compare match interrupt. In the interrupt handler, we get the timer3 elapsed time in second and calculate the motor's frequency. The timer3 is then restarted.


## LCD ##
*Baud rate needed:* 9600 

*Serial Data Format:* 8 Bits, No Parity, 1 Stop Bit. Serial data is true and not inverted

Interface function description skipped because this is not a compulsory part of implementation. We implemented the LCD by printing out some practical text information of the car and its current status.

# Driving logic #

Our main driving logic is written inside an infinite loop.

## Start ##

We have added an interrupt on the black button. When the interrupt hits, the handler writes the new status (driving) on the LCD and set a flag to true to start the driving instuctions.

## Sensoring ##

We collect the values of the sensors on each loop. 

We then check if we are currently on the starting line. If more than 3 sensors are hit at the same time, we consider to have passed the starting line and increment the lap counter by 1. If multiple sensors are hit, we only take the most extreme one into account. This is a safety to stay on track as much as possible. Based on the sensor hit, we choose a value that is a percentage of the maximum steering angle (45°).

## Steering ##

After we received the percentage from the sensoring, we calculate the real angle (percentage * 45°) and steer using the servo module.

## Tachometer ##

Everytime the motor makes a new turn, an interruption is triggered. This interruption calculates the motor frequency and stores it in a global variable. This variable is used to calculate the motor speed.

## Motor ##

We decided to accelerate on straight lines and deccelerate on curves. To do so, we calculate a new motor speed based on the new steering direction. At first, the steering direction was used to calculate a new motor power. However, during testing, we realized that it was not the best solution. Indeed, the power is dependent on the battery use: the car was driving faster when the battery was new and slower when it was almost flat. We then decided to use the tachometer to get more control over the car speed.

We calculate the target motor speed in relation with new direction. We assign a minimum speed (9 motor turns/s) and accelerate in proportion to the direction, with a maximum of (14 motor turns/s). On each loop, the current motor speed is read from the global variable and used to calculate how much we need to accelerate/deccelerate. Instead of setting the target motor speed directly and induce drastic acceleration/decceleration, we decided to do it smoothly. On each loop, we approach the target motor speed smoothly by modifying adding/removing a small amount of the difference between target speed and current speed. This result in a gradual acceleration/decceleration and a small oscillation around the target speed if the target speed doesn't change.

## Stop ##

When the car has driven 3 laps, a flag is set to stop the motor, set the wheels to a straight direction and write the new driving status to the LCD.

# Possible improvements #

Our driving logic is quite precarious. However, we didn't have time to build a real artificial intelligence.

That said, there are still a few improvements that we could have made if we had a bit more time.

*	Stop the car if no sensored were hit during 1 second. This could have been added for safety in case the car goes out of track.
*	Configure the number of laps and the speed parameters (we could tweak it a bit based on what track we are driving on) via the LCD and buttons.
*	Timed based acceleration instead of relying on the loop execution speed.













