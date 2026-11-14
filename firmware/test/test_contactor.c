/**
 * @file test_contactor.c
 * @brief Unit tests for contactor sequencing.
 */
#include "bms/contactor.h"

#include "bms/bms_config.h"
#include "hal_stub.h"
#include "unit_test.h"

#define PACK_MV (44400U)

static void setup(void)
{
    hal_stub_reset();
    (void)contactor_init();
}

static void test_close_sequence(void)
{
    setup();
    UT_ASSERT_EQ(BMS_OK, contactor_request_close(0U));
    UT_ASSERT_EQ(CONTACTOR_PRECHARGE, contactor_get_state());
    UT_ASSERT(hal_stub.contactor[HAL_CONTACTOR_MAIN_NEG]);
    UT_ASSERT(hal_stub.contactor[HAL_CONTACTOR_PRECHARGE]);
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_MAIN_POS]);

    hal_stub.link_mv = (PACK_MV * BMS_PRECHARGE_DONE_PERCENT) / 100U;
    UT_ASSERT_EQ(BMS_OK, contactor_update(PACK_MV, 100U));
    UT_ASSERT_EQ(CONTACTOR_CLOSED, contactor_get_state());
    UT_ASSERT(hal_stub.contactor[HAL_CONTACTOR_MAIN_POS]);
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_PRECHARGE]);
}

static void test_precharge_timeout(void)
{
    setup();
    (void)contactor_request_close(0U);
    hal_stub.link_mv = PACK_MV / 2U;
    UT_ASSERT_EQ(BMS_OK, contactor_update(PACK_MV, BMS_PRECHARGE_TIMEOUT_MS - 1U));
    UT_ASSERT_EQ(CONTACTOR_PRECHARGE, contactor_get_state());
    (void)contactor_update(PACK_MV, BMS_PRECHARGE_TIMEOUT_MS);
    UT_ASSERT_EQ(CONTACTOR_FAULT, contactor_get_state());
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_MAIN_NEG]);
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_PRECHARGE]);
}

static void test_fault_needs_reset(void)
{
    setup();
    (void)contactor_request_close(0U);
    (void)contactor_update(PACK_MV, BMS_PRECHARGE_TIMEOUT_MS);
    UT_ASSERT_EQ(BMS_E_STATE, contactor_request_close(1000U));
    UT_ASSERT_EQ(BMS_OK, contactor_reset());
    UT_ASSERT_EQ(CONTACTOR_OPEN, contactor_get_state());
    UT_ASSERT_EQ(BMS_E_STATE, contactor_reset());
}

static void test_open_from_closed(void)
{
    setup();
    (void)contactor_request_close(0U);
    hal_stub.link_mv = PACK_MV;
    (void)contactor_update(PACK_MV, 10U);
    UT_ASSERT_EQ(BMS_OK, contactor_request_open());
    UT_ASSERT_EQ(CONTACTOR_OPEN, contactor_get_state());
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_MAIN_POS]);
    UT_ASSERT(!hal_stub.contactor[HAL_CONTACTOR_MAIN_NEG]);
}

int main(void)
{
    UT_RUN(test_close_sequence);
    UT_RUN(test_precharge_timeout);
    UT_RUN(test_fault_needs_reset);
    UT_RUN(test_open_from_closed);
    return UT_REPORT();
}
