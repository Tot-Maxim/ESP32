#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA на GPIO21, SCL на GPIO22
  
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("Ошибка инициализации BH1750!");
    while (1);
  }
  
  Serial.println("BH1750 готов к работе!");
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Освещенность: ");
  Serial.print(lux);
  Serial.println(" лк");
  
  delay(1000); // Задержка 1 секунда
}