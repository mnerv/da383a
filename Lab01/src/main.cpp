#include <array>
#include <cstdint>

#include "esp_task_wdt.h"
#include "driver/dac.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "soc/dac_channel.h"
#include "esp32/rom/ets_sys.h"

extern "C" auto app_main() -> void;

constexpr std::uint32_t FREQUENCY = 10'000;
constexpr std::uint64_t US_ONE_S  = 1'000'000;

static auto timer_callback(void *arg) -> void {
    auto value = adc1_get_raw(ADC1_CHANNEL_0) / 16;
    dac_output_voltage(DAC_CHANNEL_1, uint8_t(value));
}

auto app_main() -> void {
    adc_power_acquire();
    adc_gpio_init(adc_unit_t(ADC_UNIT_1), adc_channel_t(ADC1_CHANNEL_0));
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    gpio_pullup_en(GPIO_NUM_36);
    dac_output_enable(DAC_CHANNEL_1);

    esp_timer_create_args_t periodConfig{};
    periodConfig.callback = timer_callback;
    periodConfig.name     = "periodic";

    esp_timer_handle_t periodicHandle;
    ESP_ERROR_CHECK(esp_timer_create(&periodConfig, &periodicHandle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodicHandle, US_ONE_S / FREQUENCY));
}

