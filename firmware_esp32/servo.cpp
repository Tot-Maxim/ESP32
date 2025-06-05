#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 13;

int currentAngle = 0;
unsigned long lastUpdateTime = 0;
const int moveInterval = 20;  // Интервал между шагами (мс)
const int pauseDuration = 2000; // Пауза 2 сек

enum State {
    MOVING_TO_90,
    PAUSED_AT_90,
    MOVING_TO_180,
    PAUSED_AT_180
};
State currentState = MOVING_TO_90;

void setup() {
    myServo.attach(servoPin);
    myServo.write(0);  // Начальное положение (0°)
}

void loop() {
    unsigned long currentMillis = millis();

    switch (currentState) {
        case MOVING_TO_90:
            if (currentMillis - lastUpdateTime >= moveInterval) {
                lastUpdateTime = currentMillis;
                if (currentAngle < 90) {
                    currentAngle++;
                    myServo.write(currentAngle);
                } else {
                    myServo.detach();  // Отключаем серву
                    currentState = PAUSED_AT_90;
                    lastUpdateTime = currentMillis;
                }
            }
            break;

        case PAUSED_AT_90:
            if (currentMillis - lastUpdateTime >= pauseDuration) {
                myServo.attach(servoPin);  // Включаем серву
                currentState = MOVING_TO_180;
            }
            break;

        case MOVING_TO_180:
            if (currentMillis - lastUpdateTime >= moveInterval) {
                lastUpdateTime = currentMillis;
                if (currentAngle < 180) {  // Поворачиваем до 180° (вместо -90°)
                    currentAngle++;
                    myServo.write(currentAngle);
                } else {
                    myServo.detach();  // Отключаем серву
                    currentState = PAUSED_AT_180;
                    lastUpdateTime = currentMillis;
                }
            }
            break;

        case PAUSED_AT_180:
            if (currentMillis - lastUpdateTime >= pauseDuration) {
                myServo.attach(servoPin);
                myServo.write(0);  // Возвращаемся в 0° (резкий сброс)
                currentAngle = 0;
                currentState = MOVING_TO_90;  // Цикл повторяется
            }
            break;
    }
}