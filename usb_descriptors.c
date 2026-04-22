#include "tusb.h"

// -----------------------------------------------------------------------------
// HID Report Descriptor (3 axes, 16-bit each)
// -----------------------------------------------------------------------------

uint8_t const hid_report_descriptor[] = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x04,       // Usage (Joystick)
    0xA1, 0x01,       // Collection (Application)

    // 3 axes, 16-bit each
    0x15, 0x00,       // Logical Min 0
    0x26, 0xFF, 0x7F, // Logical Max 32767
    0x75, 0x10,       // Report Size 16 bits
    0x95, 0x03,       // Report Count 3
    0x09, 0x30,       // X
    0x09, 0x31,       // Y
    0x09, 0x32,       // Z
    0x81, 0x02,       // Input (Data,Var,Abs)

    0xC0              // End Collection
};

// -----------------------------------------------------------------------------
// Device Descriptor
// -----------------------------------------------------------------------------

tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0xCafe,   // Example VID
    .idProduct          = 0x4000,   // Example PID
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&desc_device;
}

// -----------------------------------------------------------------------------
// Configuration Descriptor
// -----------------------------------------------------------------------------

enum {
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define EPNUM_HID   0x81   // Endpoint 1 IN
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] = {
    // Config descriptor
    TUD_CONFIG_DESCRIPTOR(
        1,                    // configuration number
        ITF_NUM_TOTAL,        // total interfaces
        0,                    // string index
        CONFIG_TOTAL_LEN,     // total length
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
        100                   // power in mA
    ),

    // HID descriptor (NEW API — 7 arguments)
    TUD_HID_DESCRIPTOR(
        ITF_NUM_HID,              // interface number
        0,                        // string index
        HID_ITF_PROTOCOL_NONE,    // no boot protocol
        sizeof(hid_report_descriptor),
        EPNUM_HID,                // endpoint IN
        16,                       // endpoint size
        1                         // polling interval (ms)
    )
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_configuration;
}


// -----------------------------------------------------------------------------
// HID Report Descriptor Callback
// -----------------------------------------------------------------------------

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return hid_report_descriptor;
}

// -----------------------------------------------------------------------------
// String Descriptors
// -----------------------------------------------------------------------------

char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: English (US)
    "Catlin",           // 1: Manufacturer
    "Cessna Throtle",            // 2: Product
    "123456",                   // 3: Serial
};

static uint16_t _desc_str[32];

uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        const char *str = string_desc_arr[index];
        chr_count = strlen(str);

        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return _desc_str;
}

// -----------------------------------------------------------------------------
// HID Get/Set Report Callbacks
// -----------------------------------------------------------------------------

uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
