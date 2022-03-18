#include <cstdint>
#include <type_traits>
#include <cstddef>
#include <iterator>

#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#ifdef EDITOR_MACOS
#undef  IRAM_ATTR
#define IRAM_ATTR
#endif

namespace nerv {
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
auto map(T const& x, T const& in_min, T const& in_max, T const& out_min, T const& out_max) -> T {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// FIFO
template <typename T, std::size_t SIZE>
class buffer {
  public:
    struct iterator {
        using interator_category = std::forward_iterator_tag;
        using difference_type    = std::ptrdiff_t;

        iterator(std::size_t ptr, buffer* bfr, std::size_t incr = 0)
            : m_buffer(bfr), m_ptr(ptr), m_incr(incr) {}

        auto operator*() const -> T& { return (*m_buffer)[m_ptr]; }
        auto operator->() -> T* { return &(*m_buffer)[m_ptr]; }

        // prefix increment
        auto operator++() -> iterator& {
            m_ptr = (m_ptr + m_buffer->m_max - 1) % m_buffer->m_max;
            if (m_incr < m_buffer->m_count) m_incr++;
            return *this;
        }
        // postfix increment
        auto operator++(std::int32_t) -> iterator {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend auto operator==(iterator const& a, iterator const& b) -> bool {
            return a.m_ptr == b.m_ptr;
        }
        friend auto operator!=(iterator const& a, iterator const& b) -> bool {
            return a.m_incr != b.m_incr;
        }

      private:
        buffer*     m_buffer;
        std::size_t m_ptr;
        std::size_t m_incr;
    };

    iterator begin() { return iterator(calc_tail(), this); }
    iterator end() { return iterator(m_head, this, m_count); }

  public:
    auto enq(T const& value) -> void {
        m_buffer[m_head] = value;
        dec(m_head);
        if (m_count < m_max) ++m_count;
    }
    auto deq() -> T {
        auto index = calc_tail();
        if (m_count > 0) --m_count;
        return m_buffer[index];
    }

    auto operator[](std::size_t index) -> T& { return m_buffer[index]; }
    //auto operator[](std::size_t index) const -> T const& { return m_buffer[index]; }
  private:
    auto dec(std::size_t& val) -> std::size_t {
        val = (val + m_max - 1) % m_max;
        return val;
    }
    auto calc_tail() -> std::size_t {
        return (m_head + m_max - m_count) % m_max;
    }

  private:
    T           m_buffer[SIZE];
    std::size_t m_max   = SIZE;
    std::size_t m_head  = 0;
    std::size_t m_count = 0;
};
}

constexpr auto PULSE_PIN = A2;

constexpr auto SCREEN_ADDRESS = 0x3C;
constexpr auto SCREEN_WIDTH   = 128;
constexpr auto SCREEN_HEIGHT  = 32;

Adafruit_SSD1306* screen;

constexpr std::size_t BUFFER_SIZE = 128;
nerv::buffer<std::uint16_t, BUFFER_SIZE> buffer;

std::int32_t on_time_count = 0;
hw_timer_t* timer          = nullptr;
portMUX_TYPE timer_mux     = portMUX_INITIALIZER_UNLOCKED;

auto IRAM_ATTR on_time() -> void {
    portENTER_CRITICAL_ISR(&timer_mux);
    on_time_count++;
    portEXIT_CRITICAL_ISR(&timer_mux);
}

auto setup() -> void {
    Serial.begin(115200);
    pinMode(PULSE_PIN, INPUT);
    analogSetAttenuation(ADC_11db);

    screen = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
    if (!screen->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("ERROR");
        for(;;);
    }

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, on_time, true);
    timerAlarmWrite(timer, 1'000'000 / 1'000, true);
    timerAlarmEnable(timer);
}

auto loop() -> void {
    if (on_time_count < 1) return;
    portENTER_CRITICAL(&timer_mux);
    on_time_count--;
    portEXIT_CRITICAL(&timer_mux);

    buffer.enq(analogRead(PULSE_PIN)); // max value is 12 bit

    screen->clearDisplay();
    auto it = buffer.begin();
    for (auto i = 0; i < BUFFER_SIZE; i++) {
        auto const value = float(*it++);
        auto const s = nerv::map(value, 0.0f, 2500.0f, 0.0f, 1.0f);
        auto y = s * -30.0f;
        screen->drawPixel(i, 32 + y, SSD1306_WHITE);
        if (it == buffer.end()) break;
    }
    screen->display();
}

