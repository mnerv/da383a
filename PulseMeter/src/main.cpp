/**
 * @file   main.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Heartrate monitor
 * @date   2022-03-18
 *
 * @copyright Copyright (c) 2022
 */
#include <cstdint>
#include <type_traits>
#include <cstddef>
#include <iterator>
#include <algorithm>

#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "driver/timer.h"

#include "queue.hpp"

// Hide editor error when on macOS, the clang lsp server
// macOS uses don't like the ESP-IDF IRAM_ATTR macro.
#ifdef EDITOR_MACOS
#undef  IRAM_ATTR
#define IRAM_ATTR
#endif

namespace nerv {
using f32 = float;

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
auto map(T const& x, T const& in_min, T const& in_max, T const& out_min, T const& out_max) -> T {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
}

constexpr auto ONE_SECOND_US = 1'000'000;
constexpr auto ONE_MINUTE_S  = 60;

constexpr auto PULSE_PIN = A2;
constexpr auto LED_PIN   = 13;
constexpr auto BAUD_RATE = 115200;

constexpr auto SCREEN_ADDRESS = 0x3C;
constexpr auto SCREEN_WIDTH   = 128;
constexpr auto SCREEN_HEIGHT  = 32;

constexpr std::uint16_t MAX_THRESHOLD = 3'000;
constexpr std::uint16_t MIN_THRESHOLD = 2'200;
constexpr std::size_t BUFFER_SIZE = 300;
nerv::queue<std::uint16_t, BUFFER_SIZE> buffer;

std::int32_t on_time_count = 0;
hw_timer_t* timer          = nullptr;
portMUX_TYPE timer_mux     = portMUX_INITIALIZER_UNLOCKED;

std::int64_t current_time = 0;
std::int64_t last_beat    = 0;
std::int64_t last_bpm     = 0;
nerv::f32    bpm_period   = 0.0f;

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

auto IRAM_ATTR on_time() -> void {
    portENTER_CRITICAL_ISR(&timer_mux);
    on_time_count++;
    portEXIT_CRITICAL_ISR(&timer_mux);
}

auto setup() -> void {
    Serial.begin(BAUD_RATE);
    pinMode(PULSE_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    analogSetAttenuation(ADC_11db);

    if (!screen.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("ERROR INITIALIZING OLED!");
        for(;;);
    }

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, on_time, true);
    timerAlarmWrite(timer, ONE_SECOND_US / 1'000, true);
    timerAlarmEnable(timer);
}

auto loop() -> void {
    if (on_time_count < 1) return;
    portENTER_CRITICAL(&timer_mux);
    on_time_count--;
    portEXIT_CRITICAL(&timer_mux);
    current_time = esp_timer_get_time();

    auto const read_value = analogRead(PULSE_PIN);
    auto last_sample = *std::rbegin(buffer);

    if (last_sample < MIN_THRESHOLD && read_value > MIN_THRESHOLD) {
        bpm_period = float(current_time - last_beat) / static_cast<float>(ONE_SECOND_US);
        last_beat = current_time;
        digitalWrite(LED_PIN, 1);
    } else {
        digitalWrite(LED_PIN, 0);
    }
    buffer.enq(read_value); // max value is 12 bit

    screen.clearDisplay();
    auto it = std::rbegin(buffer);
    for (auto i = 0; i < SCREEN_WIDTH; i++) {
        auto const y = nerv::map<std::int32_t>(*it++, 0, 4095, 0, SCREEN_HEIGHT);
        screen.drawPixel(SCREEN_WIDTH - i, SCREEN_HEIGHT - y, SSD1306_WHITE);
    }
    screen.setCursor(0, 0);
    screen.setTextSize(1);
    screen.setTextColor(SSD1306_WHITE);

    screen.printf("BPM:%.0f", ONE_MINUTE_S / bpm_period );
    screen.display();
}

