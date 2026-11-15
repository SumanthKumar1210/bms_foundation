/**
 * @file test_balancing.c
 * @brief Unit tests for passive balancing.
 */
#include "bms/balancing.h"

#include "bms/bms_config.h"
#include "hal_stub.h"
#include "unit_test.h"

static cell_monitor_data_t pack_at(bms_millivolt_t mv)
{
    cell_monitor_data_t d = {0};
    uint8_t i;

    for (i = 0U; i < BMS_CELL_COUNT; i++)
    {
        d.cell_mv[i] = mv;
    }
    d.min_mv = mv;
    d.max_mv = mv;
    d.valid = true;
    return d;
}

static void test_balanced_pack_needs_nothing(void)
{
    cell_monitor_data_t d = pack_at(3900U);
    UT_ASSERT_EQ(0, balancing_compute_mask(&d, false));
}

static void test_high_cells_are_balanced(void)
{
    cell_monitor_data_t d = pack_at(3900U);

    d.cell_mv[2] = 3900U + BMS_BALANCE_DELTA_MV + 1U;
    d.cell_mv[5] = 3900U + BMS_BALANCE_DELTA_MV; /* exactly at the threshold: not balanced */
    d.max_mv = d.cell_mv[2];
    UT_ASSERT_EQ(1U << 2, balancing_compute_mask(&d, false));
}

static void test_no_balancing_below_minimum_voltage(void)
{
    cell_monitor_data_t d = pack_at(BMS_BALANCE_MIN_MV - 100U);

    d.cell_mv[0] = BMS_BALANCE_MIN_MV - 1U;
    UT_ASSERT_EQ(0, balancing_compute_mask(&d, false));
}

static void test_disabled_on_fault_or_invalid_data(void)
{
    cell_monitor_data_t d = pack_at(3900U);

    d.cell_mv[1] = 4000U;
    UT_ASSERT_EQ(0, balancing_compute_mask(&d, true));
    d.valid = false;
    UT_ASSERT_EQ(0, balancing_compute_mask(&d, false));
    UT_ASSERT_EQ(0, balancing_compute_mask(NULL, false));
}

static void test_update_applies_mask(void)
{
    cell_monitor_data_t d = pack_at(3900U);

    hal_stub_reset();
    d.cell_mv[4] = 4000U;
    UT_ASSERT_EQ(BMS_OK, balancing_update(&d, false));
    UT_ASSERT_EQ(1U << 4, hal_stub.balancing_mask);
    UT_ASSERT_EQ(1U << 4, balancing_active_mask());
}

int main(void)
{
    UT_RUN(test_balanced_pack_needs_nothing);
    UT_RUN(test_high_cells_are_balanced);
    UT_RUN(test_no_balancing_below_minimum_voltage);
    UT_RUN(test_disabled_on_fault_or_invalid_data);
    UT_RUN(test_update_applies_mask);
    return UT_REPORT();
}
