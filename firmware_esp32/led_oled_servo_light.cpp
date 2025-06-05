// Код для управления WS2812B-8 светодиодами (цвет #00FFFF — голубой)
#include <FastLED.h>
#include <driver/ledc.h>

#define NUM_LEDS 8           // Количество светодиодов
#define DATA_PIN 18          // Пин подключения DIN (GPIO18)
#define PWM_FREQ 1000        // Частота ШИМ (Гц)
#define FADE_DELAY 10        // Задержка изменения яркости (мс)
#define FADE_STEP 5          // Шаг изменения ярчности
#define COLOR_CODE 0x00FFFF  // Бирюзовый цвет (#00FFFF)

CRGB leds[NUM_LEDS];         // Массив светодиодов

struct LedState {
    uint16_t brightness = 0; // Текущая яркость (0-255)
    int16_t step = FADE_STEP;// Шаг изменения яркости
    bool directionUp = true; // Направление изменения
    CRGB color = COLOR_CODE; // Цвет светодиодов
};

LedState led;

void setup() {
    // Инициализация адресных светодиодов
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    
    // Настройка PWM для управления общей яркостью
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = DATA_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);
    
    // Установка начального цвета
    setColor(led.color);
}

void updateBrightness(LedState &state) {
    if (state.directionUp) {
        state.brightness += state.step;
        if (state.brightness >= 255) {
            state.brightness = 255;
            state.directionUp = false;
        }
    } else {
        state.brightness -= state.step;
        if (state.brightness <= 0) {
            state.brightness = 0;
            state.directionUp = true;
        }
    }
}

void setColor(CRGB color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = color;
    }
    FastLED.show();
}

void loop() {
    static uint32_t tmr = 0;
    if (millis() - tmr >= FADE_DELAY) {
        tmr = millis();
        updateBrightness(led);
        
        // Применяем яркость через ШИМ
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, led.brightness);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        
        // Обновляем цвет с текущей яркостью
        CRGB adjustedColor = led.color;
        adjustedColor.fadeToBlackBy(255 - led.brightness);
        setColor(adjustedColor);
    }
}