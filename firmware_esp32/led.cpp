// Оптимизированный код для ledPWM
#include <driver/ledc.h>

#define LED_PIN 2
#define PWM_FREQ 1000
#define FADE_DELAY 10
#define FADE_STEP 5

struct LedState {
    uint16_t brightness = 0;
    int16_t step = FADE_STEP;
    bool directionUp = true;
};

LedState led;

void setup() {
    // Настройка ШИМ
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = LED_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);
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

void loop() {
    static uint32_t tmr = 0;
    if (millis() - tmr >= FADE_DELAY) {
        tmr = millis();
        updateBrightness(led);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, led.brightness);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    }
}