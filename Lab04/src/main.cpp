/**
 * @file main.cpp
 * @brief IIR filter implementation
 */
#include <array>
#include <cstdint>
#include <queue>

#include "esp_task_wdt.h"
#include "driver/dac.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "soc/dac_channel.h"
#include "esp32/rom/ets_sys.h"

extern "C" auto app_main() -> void;

constexpr auto M = 28;
static std::array<float, M + 1> b{
    0.01080096047,   0.009150882252,  0.007511904463,  0.0005792030715, -0.01127376128,
   -0.02515191026,  -0.03590095788,  -0.03739762306,  -0.02453046478,    0.004719638731,
    0.04788555577,   0.09797523171,   0.1449637711,    0.1784338504,     0.1905580908,
    0.1784338504,    0.1449637711,    0.09797523171,   0.04788555577,    0.004719638731,
   -0.02453046478,  -0.03739762306,  -0.03590095788,  -0.02515191026,   -0.01127376128,
    0.0005792030715, 0.007511904463,  0.009150882252,  0.01080096047
};
static std::array<float, M + 1> x{};

constexpr auto N = 1;
static std::array<float, N + 1> a{0.1};
static std::array<float, N + 1> y{};

constexpr std::uint32_t FREQUENCY = 10'000;
constexpr std::uint64_t US_ONE_S  = 1'000'000;
constexpr auto PIN                = GPIO_NUM_13;

static auto timer_callback(void *arg) -> void {
    static auto k = 0, l = 0;
    auto value = float(adc1_get_raw(ADC1_CHANNEL_0) / 16);
    x[k++] = value;
    if (k == M + 1) k = 0;

    auto sum = 0.0f;
    auto current = k;
    for (auto i = 0; i < M + 1; i++) {
        sum += b[i] * x[current];
        current = (current + (M + 1) - 1) % (M + 1);
    }
    dac_output_voltage(DAC_CHANNEL_1, uint8_t(sum));
}

auto app_main() -> void {
    adc_power_acquire();
    adc_gpio_init(adc_unit_t(ADC_UNIT_1), adc_channel_t(ADC1_CHANNEL_0));
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    gpio_pullup_en(GPIO_NUM_36);
    dac_output_enable(DAC_CHANNEL_1);

    gpio_config_t config;
    config.pin_bit_mask  = (uint64_t) 1 << PIN;
    config.mode          = gpio_mode_t(GPIO_MODE_DEF_OUTPUT);
    config.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en    = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&config));

    esp_timer_create_args_t periodConfig{};
    periodConfig.callback = timer_callback;
    periodConfig.name     = "sample_callback";

    esp_timer_handle_t periodicHandle;
    ESP_ERROR_CHECK(esp_timer_create(&periodConfig, &periodicHandle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodicHandle, US_ONE_S / FREQUENCY));
}


