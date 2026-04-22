#pragma once

#define CFG_TUSB_MCU OPT_MCU_RP2040
#define CFG_TUSB_OS  OPT_OS_PICO

// Enable device stack
#define CFG_TUD_ENABLED 1
#define CFG_TUD_HID     1

// Root hub port mode (Pico uses port 0 as device)
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)
#define CFG_TUSB_RHPORT1_MODE 0

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE 64

// Disable unused classes
#define CFG_TUD_CDC     0
#define CFG_TUD_MSC     0
#define CFG_TUD_MIDI    0
#define CFG_TUD_VENDOR  0
