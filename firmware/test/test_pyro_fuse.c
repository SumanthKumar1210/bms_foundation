/**
 * @file test_pyro_fuse.c
 * @brief Unit tests for the one-shot Pyro Fuse driver.
 */
#include "bms/pyro_fuse.h"

#include "hal_stub.h"
#include "unit_test.h"

static void test_fire_is_one_shot(void)
{
    hal_stub_reset();

    UT_ASSERT_EQ(PYRO_FUSE_ARMED, pyro_fuse_get_state());
    UT_ASSERT_EQ(BMS_OK, pyro_fuse_fire());
    UT_ASSERT(hal_stub.pyro_fuse_triggered);
    UT_ASSERT_EQ(PYRO_FUSE_FIRED, pyro_fuse_get_state());

    UT_ASSERT_EQ(BMS_E_STATE, pyro_fuse_fire());
    UT_ASSERT_EQ(PYRO_FUSE_FIRED, pyro_fuse_get_state());
}

int main(void)
{
    UT_RUN(test_fire_is_one_shot);
    return UT_REPORT();
}
