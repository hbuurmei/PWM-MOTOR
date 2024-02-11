struct ManualPWM {
    int pin_out;
    int resolution;
    long threshold;
    bool pin_state = false;
    int counter = 0;
};

void reset_pwm_period(ManualPWM& manual_pwm);
void update_pwm(ManualPWM& manual_pwm);
void set_duty_cycle(ManualPWM& manual_pwm, int duty_cycle);
