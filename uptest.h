#ifndef UPTEST_H
#define UPTEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

// Uncomment this line to disable colorized output
// #define NO_COLOR_OUTPUT

// ANSI color codes for terminal output
#ifndef NO_COLOR_OUTPUT
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#else
#define COLOR_GREEN   ""
#define COLOR_RED     ""
#define COLOR_RESET   ""
#define COLOR_BOLD    ""
#define COLOR_YELLOW  ""
#define COLOR_CYAN    ""
#endif

// TAP version output
static void uptest_tap_version(void)
{
    printf("TAP version 14\n");
}

// Counters for test results
static int uptest_total_tests = 0;              // Total tests (unit + prop)
static int uptest_passed_tests = 0;
static int uptest_failed_tests = 0;
static int uptest_total_assertions = 0;
static int uptest_passed_assertions = 0;
static int uptest_failed_assertions = 0;
static int current_assertion_count = 0;         // Counter for assertions within the current unit test
static int current_test_number = 0;             // Counter for test numbers

// Internal function to print TAP-compliant output for assertions
static void uptest_tap_output_assert(int passed, const char *message)
{
    if (passed)
        printf("    %sok%s %d - %s\n", COLOR_GREEN, COLOR_RESET, current_assertion_count, message);
    else
        printf("    %snot ok%s %d - %s\n", COLOR_RED, COLOR_RESET, current_assertion_count, message);
}

// Internal function to print TAP-compliant output for tests
static void uptest_tap_output_test(const char *name, int passed)
{
    if (passed)
        printf("%sok%s %d - %s\n", COLOR_GREEN, COLOR_RESET, current_test_number, name);
    else
        printf("%snot ok%s %d - %s\n", COLOR_RED, COLOR_RESET, current_test_number, name);
}

// Reset counters for a new test
static void uptest_reset_assertion_counter()
{
    current_assertion_count = 0;
}

// Increment test counters
static void uptest_increment_test_counters(int passed)
{
    uptest_total_tests++;
    if (passed)
        uptest_passed_tests++;
    else
        uptest_failed_tests++;
}

// Assertions for unit testing
#define UT_ASSERT(COND) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if (COND) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, #COND); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, #COND); \
    } \
} while (0)

#define UT_ASSERTm(MSG, COND) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if (COND) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, MSG); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, MSG); \
    } \
} while (0)

#define UT_ASSERT_EQ(EXPECTED, ACTUAL) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if ((EXPECTED) == (ACTUAL)) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, "ASSERT_EQ"); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, "ASSERT_EQ"); \
        printf("    # Expected: %d, Got: %d\n", (int)(EXPECTED), (int)(ACTUAL)); \
    } \
} while (0)

#define UT_ASSERT_NEQ(EXPECTED, ACTUAL) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if ((EXPECTED) != (ACTUAL)) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, "ASSERT_NEQ"); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, "ASSERT_NEQ"); \
        printf("    # Expected not: %d, Got: %d\n", (int)(EXPECTED), (int)(ACTUAL)); \
    } \
} while (0)

#define UT_ASSERT_GT(EXPECTED, ACTUAL) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if ((EXPECTED) > (ACTUAL)) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, "ASSERT_GT"); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, "ASSERT_GT"); \
        printf("    # Expected greater than: %d, Got: %d\n", (int)(EXPECTED), (int)(ACTUAL)); \
    } \
} while (0)

#define UT_ASSERT_LT(EXPECTED, ACTUAL) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if ((EXPECTED) < (ACTUAL)) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, "ASSERT_LT"); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, "ASSERT_LT"); \
        printf("    # Expected less than: %d, Got: %d\n", (int)(EXPECTED), (int)(ACTUAL)); \
    } \
} while (0)

#define UT_ASSERT_STR_EQ(EXPECTED, ACTUAL) do { \
    current_assertion_count++; \
    uptest_total_assertions++; \
    if (strcmp((EXPECTED), (ACTUAL)) == 0) { \
        uptest_passed_assertions++; \
        uptest_tap_output_assert(1, "ASSERT_STR_EQ"); \
    } else { \
        uptest_failed_assertions++; \
        uptest_tap_output_assert(0, "ASSERT_STR_EQ"); \
        printf("    # Expected: %s, Got: %s\n", (EXPECTED), (ACTUAL)); \
    } \
} while (0)

#define UT_ASSERT_OR_LONGJMP(COND) do { \
    if (!(COND)) { \
        longjmp(uptest_jmp_buf, 1); \
    } \
} while (0)

// Property test result enumeration
typedef enum {
    PROP_PASS,
    PROP_FAIL,
    PROP_ERROR
} prop_result;

