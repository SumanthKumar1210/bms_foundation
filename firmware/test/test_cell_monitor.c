/**
 * @file test_cell_monitor.c
 * @brief Unit tests for cell_monitor.
 */
#include "bms/cell_monitor.h"

#include "hal_stub.h"
#include "unit_test.h"

static void setup(void)
{
    hal_stub_reset();
    (void)cell_monitor_init();
}

static void test_invalid_before_first_update(void)
{
    setup();
    UT_ASSERT(!cell_monitor_get()->valid);
}

static void test_statistics(void)
{
    const cell_monitor_data_t *d;

    setup();
    hal_stub.cell_mv[3] = 3600U;
    hal_stub.cell_mv[7] = 3900U;
    hal_stub.temp[1] = -50;
    hal_stub.temp[2] = 410;

    UT_ASSERT_EQ(BMS_OK, cell_monitor_update());
    d = cell_monitor_get();
    UT_ASSERT(d->valid);
    UT_ASSERT(!d->sensor_fault);
    UT_ASSERT_EQ(3600, d->min_mv);
    UT_ASSERT_EQ(3, d->min_cell);
    UT_ASSERT_EQ(3900, d->max_mv);
    UT_ASSERT_EQ(7, d->max_cell);
    UT_ASSERT_EQ((10U * 3700U) + 3600U + 3900U, d->pack_mv);
    UT_ASSERT_EQ(-50, d->min_temp);
    UT_ASSERT_EQ(410, d->max_temp);
}

static void test_implausible_voltage_keeps_last_values(void)
{
    const cell_monitor_data_t *d;

    setup();
    UT_ASSERT_EQ(BMS_OK, cell_monitor_update());
    hal_stub.cell_mv[0] = 6000U;

    UT_ASSERT_EQ(BMS_E_RANGE, cell_monitor_update());
    d = cell_monitor_get();
    UT_ASSERT(d->sensor_fault);
    UT_ASSERT(d->valid);
    UT_ASSERT_EQ(3700, d->max_mv);
}

static void test_hal_error_sets_sensor_fault(void)
{
    setup();
    hal_stub.read_result = BMS_E_HW;
    UT_ASSERT_EQ(BMS_E_HW, cell_monitor_update());
    UT_ASSERT(cell_monitor_get()->sensor_fault);
    UT_ASSERT(!cell_monitor_get()->valid);
}

int main(void)
{
    UT_RUN(test_invalid_before_first_update);
    UT_RUN(test_statistics);
    UT_RUN(test_implausible_voltage_keeps_last_values);
    UT_RUN(test_hal_error_sets_sensor_fault);
    return UT_REPORT();
}
