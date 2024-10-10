#include <Servo.h>
#include <Arduino.h>

Servo base_servo;  // create servo object to control a servo

int POT_PIN = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
    base_servo.attach(9);  // attaches the servo on pin 9 to the servo object
    Serial.begin(9600);  // 设置波特率为9600
}

void loop() {
    val = analogRead(POT_PIN);
    // reads the value of the potentiometer (value between 0 and 1023)
    val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
    base_servo.write(val);
    Serial.println(val);                  // sets the servo position according to the scaled value·
    delay(20);                           // waits for the servo to get there
}