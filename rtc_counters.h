#pragma once
// Counters kept in RTC slow memory: survive deep sleep, reset on power loss.
#include "esp_attr.h"
#include "esp_bt.h"
#include "esp_sleep.h"

static RTC_DATA_ATTR uint32_t rtc_wake_count = 0;
static RTC_DATA_ATTR uint8_t rtc_ble_fail_count = 0;
