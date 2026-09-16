#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <cmath>
#include <functional>
#include <vector>

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define ASSERT_TRUE(expr) do { g_tests_run++; \
    if (expr) { g_tests_passed++; } \
    else { g_tests_failed++; \
        std::cout << "  FAIL: " << __FILE__ << ":" << __LINE__ << " " << #expr << "\n"; } \
} while(0)

#define ASSERT_EQ(a, b) ASSERT_TRUE((a) == (b))
#define ASSERT_NE(a, b) ASSERT_TRUE((a) != (b))
#define ASSERT_GT(a, b) ASSERT_TRUE((a) > (b))
#define ASSERT_FLOAT_EQ(a, b, tol) ASSERT_TRUE(std::fabs((a)-(b)) < (tol))

#define TEST_SUITE(name) void name()
#define RUN_SUITE(name) do { std::cout << "--- " #name " ---\n"; name(); } while(0)

#define TEST_REPORT() do { \
    std::cout << "\n========================================\n" \
              << "Tests: " << g_tests_run << " | Passed: " << g_tests_passed \
              << " | Failed: " << g_tests_failed << "\n" \
              << "========================================\n"; \
} while(0)

#endif
