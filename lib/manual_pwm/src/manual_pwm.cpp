#include <Arduino.h>
#include <manual_pwm.hpp>

void reset_pwm_period(ManualPWM& manual_pwm) {
    manual_pwm.counter = 0;
    manual_pwm.pin_state = true;
    digitalWrite(manual_pwm.pin_out, manual_pwm.pin_state);
}
void update_pwm(ManualPWM& manual_pwm) {
    manual_pwm.counter++;
    if (manual_pwm.counter > manual_pwm.threshold && manual_pwm.pin_state == true) {
        manual_pwm.pin_state = false;
        digitalWrite(manual_pwm.pin_out, manual_pwm.pin_state);
    }
}
void set_duty_cycle(ManualPWM& manual_pwm, int duty_cycle) {
    manual_pwm.threshold = long(duty_cycle) * long(manual_pwm.resolution) / 100;
}
