#include <FastLED.h>
#include <DHT.h>
#include <Stepper.h>

// RGB Configuration
#define LED_PIN 3
#define NUM_LEDS 8
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// DHT Sensor Configuration
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Stepper Motor Configuration
const int STEPS_PER_ROTOR_REV = 32;
const int GEAR_REDUCTION = 64;
const float STEPS_PER_OUT_REV = STEPS_PER_ROTOR_REV * GEAR_REDUCTION;
const float SMALL_STEP_ANGLE = 5.0; // 每次小角度旋转

// Initialize stepper motors
Stepper base_stepper(STEPS_PER_ROTOR_REV, 8, 10, 9, 11);
Stepper arm_stepper(STEPS_PER_ROTOR_REV, A0, A2, A1, A3);
Stepper head_stepper(STEPS_PER_ROTOR_REV, 4, 6, 5, 7);

// Stepper motor positions and targets
float base_position = 0, base_target = 0;
float arm_position = 0, arm_target = 0;
float head_position = 0, head_target = 0;

// 控制亮度变量
int brightness = 100;
bool isLightOn = true;

// 非阻塞控制时间变量
unsigned long lastStepTime = 0;
const unsigned long stepInterval = 50; // 每次小角度旋转的间隔时间（50ms）

unsigned long lastPrintTime = 0;
const long printInterval = 2000; // 2秒打印一次

// 太阳位置更新时间变量
unsigned long lastSunUpdate = 0;
const long sunUpdateInterval = 2000; // 每2秒更新一次太阳位置

// 温湿度传感器的非阻塞读取时间变量
unsigned long previousMillis = 0;
const long interval = 2000; // 2000ms（2秒）读取一次温湿度

// 环境温度控制参数
const float optimalTemp = 25.0; // 适宜温度
const float tempRange = 10.0; // 偏差范围 ±5°C

// 状态变量
String status = "";
float sun_azimuth = 0.0;
float sun_altitude = 0.0;

// Function to move stepper by small increments towards a target angle
bool moveStepperByAngle(Stepper &stepper, float &position, float &target, int speed) {
    if (abs(target - position) < SMALL_STEP_ANGLE) {
        return true; // 已到达目标角度
    }

    // 检查是否到达步进时间间隔
    unsigned long currentMillis = millis();
    if (currentMillis - lastStepTime >= stepInterval) {
        lastStepTime = currentMillis;

        // 每次移动一个小角度
        float stepAngle = (target > position) ? SMALL_STEP_ANGLE : -SMALL_STEP_ANGLE;
        int steps = stepAngle * STEPS_PER_OUT_REV / 360.0;
        position += stepAngle;
        stepper.setSpeed(speed);
        stepper.step(steps);
    }

    return false; // 尚未到达目标角度
}

// Non-blocking function to reset stepper to zero position
void resetStepperToZero(Stepper &stepper, float &position, float &target, int speed) {
    target = 0; // 设置目标角度为零
    moveStepperByAngle(stepper, position, target, speed);
}

// Function to update target angles based on sun's azimuth and altitude
void updateSunPosition() {
    // 计算并更新 base 和 arm 的目标角度
    base_target = sun_azimuth; // base 跟随太阳的方位角
    arm_target = map(sun_altitude, 0, 90, 10, 0); // arm 根据高度角调整，0度时为水平，15度为低头
}

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(255, 100, 0); // 初始颜色
    }
    FastLED.show();
    Serial.begin(9600);
    dht.begin();
}

