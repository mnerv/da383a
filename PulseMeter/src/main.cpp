/**
 * @file   main.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Heartrate monitor
 * @date   2022-03-18
 *
 * @copyright Copyright (c) 2022
 */
#include <algorithm>

#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "driver/timer.h"

#include "types.hpp"
#include "queue.hpp"
#include "utils.hpp"

// Hide editor error when on macOS, the clang lsp server
// macOS uses don't like the ESP-IDF IRAM_ATTR macro.
#ifdef EDITOR_MACOS
#undef  IRAM_ATTR
#define IRAM_ATTR
#endif

// factor constants for time conversion
constexpr auto ONE_SECOND_US = 1'000'000;
constexpr auto ONE_MINUTE_S  = 60;

// misc configuration
constexpr auto PULSE_PIN = A2;
constexpr auto LED_PIN   = 13;
constexpr auto BAUD_RATE = 115200;

constexpr auto DRAW_RATE   = 30;
constexpr auto DRAW_PERIOD = ONE_SECOND_US / DRAW_RATE;

constexpr auto TIMER_FREQUENCY = 1'000;  // Hz

// OLED configurations
constexpr auto SCREEN_ADDRESS = 0x3C;
constexpr auto SCREEN_WIDTH   = 128;
constexpr auto SCREEN_HEIGHT  = 32;

// max and min threshold for pulse data trigger
constexpr nerv::u16   MAX_THRESHOLD = 3'000;
constexpr nerv::u16   MIN_THRESHOLD = 2'200;

constexpr nerv::usize BUFFER_SIZE   = 1024;
nerv::queue<nerv::u16, BUFFER_SIZE> buffer{};

// timer callback data
nerv::i32 on_time_count = 0;
hw_timer_t* timer       = nullptr;
portMUX_TYPE timer_mux  = portMUX_INITIALIZER_UNLOCKED;

// pulse data
nerv::f32 bpm_period   = 0.0f;

// OLED interface
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// time keeping
nerv::i64 current_time = 0;
nerv::i64 last_beat    = 0;
nerv::i64 last_draw    = 0;

auto IRAM_ATTR on_time() -> void {
    portENTER_CRITICAL_ISR(&timer_mux);
    on_time_count++;
    portEXIT_CRITICAL_ISR(&timer_mux);
}

auto setup() -> void {
    Serial.begin(BAUD_RATE);

    // setup pin mode
    pinMode(PULSE_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    analogSetAttenuation(ADC_11db);  // make sure that the adc
                                     // is set to 11 dB attenuation

    if (!screen.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("ERROR INITIALIZING OLED!");
        for(;;);
    }

    // setup timer interrupt callback at a fixed frequency
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, on_time, true);
    timerAlarmWrite(timer, ONE_SECOND_US / TIMER_FREQUENCY, true);
    timerAlarmEnable(timer);
}

auto loop() -> void {
    if (on_time_count < 1) return;
    // update data
    current_time = esp_timer_get_time();
    portENTER_CRITICAL(&timer_mux);
    on_time_count--;
    portEXIT_CRITICAL(&timer_mux);

    auto const read_value = analogRead(PULSE_PIN);
    auto last_sample      = *std::rbegin(buffer);

    if (last_sample < MIN_THRESHOLD && read_value > MIN_THRESHOLD) {
        bpm_period = float(current_time - last_beat) / static_cast<float>(ONE_SECOND_US);
        last_beat  = current_time;
        digitalWrite(LED_PIN, 1);
    } else {
        digitalWrite(LED_PIN, 0);
    }

    // add read value to buffer queue
    buffer.enq(read_value);

    // render data to OLED
    if (current_time - last_draw < DRAW_PERIOD) return;
    screen.clearDisplay();
    auto it = std::rbegin(buffer);
    for (auto i = 0; i < SCREEN_WIDTH; i++) {
        if (it == std::rend(buffer)) break;
        auto const y = nerv::map<std::int32_t>(*it, 0, 4095, 0, SCREEN_HEIGHT);
        screen.drawPixel(SCREEN_WIDTH - i, SCREEN_HEIGHT - y, SSD1306_WHITE);
        it -= BUFFER_SIZE / SCREEN_WIDTH;
    }
    screen.setCursor(0, 0);
    screen.setTextSize(1);
    screen.setTextColor(SSD1306_WHITE);

    screen.printf("BPM:%.0f", static_cast<float>(ONE_MINUTE_S) / bpm_period);
    screen.display();
    last_draw = current_time;
}

