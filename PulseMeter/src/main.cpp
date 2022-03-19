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

// Fixed size queue buffer, FIFO
template <typename T, std::size_t SIZE>
class queue {
  public:
    template <typename pointer_type, typename reference_type>
    struct iterator_base {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = pointer_type;
        using reference         = reference_type;

        constexpr iterator_base(pointer buffer, std::size_t const& ptr, std::size_t const& max, std::size_t const& end)
            : m_buffer(buffer), m_ptr(ptr), m_max(max), m_end(end) {}

        constexpr auto operator*() const -> reference { return m_buffer[m_ptr]; }
        constexpr auto operator->() -> pointer { return &m_buffer[m_ptr]; }

        constexpr auto operator++() -> iterator_base& {    // prefix increment
            m_ptr = (m_ptr + m_max - 1) % m_max;
            return *this;
        }
        constexpr auto operator++(int) -> iterator_base {  // postfix increment
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr auto operator--() -> iterator_base& {
            m_ptr = (m_ptr + 1) % m_max;
            return *this;
        }
        constexpr auto operator--(int) -> iterator_base {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        constexpr friend auto operator==(iterator_base const& a, iterator_base const& b) -> bool {
            return a.m_ptr == b.m_ptr;
        }
        constexpr friend auto operator!=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a == b);
        }

        // random access iterator requirements
        constexpr auto operator+=(difference_type const& n) -> iterator_base& {
            if ((n > 0 && n > m_max - 1) || (n < 0 && -n > m_max - 1))  // sets the iterator to end iterator
                m_ptr = (m_ptr + m_max + m_max - 1) % m_max;
            else                                                        // do normal addition with wrap
                m_ptr = (m_ptr + m_max + n) % m_max;
            return *this;
        }

        constexpr auto operator+(difference_type const& n) -> iterator_base {
            if ((n > 0 && n > m_max - 1) || (n < 0 && -n > m_max - 1))  // sets the iterator to end iterator
                m_ptr = (m_ptr + m_max + m_max - 1) % m_max;
            else                                                        // do normal addition with wrap
                m_ptr = (m_ptr + m_max + n) % m_max;
            return *this;
        }
        constexpr auto operator-(difference_type const& n) -> iterator_base {
            return (*this) + (-n);
        }

        constexpr auto operator-=(difference_type const& n) -> iterator_base& {
            return (*this) += (-n);
        }

        constexpr friend auto operator-(iterator_base const& a, iterator_base const& b) -> difference_type {
            // calcuate the distance to the end iterator
            auto a_end = a.dist_to_end();
            auto b_end = b.dist_to_end();
            return a_end - b_end;
        }

        constexpr auto operator[](difference_type const& n) -> reference {
            if ((n > 0 && n > m_max - 1) || (n < 0 && -n > m_max - 1)) // offset the pointer with wrap
                return m_buffer[(m_ptr + m_max + n) % m_max];
            else                                                       // deference the unused part of the memory,
                return m_buffer[(m_ptr + m_max + m_max - 1) % m_max];  // i.e the place where end iterator is pointed at
        }

        constexpr friend auto operator<(iterator_base const& a, iterator_base const& b) -> bool {
            return b - a > 0;
        }
        constexpr friend auto operator>(iterator_base const& a, iterator_base const& b) -> bool {
            return b < a;
        }
        constexpr friend auto operator>=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a < b);
        }
        constexpr friend auto operator<=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a > b);
        }

      private:
        // FIXME: Find a better way to get the distance from the current ptr to the end, complexity: O(n)
        auto dist_to_end() const -> difference_type {
            auto start = m_ptr;
            auto dist = 0;
            while(start != m_end) {
                dist++;
                start = (start + m_max - 1) % m_max;
            }
            return dist;
        }

      private:
        pointer     m_buffer;
        std::size_t m_ptr;
        std::size_t m_max;
        std::size_t m_end;
    };

    using iterator       = iterator_base<T*, T&>;
    using const_iterator = iterator_base<T const*, T const&>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    auto begin() -> iterator { return iterator(m_buffer, m_tail, m_max, m_head); }
    auto end()   -> iterator { return iterator(m_buffer, m_head, m_max, m_head); }
    auto cbegin() const -> const_iterator { return const_iterator(m_buffer, m_tail, m_max, m_head); }
    auto cend()   const -> const_iterator { return const_iterator(m_buffer, m_head, m_max, m_head); }

    auto rbegin() -> reverse_iterator { return reverse_iterator(end()); }
    auto rend()   -> reverse_iterator { return reverse_iterator(begin()); }
    auto crbegin() const -> const_reverse_iterator { return const_reverse_iterator(cend()); }
    auto crend()   const -> const_reverse_iterator { return const_reverse_iterator(cbegin()); }

  public:
    auto enq(T const& value) -> void {
        m_buffer[m_head] = value;
        if (queue::dec_wrap(m_head, m_max) == m_tail)
            queue::dec_wrap(m_tail, m_max);
        else
            ++m_size;
    }
    auto deq() -> T {
        auto const ret = m_buffer[m_tail];
        if (m_tail != m_head) {
            queue::dec_wrap(m_tail, m_max);
            --m_size;
        }
        return ret;
    }

    auto size() const -> std::size_t { return m_size; }
    auto back() -> T { return m_buffer[m_tail]; }
    auto front() -> T { return m_buffer[(m_head + 1) % m_max]; }

    using ref_type       = T&;
    using const_ref_type = T const&;
    auto operator[](std::size_t const& offset) -> ref_type { return m_buffer[(m_tail + m_max + offset) % m_max]; }
    auto operator[](std::size_t const& offset) const -> const_ref_type { return m_buffer[(m_tail + m_max + offset) % m_max]; }
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
nerv::queue<std::uint16_t, BUFFER_SIZE> buffer;

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
    for (auto i = 0; i < SCREEN_WIDTH; i++) {
        auto const value = float(*it++);
        auto const s = nerv::map(value, 0.0f, 4095.0f, 0.0f, 1.0f);
        auto y = s * 32.0f;
        screen->drawPixel(SCREEN_WIDTH - i, 32 - y, SSD1306_WHITE);
    }
    screen->setCursor(90, 0);
    screen->setTextSize(1);
    screen->setTextColor(SSD1306_WHITE);
    screen->printf("%.2f", ONE_SECOND_US / float(current_time - last_beat));
    screen->display();
}

