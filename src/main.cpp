#include <Servo.h>
#include <Arduino.h>

Servo base_servo;
Servo arm_servo;

int base_pot_pin = 0;
int arm_pot_pin = 1;
int head_pot_pin = 2;

int base_val;
int arm_val;

int smoothAnalogRead(int pin) {
    int total = 0;
    int readings = 10;
    for (int i = 0; i < readings; i++) {
        total += analogRead(pin);
        delay(10);
    }
    return total / readings;
}

void setup() {
    base_servo.attach(9);
    arm_servo.attach(10);
    Serial.begin(9600);
}

void loop() {
    base_val = smoothAnalogRead(base_pot_pin);
    arm_val = smoothAnalogRead(arm_pot_pin);

    base_val = map(base_val, 0, 1023, 0, 180);
    arm_val = map(arm_val, 0, 1023, 0, 180);

    base_servo.write(base_val);
    arm_servo.write(arm_val);

    Serial.print("Base Servo Value: ");
    Serial.println(base_val);

    Serial.print("Arm Servo Value: ");
    Serial.println(arm_val);

    delay(50);
}
