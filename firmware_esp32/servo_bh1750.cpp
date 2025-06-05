#include <Wire.h>
#include <BH1750.h>
#include <ESP32Servo.h>

// =============================================
// КОНФИГУРАЦИОННЫЕ ПАРАМЕТРЫ
// =============================================

// Конфигурация пинов
#define SDA_PIN 21       // Пин для I2C SDA
#define SCL_PIN 22       // Пин для I2C SCL
#define SERVO_PIN 13     // Пин управления сервоприводом

// Параметры сервопривода
#define SERVO_INIT_ANGLE 90    // Начальный угол сервы (нейтральное положение)
#define SERVO_MIN_ANGLE 0      // Минимальный угол поворота сервы
#define SERVO_MAX_ANGLE 180    // Максимальный угол поворота сервы
#define MOVE_DURATION_MS 120   // Время движения сервы (мс)
#define SERVO_DETACH_DELAY_MS 1000 // Задержка перед отключением сервы (мс)

// Параметры датчика освещенности
#define LUX_THRESHOLD 300      // Порог освещенности для срабатывания (люкс)
#define LUX_READ_DELAY_MS 200  // Задержка между измерениями освещенности (мс)

// Сообщения для Serial
#define MSG_BH1750_INIT_OK "Датчик BH1750 успешно инициализирован"
#define MSG_BH1750_INIT_FAIL "Ошибка инициализации датчика BH1750!"
#define MSG_SERVO_INIT "Сервопривод инициализирован в положении: "
#define MSG_LIGHT_READING "Освещенность: "
#define MSG_LUX_UNIT " люкс"
#define MSG_THRESHOLD_CROSSED "Порог освещенности пересечен (" + String(LUX_THRESHOLD) + " люкс) - поворот на "
#define MSG_DEGREE_SYMBOL "°"
#define MSG_SERVO_MOVING "Поворот сервопривода на: "
#define MSG_SERVO_COMPLETE "Поворот завершен. Текущий угол: "

// =============================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// =============================================

BH1750 lightMeter;      // Объект датчика освещенности
Servo myServo;          // Объект сервопривода

// Состояние сервопривода
unsigned long servoStartTime = 0;  // Время начала движения сервы
bool isServoMoving = false;        // Флаг движения сервы
int targetAngle = SERVO_INIT_ANGLE; // Текущий целевой угол сервы

// Состояние освещенности
float prevLux = 0;      // Предыдущее значение освещенности
bool wasDark = false;   // Флаг предыдущего состояния (темно/светло)

// =============================================
// ФУНКЦИИ
// =============================================

/**
 * Функция инициализации системы
 * Выполняется один раз при старте устройства
 */
void setup() {
  Serial.begin(115200); // Инициализация последовательного порта
  
  // Инициализация сервопривода
  myServo.attach(SERVO_PIN);           // Подключение сервы
  myServo.write(SERVO_INIT_ANGLE);     // Установка в начальное положение
  delay(300);                          // Задержка для стабилизации
  myServo.detach();                    // Отключение сервы для экономии энергии
  Serial.print(MSG_SERVO_INIT);        // Вывод сообщения о инициализации
  Serial.println(SERVO_INIT_ANGLE);

  // Инициализация датчика освещенности
  Wire.begin(SDA_PIN, SCL_PIN);        // Настройка I2C
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(MSG_BH1750_INIT_OK); // Успешная инициализация
  } else {
    Serial.println(MSG_BH1750_INIT_FAIL); // Ошибка инициализации
  }
}

/**
 * Основной цикл программы
 * Выполняется циклически после setup()
 */
void loop() {
  // Чтение текущего уровня освещенности
  float lux = lightMeter.readLightLevel();
  
  // Отладочный вывод (закомментирован)
  // Serial.print(MSG_LIGHT_READING);
  // Serial.print(lux);
  // Serial.println(MSG_LUX_UNIT);
  
  // Проверка условий для движения сервы
  if (!isServoMoving) {
    // Если освещенность перешла снизу вверх через порог
    if (prevLux <= LUX_THRESHOLD && lux > LUX_THRESHOLD && !wasDark) {
      handleLightChange(SERVO_MAX_ANGLE, true);
    } 
    // Если освещенность перешла сверху вниз через порог
    else if (prevLux > LUX_THRESHOLD && lux <= LUX_THRESHOLD && wasDark) {
      handleLightChange(SERVO_MIN_ANGLE, false);
    }
  }
  
  // Проверка завершения движения сервы
  if (isServoMoving && (millis() - servoStartTime >= MOVE_DURATION_MS)) {
    completeServoMovement();
  }
  
  prevLux = lux; // Сохранение текущего значения освещенности
  delay(LUX_READ_DELAY_MS); // Задержка между измерениями
}

/**
 * Обработка изменения уровня освещенности
 * @param angle - целевой угол поворота сервы
 * @param newDarkState - новое состояние освещенности
 */
void handleLightChange(int angle, bool newDarkState) {
  Serial.print("Порог освещенности пересечен (");
  Serial.print(LUX_THRESHOLD);
  Serial.print(" люкс) - поворот на ");
  Serial.print(angle);
  Serial.println(MSG_DEGREE_SYMBOL);
  activateServo(angle);      // Активация сервы
  wasDark = newDarkState;    // Обновление состояния освещенности
}

/**
 * Активация сервопривода
 * @param angle - угол, на который нужно повернуть серву
 */
void activateServo(int angle) {
  targetAngle = angle;               // Установка целевого угла
  myServo.attach(SERVO_PIN);         // Подключение сервы
  myServo.write(targetAngle);        // Установка угла
  servoStartTime = millis();         // Фиксация времени начала движения
  isServoMoving = true;              // Установка флага движения
  // Вывод информации о движении
  Serial.print(MSG_SERVO_MOVING);
  Serial.print(targetAngle);
  Serial.println(MSG_DEGREE_SYMBOL);
}

/**
 * Завершение движения сервопривода
 */
void completeServoMovement() {
  myServo.detach();                  // Отключение сервы
  isServoMoving = false;             // Сброс флага движения
  // Вывод информации о завершении
  Serial.print(MSG_SERVO_COMPLETE);
  Serial.println(targetAngle);
}