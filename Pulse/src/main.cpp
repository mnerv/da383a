/**
 * @file   main.cpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Pulse monitor. This project uses a lightbase heartbeat sensor on the
 *         finger tip. This implementation does not have any signal filter and
 *         calculate the BPM between every trigger point.
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
#include "ring.hpp"
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
constexpr nrv::u16 MAX_THRESHOLD = 3'000;
constexpr nrv::u16 MIN_THRESHOLD = 2'200;

nrv::ring<nrv::f32, 2048> draw_buffer{};

// timer callback data
nrv::i32 on_time_count = 0;
hw_timer_t* timer       = nullptr;
portMUX_TYPE timer_mux  = portMUX_INITIALIZER_UNLOCKED;

// pulse data
nrv::f32 bpm_period   = 0.0f;

// OLED interface
Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// time keeping
nrv::i64 current_time = 0;
nrv::i64 last_beat    = 0;
nrv::i64 last_update  = 0;
nrv::i64 last_draw    = 0;

namespace nrv {
template <typename T, std::size_t N>
constexpr auto length_of(T (&)[N]) -> std::size_t {
    return N;
}

auto iir_high_pass(nrv::f64 const& value) -> nrv::f32 {
    // [Hz] Butterworth
    // Fs    = 1000
    // Fstop = 0.1
    // Fpass = 0.8
    // [dB]
    // Astop = 80
    // Apass = 1
    static nrv::f64 b[] = {
         0.9936059630099,    -4.96802981505,    9.936059630099,   -9.936059630099,
           4.96802981505,  -0.9936059630099
    };
    static nrv::f64 a[] = {
                       1,   -4.987170880032,     9.94876577478,   -9.923271718397,
          4.948929633379,  -0.9872528097289
    };
    static nrv::ring<nrv::f64, length_of(b)> x{};
    static nrv::ring<nrv::f64, length_of(a)> y{};

    x.enq(value);

    auto forward = 0.0;
    for (std::size_t i = 0; i < x.capacity(); i++) {
        forward += b[i] * x[i];
    }

    auto feedback = 0.0;
    for (std::size_t i = 1; i < y.capacity(); i++) {
        feedback += -a[i] * y[i - 1];
    }
    y.enq(forward + feedback);
    return *y.rbegin();
}

auto iir_low_pass(nrv::f64 const& value) -> nrv::f32 {
    // [Hz] Butterworth
    // Fs    = 1000
    // Fpass = 5
    // Fstop = 30
    // [dB]
    // Astop = 80
    // Apass = 1
    static nrv::f64 b[] = {
        6.594578622361e-11,3.956747173417e-10,9.891867933541e-10,1.318915724472e-09,
        9.891867933541e-10,3.956747173417e-10,6.594578622361e-11
    };
    static nrv::f64 a[] = {
                     1,   -5.842652126594,    14.22559205773,   -18.47523699553,
        13.49869991173,   -5.260796021795,   0.8543931786795
    };
    static nrv::ring<nrv::f64, length_of(b)> x{};
    static nrv::ring<nrv::f64, length_of(a)> y{};

    x.enq(value);

    auto forward = 0.0;
    for (std::size_t i = 0; i < x.capacity(); i++) {
        forward += b[i] * x[i];
    }

    auto feedback = 0.0;
    for (std::size_t i = 1; i < y.capacity(); i++) {
        feedback += -a[i] * y[i - 1];
    }
    y.enq(forward + feedback);
    return *y.rbegin();
}
}

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

    // Apply High- and Low-pass filter to achieve bandpass
    nrv::f32 value = nrv::iir_high_pass(nrv::f32(read_value));
    value = nrv::iir_low_pass(value);

    if (*std::rbegin(draw_buffer) < MIN_THRESHOLD && read_value > MIN_THRESHOLD) {
        bpm_period = float(current_time - last_beat) / static_cast<float>(ONE_SECOND_US);
        last_beat  = current_time;
        digitalWrite(LED_PIN, 1);
    } else {
        digitalWrite(LED_PIN, 0);
    }

    // add read value to draw_buffer ring
    draw_buffer.enq(value);

    auto update_delta = current_time - last_update;
    last_update = current_time;
    // render data to OLED
    if (current_time - last_draw < DRAW_PERIOD) return;
    screen.clearDisplay();

    nrv::i32 min = INT32_MAX, max = INT32_MIN;

    for (std::size_t i = 0; i < draw_buffer.capacity(); i++) {
        auto value = draw_buffer.at_back(i);
        if (value > max) max = value;
        if (value < min) min = value;
    }

    auto it = std::rbegin(draw_buffer);
    for (auto i = 0; i < SCREEN_WIDTH; i++) {
        if (it == std::rend(draw_buffer) || min == max) break;
        auto const y = nrv::map<nrv::i32>(nrv::i32(*it), min, max, 0, SCREEN_HEIGHT);
        screen.drawPixel(SCREEN_WIDTH - i, y, SSD1306_WHITE);
        it -= draw_buffer.capacity() / SCREEN_WIDTH;
    }

    // print BPM to OLED
    screen.setCursor(0, 0);
    screen.setTextSize(1);
    screen.setTextColor(SSD1306_WHITE);
    //screen.printf("BPM:%.0f", static_cast<float>(ONE_MINUTE_S) / bpm_period);
    screen.printf("MAX:%d", max);

    screen.display();
    Serial.printf("frame time: %lld ms, update: %lld ms\n", (current_time - last_draw) / 1000, update_delta / 1000);
    last_draw = current_time;
}

