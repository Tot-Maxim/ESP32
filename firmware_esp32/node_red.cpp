#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============= НАСТРОЙКИ =============
const char* WIFI_SSID = "your_wifi_name";
const char* WIFI_PASSWORD = "your_pass";

const char* MQTT_BROKER = "your_ip_server";
const char* MQTT_TOPIC = "sensors/light";
const char* MQTT_COMMAND_TOPIC = "light/control";
const char* MQTT_DISPLAY_TOPIC = "display/control";
const int MQTT_PORT = 1883;

const int I2C_SDA = 21;
const int I2C_SCL = 22;
const int SENSOR_ADDRESS = 0x23;  // Адрес датчика BH1750
const int LED_PIN = 2;

// Конфигурация дисплея
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_I2C_ADDRESS 0x3C
#define OLED_RESET -1
#define TEXT_SIZE 2
#define TEXT_COLOR SSD1306_WHITE

const int SERIAL_BAUDRATE = 115200;
const unsigned long WIFI_TIMEOUT = 10000;
const unsigned long MQTT_RETRY_DELAY = 2000;
const unsigned long PUBLISH_INTERVAL = 5000;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient espClient;
PubSubClient client(espClient);

// Инициализация OLED дисплея
void initDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("Ошибка инициализации OLED!"));
    while(true);
  }
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(TEXT_COLOR);
  display.display();
}

// Обновление текста на дисплее
void updateDisplay(const char* text) {
  display.clearDisplay();
  int16_t x = (SCREEN_WIDTH - (strlen(text) * 6 * TEXT_SIZE)) / 2;
  int16_t y = (SCREEN_HEIGHT - (8 * TEXT_SIZE)) / 2;
  
  display.setCursor(x, y);
  display.println(text);
  display.display();
}

// Инициализация I2C интерфейса
void initI2C() {
    Wire.begin(I2C_SDA, I2C_SCL);
}

// Подключение к WiFi сети
bool connectWiFi() {
    Serial.print("Подключение к WiFi");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > WIFI_TIMEOUT) {
            Serial.println("\nОшибка подключения к WiFi!");
            return false;
        }
        delay(100);
        Serial.print(".");
    }
    
    Serial.printf("\nПодключено. IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
}

// Чтение данных с датчика освещенности BH1750
float readLightIntensity() {
    Wire.beginTransmission(SENSOR_ADDRESS);
    Wire.write(0x10); // Установка режима измерения
    Wire.endTransmission();
    
    delay(120); // Ожидание завершения измерения
    
    Wire.requestFrom(SENSOR_ADDRESS, 2);
    if (Wire.available() == 2) {
        uint16_t val = Wire.read() << 8 | Wire.read();
        return val / 1.2; // Конвертация в люксы
    }
    return -1.0; // Возврат ошибки
}

// Обработка MQTT сообщений
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    if (strcmp(topic, MQTT_COMMAND_TOPIC) == 0) {
        // Управление светодиодом
        digitalWrite(LED_PIN, (strcmp(message, "ON") == 0) ? HIGH : LOW);
    }
    else if (strcmp(topic, MQTT_DISPLAY_TOPIC) == 0) {
        updateDisplay(message); // Обновление дисплея
    }
}

// Подключение к MQTT брокеру
void connectMQTT() {
    while (!client.connected()) {
        String clientId = "esp32-client-" + String(WiFi.macAddress());
        
        if (client.connect(clientId.c_str())) {
            client.subscribe(MQTT_COMMAND_TOPIC);
            client.subscribe(MQTT_DISPLAY_TOPIC);
            Serial.println("Подключено к MQTT брокеру");
        } else {
            Serial.print("Ошибка подключения к MQTT, код: ");
            Serial.print(client.state());
            Serial.println(", повторная попытка...");
            delay(MQTT_RETRY_DELAY);
        }
    }
}

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    initI2C();
    initDisplay();
    updateDisplay("STARTING...");
    
    if (!connectWiFi()) {
        ESP.restart();
    }

    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(mqttCallback);
    connectMQTT();
}

void loop() {
    if (!client.connected()) {
        connectMQTT();
    }
    client.loop();

    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > PUBLISH_INTERVAL) {
        lastMsg = millis();
        
        float light = readLightIntensity();
        if (light >= 0) {
            char msg[20];
            snprintf(msg, sizeof(msg), "%.2f", light);
            client.publish(MQTT_TOPIC, msg);
            Serial.print("Отправлено значение освещенности: ");
            Serial.println(msg); // Вывод значения в Serial
        }
    }
}