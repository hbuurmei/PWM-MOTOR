// Defines
#define USE_TIMER_1         true
#define USE_TIMER_2         true
#define POTENTIOMETER_IN    A5
#define PWM_OUT             9
#define FWD_PIN             7
#define REV_PIN             4
#define PWM_FREQ            100. // Hz
#define CHECK_RESOLUTION    100
#define PWM_UPDATE_FREQ     PWM_FREQ * CHECK_RESOLUTION // Hz
#define MIN_DUTY_CYCLE      0 // %
#define MAX_DUTY_CYCLE      100 // %
#define MIN_ANALOG_READ     0
#define MAX_ANALOG_READ     1023
// #define MANUAL_PWM
// #define DEBUG            

// Includes
#include <Arduino.h>
#include <TimerInterrupt.h>
#ifdef MANUAL_PWM
    #include <manual_pwm.hpp>
    // Create manual PWM Object
    manual_pwm_t manual_pwm;
#endif

enum DIRECTION {
    REVERSE,
    FORWARD
};
DIRECTION direction = FORWARD;

void setup() {
    // put your setup code here, to run once:
    // Start serial
    Serial.begin(9600);  // use 4800 for serial plotting
    pinMode(PWM_OUT,OUTPUT);
    pinMode(FWD_PIN,OUTPUT);
    pinMode(REV_PIN,OUTPUT);

    #ifdef MANUAL_PWM
        // Populate struct
        manual_pwm.pin_out = PWM_OUT;
        manual_pwm.resolution = CHECK_RESOLUTION;
        
        // Init timer ITimer1 for PWM
        ITimer1.init();
        ITimer1.setFrequency(PWM_FREQ, []{reset_pwm_period(manual_pwm);});  // use lambda function to call the member function

        // Init timer ITimer2 for counting/checking
        ITimer2.init();
        ITimer2.setFrequency(PWM_UPDATE_FREQ, []{update_pwm(manual_pwm);});  // use lambda function to call the member function
    #endif
}

// Define global variables that we update in the loop
int potMeter_volt;
// int new_potMeter_volt;
bool motor_on = true;

void loop() {
    // put your main code here, to run repeatedly:
    static int potMeter_volt_prev = 0;
    static DIRECTION direction_prev = direction;
    potMeter_volt = analogRead(POTENTIOMETER_IN);

    if (Serial.available()) {
        while (Serial.available()) {Serial.read();}
        #ifdef MANUAL_PWM
            // Turn off motor if key is pressed
            motor_on = false;
            set_duty_cycle(manual_pwm, 0);
        #else
            direction == FORWARD ? direction = REVERSE : direction = FORWARD;
            Serial.print("Set Direction to ");
            Serial.println(direction);
        #endif
    };
    // Only change duty cycle if we actually turn the knob
    // Serial.println(abs(potMeter_volt - potMeter_volt_prev) > 5);
    if (abs(potMeter_volt - potMeter_volt_prev) > 5 && motor_on) {
        long duty_cycle = map(potMeter_volt, MIN_ANALOG_READ, MAX_ANALOG_READ, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

        #ifdef MANUAL_PWM
            set_duty_cycle(manual_pwm, duty_cycle);
        #else
            analogWrite(PWM_OUT, duty_cycle * 255 / 100);
            Serial.println("Set duty cycle:"); 
            Serial.println(duty_cycle);
        #endif
        #ifndef DEBUG
            #ifdef MANUAL_PWM
                Serial.println("Set threshold: ");
                Serial.println(manual_pwm.threshold);
            #endif
        #endif
        potMeter_volt_prev = potMeter_volt;
    }
    

    if (direction != direction_prev) {
        switch (direction)
        {
        case FORWARD:
            digitalWrite(FWD_PIN, HIGH);
            digitalWrite(REV_PIN, LOW);
            break;
        case REVERSE:
            digitalWrite(FWD_PIN, LOW);
            digitalWrite(REV_PIN, HIGH);
            break;
        default:
            digitalWrite(FWD_PIN, LOW);
            digitalWrite(REV_PIN, LOW);
            break;
        }
    }
    direction_prev = direction;
    
    
    #ifdef DEBUG
        // Using custom serial plotter
        Serial.print(potMeter_volt);
        Serial.print("\t");
        Serial.print(motor_on);
    #endif
}
