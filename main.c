#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "tusb.h"

// -----------------------------------------------------------------------------
// HID joystick report (3 axes, 16-bit each)
// -----------------------------------------------------------------------------
typedef struct __attribute__((packed)) {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} joystick_report_t;

// -----------------------------------------------------------------------------
// LED pins for landing gear indicators
// -----------------------------------------------------------------------------
#define LED_GEAR_DOWN     2   // Green
#define LED_GEAR_UP       3   // Red
#define LED_GEAR_TRANSIT  4   // Amber

void init_gear_leds() {
    gpio_init(LED_GEAR_DOWN);
    gpio_set_dir(LED_GEAR_DOWN, GPIO_OUT);

    gpio_init(LED_GEAR_UP);
    gpio_set_dir(LED_GEAR_UP, GPIO_OUT);

    gpio_init(LED_GEAR_TRANSIT);
    gpio_set_dir(LED_GEAR_TRANSIT, GPIO_OUT);
}

void update_gear_leds(int state) {
    // 0 = up, 1 = down, 2 = in transit
    gpio_put(LED_GEAR_DOWN,    state == 1);
    gpio_put(LED_GEAR_UP,      state == 0);
    gpio_put(LED_GEAR_TRANSIT, state == 2);
}

// -----------------------------------------------------------------------------
// ADC helpers (oversampling + scaling)
// -----------------------------------------------------------------------------

// 4× oversampling — perfect for 10k slide pots
static inline uint16_t read_adc_oversampled(uint input) {
    adc_select_input(input);

    uint32_t sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += adc_read();   // 0–4095
    }
    return sum / 4;
}

// Convert 12-bit ADC → 16-bit HID range
static inline uint16_t scale(uint16_t v) {
    return (v * 32767) / 4095;
}

// -----------------------------------------------------------------------------
// Light smoothing (fast + stable)
// -----------------------------------------------------------------------------
static uint16_t fx, fy, fz;

static inline uint16_t smooth(uint16_t prev, uint16_t current) {
    // 90% new, 10% old — responsive and stable
    return (prev * 1 + current * 9) / 10;
}

// -----------------------------------------------------------------------------
// Send HID joystick report
// -----------------------------------------------------------------------------
void send_report(void) {
    if (!tud_hid_ready()) return;

    uint16_t rx = scale(read_adc_oversampled(0));
    uint16_t ry = scale(read_adc_oversampled(1));
    uint16_t rz = scale(read_adc_oversampled(2));

    fx = smooth(fx, rx);
    fy = smooth(fy, ry);
    fz = smooth(fz, rz);

    joystick_report_t r = { fx, fy, fz };
    tud_hid_report(0, &r, sizeof(r));
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main() {
    stdio_init_all();   // enables USB serial for X‑Plane

    // ADC setup
    adc_init();
    adc_gpio_init(26);  // X axis
    adc_gpio_init(27);  // Y axis
    adc_gpio_init(28);  // Z axis

    // Seed smoothing filters with real values
    fx = scale(read_adc_oversampled(0));
    fy = scale(read_adc_oversampled(1));
    fz = scale(read_adc_oversampled(2));

    // Init LEDs
    init_gear_leds();

    // Init USB HID
    tusb_init();

    while (1) {
        tud_task();      // USB HID
        send_report();   // joystick update

        // Listen for gear messages from X‑Plane ("G0", "G1", "G2")
        int c = getchar_timeout_us(0);
        if (c == 'G') {
            int s = getchar_timeout_us(0);
            if (s >= '0' && s <= '2') {
                update_gear_leds(s - '0');
            }
        }

        sleep_ms(5);
    }
}