void loop() {
    unsigned long currentMillis = millis();

    // 打印当前角度信息，每2秒更新一次
    if (currentMillis - lastPrintTime >= printInterval) {
        lastPrintTime = currentMillis;
        Serial.print("Head target:");
        Serial.println(head_target);
        Serial.print("Base target:");
        Serial.println(base_target);
        Serial.print("Arm target:");
        Serial.println(arm_target);
    }

    // 当状态为 "sun" 时，每2秒更新太阳位置
    if (status == "sun" && currentMillis - lastSunUpdate >= sunUpdateInterval) {
        lastSunUpdate = currentMillis;
        updateSunPosition(); // 更新 base 和 arm 的目标角度
    }

    // 使用 millis() 来实现非阻塞式的温湿度读取
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float humidity = dht.readHumidity(); // 读取湿度
        float temperature = dht.readTemperature(); // 读取温度（摄氏度）

        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("读取温湿度失败，请检查传感器连接。");
        } else {
            Serial.print("temp:");
            Serial.print(temperature);
            Serial.print(",humidity:");
            Serial.println(humidity);

            // 仅当 status 为 "environment" 时根据温度调整花朵和 arm
            if (status == "environment") {
                float deviation = abs(temperature - optimalTemp);
                if (deviation >= tempRange) {
                    // 温度偏离适宜范围，花朵完全关闭，arm 角度设为 15 度
                    head_target = 0; // 花朵完全关闭
                    arm_target = 10; // arm 低头
                    FastLED.setBrightness(50); // 调暗 LED
                    Serial.println("Temperature out of range: closing flower and lowering arm.");
                } else {
                    // 温度在适宜范围内，根据偏差调整花朵开合比例和 arm 角度
                    float openness = 1.0 - (deviation / tempRange); // 计算花朵开合比例
                    head_target = openness * -360; // 花朵的开合角度
                    arm_target = openness * 10; // arm 角度接近水平
                    FastLED.setBrightness(255 * openness); // LED 亮度随开合程度
                    Serial.print("Temperature within range: flower openness ");
                    Serial.println(openness * 100, 1);
                }
                FastLED.show();
            }
        }
    }

    // 更新步进电机位置
    moveStepperByAngle(base_stepper, base_position, base_target, 800);
    moveStepperByAngle(arm_stepper, arm_position, arm_target, 800);
    moveStepperByAngle(head_stepper, head_position, head_target, 800);

    // 检查串口输入，处理其他指令
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        int separatorIndex = input.indexOf(':');
        String command = input.substring(0, separatorIndex);
        String value = input.substring(separatorIndex + 1);

        if (command == "brightness" && isLightOn) {
            int newBrightness = map(value.toInt(), 0, 100, 0, 255);
            brightness = newBrightness;
            FastLED.setBrightness(brightness);
            FastLED.show();
        } else if (command == "power") {
            if (value == "on") {
                isLightOn = true;
                FastLED.setBrightness(brightness);
                FastLED.show();
            } else if (value == "off") {
                isLightOn = false;
                FastLED.setBrightness(0);
                FastLED.show();
            }
        } else if (command == "status") {
            if (value == "interactive") {
                status = "interactive";
            } else if (value == "no") {
                head_target = 0;
                base_target = 0;
                arm_target = 0;
                status = "";
            } else if (value == "sun") {
                status = "sun";
            } else if (value == "environment") {
                status = "environment";
            } else if (value == "open") {
                arm_target = arm_position - 10;
            } else if (value == "close") {
                arm_target = arm_position + 10;
            } else if (value == "reset") {
                head_position = 0;
                head_target = 0;
            }
        } else if (command == "sunpos") {
            // 解析并更新太阳位置
            int commaIndex = value.indexOf(',');
            sun_azimuth = value.substring(0, commaIndex).toFloat();
            sun_altitude = value.substring(commaIndex + 1).toFloat();
            Serial.print("Received sun position - Azimuth: ");
            Serial.print(sun_azimuth);
            Serial.print(", Altitude: ");
            Serial.println(sun_altitude);
        }

        Serial.print("Status: ");
        Serial.println(status); // 打印状态，检查是否正确

        // 仅当 status 为 "interactive" 时处理 emo 数据
        if (status == "interactive" && command == "emo") {
            int emoValue = value.toInt();
            if (emoValue == 1) {
                FastLED.setBrightness(255);
                FastLED.show();
                head_target = -360;
                Serial.println("Flower opened and LED at full brightness.");
            } else if (emoValue == 2) {
                FastLED.setBrightness(50); // 调暗 LED 亮度
                FastLED.show();
                head_target = 0;
                Serial.println("Flower closed and LED dimmed.");
            }
        }
    }
}