// Function signature for property test
typedef prop_result (*property_func)(void *);

// Macro for defining a simple property test
#define PROP_TEST(NAME, FUNC) \
    static void NAME(void) { \
        current_test_number++; \
        uptest_reset_assertion_counter(); \
        prop_result result = FUNC(NULL); \
        printf("    1..%d\n", current_assertion_count); \
        if (result == PROP_PASS) { \
            uptest_increment_test_counters(1); \
            uptest_tap_output_test(#NAME, 1); \
        } else { \
            uptest_increment_test_counters(0); \
            uptest_tap_output_test(#NAME, 0); \
        } \
    }

// Function signature for setup, test, and teardown in complex property tests
typedef void * (*prop_setup_func)(void);
typedef prop_result (*prop_run_func)(void *);
typedef void (*prop_teardown_func)(void *);

// Struct for a complex property test
typedef struct {
    const char *            name;
    prop_setup_func         setup;
    prop_run_func           run;
    prop_teardown_func      teardown;
} complex_property_test;

// Macro for defining a complex property test
#define COMPLEX_PROP_TEST(NAME, SETUP, RUN, TEARDOWN) \
    static void NAME(void) { \
        current_test_number++; \
        uptest_reset_assertion_counter(); \
        void *env = SETUP(); \
        if (!env) { \
            uptest_increment_test_counters(0); \
            uptest_tap_output_test(#NAME, 0); \
            printf("# Setup failed for test %s\n", #NAME); \
            return; \
        } \
        prop_result result = RUN(env); \
        printf("    1..%d\n", current_assertion_count); \
        if (result == PROP_PASS) { \
            uptest_increment_test_counters(1); \
            uptest_tap_output_test(#NAME, 1); \
        } else { \
            uptest_increment_test_counters(0); \
            uptest_tap_output_test(#NAME, 0); \
        } \
        TEARDOWN(env); \
    }

// Unit test result enumeration
typedef enum {
    UNIT_PASS,
    UNIT_FAIL
} unit_result;

// Function signature for unit test
typedef unit_result (*unit_func)(void);

// Macro for defining a simple unit test
#define UNIT_TEST(NAME, FUNC) \
    static void NAME(void) { \
        current_test_number++; \
        uptest_reset_assertion_counter(); \
        unit_result result = FUNC(); \
        printf("    1..%d\n", current_assertion_count); \
        if (result == UNIT_PASS) { \
            uptest_increment_test_counters(1); \
            uptest_tap_output_test(#NAME, 1); \
        } else { \
            uptest_increment_test_counters(0); \
            uptest_tap_output_test(#NAME, 0); \
        } \
    }

// Macro to run all property tests
#define RUN_PROP_TESTS(...) \
    do { \
        void (*tests[])(void) = { __VA_ARGS__ }; \
        size_t num_tests = sizeof(tests) / sizeof(tests[0]); \
        for (size_t i = 0; i < num_tests; i++) { \
            tests[i](); \
        } \
    } while (0)

// Macro to run all unit tests
#define RUN_UNIT_TESTS(...) \
    do { \
        uptest_tap_version(); \
        void (*tests[])(void) = { __VA_ARGS__ }; \
        size_t num_tests = sizeof(tests) / sizeof(tests[0]); \
        for (size_t i = 0; i < num_tests; i++) { \
            tests[i](); \
        } \
        printf("1..%zu\n", num_tests); \
    } while (0)

// Function to summarize the test results
static void uptest_summary(void)
{
    printf("\n%sTotal tests: %s%d\n", COLOR_BOLD, COLOR_RESET, uptest_total_tests);
    printf("%sPassed tests: %s%d\n", COLOR_GREEN, COLOR_RESET, uptest_passed_tests);
    printf("%sFailed tests: %s%d\n\n", COLOR_RED, COLOR_RESET, uptest_failed_tests);
    printf("%sTotal assertions: %s%d\n", COLOR_BOLD, COLOR_RESET, uptest_total_assertions);
    printf("%sPassed assertions: %s%d\n", COLOR_GREEN, COLOR_RESET, uptest_passed_assertions);
    printf("%sFailed assertions: %s%d\n", COLOR_RED, COLOR_RESET, uptest_failed_assertions);
}

// Reset test counters
static void uptest_reset(void)
{
    uptest_total_tests = 0;
    uptest_passed_tests = 0;
    uptest_failed_tests = 0;
    uptest_total_assertions = 0;
    uptest_passed_assertions = 0;
    uptest_failed_assertions = 0;
    current_assertion_count = 0;
    current_test_number = 0;
}

#endif // UPTEST_H
