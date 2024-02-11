// Defines
#define USE_TIMER_1         true
#define USE_TIMER_2         true
#define POTENTIOMETER_IN    A5
#define PWM_OUT             9
#define PWM_FREQ            100. // Hz
#define CHECK_RESOLUTION    100
#define PWM_UPDATE_FREQ     PWM_FREQ * CHECK_RESOLUTION // Hz
#define MIN_DUTY_CYCLE      0 // %
#define MAX_DUTY_CYCLE      100 // %
#define MIN_ANALOG_READ     0
#define MAX_ANALOG_READ     1023
#define DEBUG               

// Includes
#include <Arduino.h>
#include <stdio.h>
#include <manual_pwm.hpp>
#include <TimerInterrupt.h>


// Create manual PWM Object
ManualPWM manual_pwm;

void setup() {
    // put your setup code here, to run once:
    // Start serial
    Serial.begin(4800);

    // // Populate struct
    manual_pwm.pin_out = PWM_OUT;
    manual_pwm.resolution = CHECK_RESOLUTION;
    
    // Init timer ITimer1 for PWM
    ITimer1.init();
    ITimer1.setFrequency(PWM_FREQ, []{reset_pwm_period(manual_pwm);});  // use lambda function to call the member function

    // Init timer ITimer2 for counting/checking
    ITimer2.init();
    ITimer2.setFrequency(PWM_UPDATE_FREQ, []{update_pwm(manual_pwm);});  // use lambda function to call the member function
}

// Define global variables that we update in the loop
int potMeter_volt;
bool motor_on = true;
void loop() {
    // put your main code here, to run repeatedly:
    static int potMeter_volt_prev = 0;
    potMeter_volt = analogRead(POTENTIOMETER_IN);
    // Turn off motor if key is pressed
    if (Serial.available()) {
        motor_on = false;
        set_duty_cycle(manual_pwm, 0);
    };
    // Only change duty cycle if we actually turn the knob
    if (abs(potMeter_volt - potMeter_volt_prev) > 5 && motor_on) {
        int duty_cycle = map(potMeter_volt, MIN_ANALOG_READ, MAX_ANALOG_READ, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
        set_duty_cycle(manual_pwm, duty_cycle);
        #ifndef DEBUG
            Serial.println("Set threshold: ");
            Serial.println(manual_pwm.threshold);
        #endif
    }
    potMeter_volt_prev = potMeter_volt;

    
    #ifdef DEBUG
        // Using custom serial plotter
        Serial.print(potMeter_volt);
        Serial.print("\t");
        Serial.print(motor_on);
        Serial.print("\t");
        Serial.println(!motor_on);
    #endif
}
