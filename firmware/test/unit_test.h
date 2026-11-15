/**
 * @file unit_test.h
 * @brief Minimal unit test framework for host builds (no external dependency).
 *
 * Usage:
 *     static void test_something(void) { UT_ASSERT_EQ(1, 1); }
 *     int main(void) { UT_RUN(test_something); return UT_REPORT(); }
 */
#ifndef BMS_UNIT_TEST_H
#define BMS_UNIT_TEST_H

#include <stdio.h>

static int ut_failures = 0;
static int ut_tests = 0;

/** Fail the current test if @p cond is false (continues running). */
#define UT_ASSERT(cond)                                                   \
    do                                                                    \
    {                                                                     \
        if (!(cond))                                                      \
        {                                                                 \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            ut_failures++;                                                \
        }                                                                 \
    } while (0)

/** Fail if the two integer expressions differ; prints both values. */
#define UT_ASSERT_EQ(expected, actual)                                                        \
    do                                                                                        \
    {                                                                                         \
        long long ut_e = (long long)(expected);                                               \
        long long ut_a = (long long)(actual);                                                 \
        if (ut_e != ut_a)                                                                     \
        {                                                                                     \
            printf("  FAIL %s:%d: %s == %s (%lld != %lld)\n", __FILE__, __LINE__, #expected, \
                   #actual, ut_e, ut_a);                                                      \
            ut_failures++;                                                                    \
        }                                                                                     \
    } while (0)

/** Run one test function. */
#define UT_RUN(test_fn)                \
    do                                 \
    {                                  \
        ut_tests++;                    \
        printf("RUN  %s\n", #test_fn); \
        test_fn();                     \
    } while (0)

/** Print a summary; evaluates to the process exit code (0 = all passed). */
#define UT_REPORT() (printf("%d tests, %d failures\n", ut_tests, ut_failures), (ut_failures == 0) ? 0 : 1)

#endif /* BMS_UNIT_TEST_H */
