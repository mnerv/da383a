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

// Hide editor error when on macOS, the clang lsp server
// macOS uses don't like the ESP-IDF IRAM_ATTR.
#ifdef EDITOR_MACOS
#undef  IRAM_ATTR
#define IRAM_ATTR
#endif

namespace nerv {
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
auto map(T const& x, T const& in_min, T const& in_max, T const& out_min, T const& out_max) -> T {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Simple circular buffer that overrides the oldest value new value is added.
// FIFO datastructure, contains a simple a iterator.
template <typename T, std::size_t SIZE>
class buffer {
  public:
    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        iterator(std::size_t ptr, buffer* bfr)
            : m_buffer(bfr), m_ptr(ptr) {}

        auto operator*() const -> reference { return (*m_buffer)[m_ptr]; }
        auto operator->() -> pointer { return &(*m_buffer)[m_ptr]; }

        // prefix increment
        auto operator++() -> iterator& {
            m_ptr = (m_ptr + m_buffer->m_max - 1) % m_buffer->m_max;
            return *this;
        }
        // postfix increment
        auto operator++(std::int32_t) -> iterator {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        auto operator--() -> iterator& {
            m_ptr = (m_ptr + 1) % m_buffer->m_max;
            return *this;
        }
        auto operator--(std::int32_t) -> iterator {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        friend auto operator==(iterator const& a, iterator const& b) -> bool {
            return a.m_ptr == b.m_ptr;
        }
        friend auto operator!=(iterator const& a, iterator const& b) -> bool {
            return !(a == b);
        }

      private:
        buffer*     m_buffer;
        std::size_t m_ptr;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    iterator begin() { return iterator(m_tail, this); }
    iterator end() { return iterator(m_head, this); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

  public:
    auto enq(T const& value) -> void {
        m_buffer[m_head] = value;
        if (buffer::dec_wrap(m_head, m_max) == m_tail)
            buffer::dec_wrap(m_tail, m_max);
        else
            ++m_size;
    }
    auto deq() -> T {
        auto const ret = m_buffer[m_tail];
        if (m_tail != m_head) {
            buffer::dec_wrap(m_tail, m_max);
            --m_size;
        }
        return ret;
    }

    auto size() const -> std::size_t { return m_size; }
    auto back() -> T { return m_buffer[m_tail]; }
    auto front() -> T { return m_buffer[(m_head + 1) % m_max]; }

    auto operator[](std::size_t index) -> T& { return m_buffer[index]; }
    //auto operator[](std::size_t index) const -> T const& { return m_buffer[index]; }
  private:
    static auto inc_wrap(std::size_t& value, std::size_t const& max) -> std::size_t const& {
        value = (value + 1) % max;
        return value;
    }
    static auto dec_wrap(std::size_t& value, std::size_t const& max) -> std::size_t const& {
        value = (value + max - 1) % max;
        return value;
    }

  private:
    std::size_t m_max   = SIZE + 1;
    T           m_buffer[SIZE + 1];  // allocate extra space for end iterator
    std::size_t m_head  = 0;
    std::size_t m_tail  = SIZE;
    std::size_t m_size  = 0;
};
}

constexpr auto PULSE_PIN = A2;
constexpr auto LED_PIN   = 13;
constexpr auto BAUD_RATE = 115200;

constexpr auto SCREEN_ADDRESS = 0x3C;
constexpr auto SCREEN_WIDTH   = 128;
constexpr auto SCREEN_HEIGHT  = 32;

constexpr auto ONE_SECOND_US  = 1'000'000;

Adafruit_SSD1306* screen;
constexpr std::size_t BUFFER_SIZE = 300;
nerv::buffer<std::uint16_t, BUFFER_SIZE> buffer;

std::int32_t on_time_count = 0;
hw_timer_t* timer          = nullptr;
portMUX_TYPE timer_mux     = portMUX_INITIALIZER_UNLOCKED;

std::int64_t current_time = 0;
std::int64_t last_beat    = 0;
std::int64_t last_bpm     = 0;

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

    screen = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
    if (!screen->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("ERROR");
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
    buffer.enq(read_value); // max value is 12 bit

    if (*(--std::end(buffer)) >= 1200) {
        last_beat = current_time;
        digitalWrite(LED_PIN, 1);
    } else {
        digitalWrite(LED_PIN, 0);
    }

    screen->clearDisplay();
    auto it = std::rbegin(buffer);
    for (auto i = 0; i < BUFFER_SIZE; i++) {
        auto const value = float(*it++);
        auto const s = nerv::map(value, 0.0f, 4095.0f, 0.0f, 1.0f);
        auto y = s * -32.0f;
        screen->drawPixel(SCREEN_WIDTH - i, 32 + y, SSD1306_WHITE);
    }
    screen->setCursor(90, 0);
    screen->setTextSize(1);
    screen->setTextColor(SSD1306_WHITE);
    screen->printf("%.2f", ONE_SECOND_US / float(current_time - last_beat));
    screen->display();

}

