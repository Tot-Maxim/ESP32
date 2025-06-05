#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Конфигурация дисплея
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_I2C_ADDRESS 0x3C  // 0x3D для некоторых дисплеев
#define OLED_RESET -1

// Параметры текста
#define TEXT_SIZE 2
#define TEXT_COLOR SSD1306_WHITE
#define DISPLAY_TEXT "DAY 1"
#define TEXT_X_POS ((SCREEN_WIDTH - (6 * TEXT_SIZE * 6)) / 2)  // 6px на символ * масштаб
#define TEXT_Y_POS ((SCREEN_HEIGHT - (8 * TEXT_SIZE)) / 2)     // 8px высота символа

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("Ошибка инициализации OLED!"));
    while(true);
  }
  
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(TEXT_COLOR);
  display.setCursor(TEXT_X_POS, TEXT_Y_POS);
  display.println(DISPLAY_TEXT);
  display.display();
  
  Serial.println(F("Текст успешно выведен"));
}

void loop() {}