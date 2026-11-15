/**
 * @file test_fault_manager.c
 * @brief Unit tests for fault_manager.
 */
#include "bms/fault_manager.h"

#include "bms/bms_config.h"
#include "unit_test.h"

static cell_monitor_data_t healthy(void)
{
    cell_monitor_data_t d = {0};

    d.min_mv = 3600U;
    d.max_mv = 3700U;
    d.min_temp = 200;
    d.max_temp = 300;
    d.valid = true;
    return d;
}

static void test_null_data(void)
{
    (void)fault_manager_init();
    UT_ASSERT_EQ(BMS_E_NULL_PTR, fault_manager_update(NULL, 0U));
}

static void test_overvoltage_is_debounced(void)
{
    cell_monitor_data_t d = healthy();

    (void)fault_manager_init();
    d.max_mv = BMS_CELL_OV_MV + 1U;
    (void)fault_manager_update(&d, 1000U);
    (void)fault_manager_update(&d, 1000U + BMS_FAULT_DEBOUNCE_MS - 1U);
    UT_ASSERT(!fault_manager_is_active(FAULT_CELL_OV));
    (void)fault_manager_update(&d, 1000U + BMS_FAULT_DEBOUNCE_MS);
    UT_ASSERT(fault_manager_is_active(FAULT_CELL_OV));
    UT_ASSERT_EQ(1U << FAULT_CELL_OV, fault_manager_active_mask());
}

static void test_short_glitch_does_not_latch(void)
{
    cell_monitor_data_t d = healthy();

    (void)fault_manager_init();
    d.min_mv = BMS_CELL_UV_MV - 1U;
    (void)fault_manager_update(&d, 0U);
    d.min_mv = 3600U;
    (void)fault_manager_update(&d, 50U);
    d.min_mv = BMS_CELL_UV_MV - 1U;
    (void)fault_manager_update(&d, 60U);
    (void)fault_manager_update(&d, 60U + BMS_FAULT_DEBOUNCE_MS - 1U);
    UT_ASSERT(!fault_manager_is_active(FAULT_CELL_UV));
}

static void test_fault_stays_latched_until_cleared(void)
{
    cell_monitor_data_t d = healthy();

    (void)fault_manager_init();
    d.max_temp = BMS_CELL_OT_DDEGC + 1;
    (void)fault_manager_update(&d, 0U);
    (void)fault_manager_update(&d, BMS_FAULT_DEBOUNCE_MS);
    UT_ASSERT_EQ(BMS_E_STATE, fault_manager_clear());

    d.max_temp = 300;
    (void)fault_manager_update(&d, BMS_FAULT_DEBOUNCE_MS + 10U);
    UT_ASSERT(fault_manager_is_active(FAULT_CELL_OT));
    UT_ASSERT_EQ(BMS_OK, fault_manager_clear());
    UT_ASSERT(!fault_manager_is_active(FAULT_CELL_OT));
}

static void test_invalid_data_sets_sensor_fault_only(void)
{
    cell_monitor_data_t d = healthy();

    (void)fault_manager_init();
    d.sensor_fault = true;
    d.max_mv = BMS_CELL_OV_MV + 100U; /* ignored: data not trusted */
    (void)fault_manager_update(&d, 0U);
    (void)fault_manager_update(&d, BMS_FAULT_DEBOUNCE_MS);
    UT_ASSERT(fault_manager_is_active(FAULT_SENSOR));
    UT_ASSERT(!fault_manager_is_active(FAULT_CELL_OV));
}

static void test_debounce_across_timer_wrap(void)
{
    cell_monitor_data_t d = healthy();

    (void)fault_manager_init();
    d.max_mv = BMS_CELL_OV_MV + 1U;
    (void)fault_manager_update(&d, 0xFFFFFFF0U);
    (void)fault_manager_update(&d, (bms_ms_t)(0xFFFFFFF0U + BMS_FAULT_DEBOUNCE_MS));
    UT_ASSERT(fault_manager_is_active(FAULT_CELL_OV));
}

int main(void)
{
    UT_RUN(test_null_data);
    UT_RUN(test_overvoltage_is_debounced);
    UT_RUN(test_short_glitch_does_not_latch);
    UT_RUN(test_fault_stays_latched_until_cleared);
    UT_RUN(test_invalid_data_sets_sensor_fault_only);
    UT_RUN(test_debounce_across_timer_wrap);
    return UT_REPORT();
}
