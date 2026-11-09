#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <httplib.h>
#include <sqlite3.h>

#include "test_framework.h"
#include <dbmanager.h>
#include <ratelimiter.h>
#include <utility.h>

// ============================================================================
// Example Smoke Test (shows how TEST_CASE, ASSERT_TRUE, and ASSERT_EQ work)
// ============================================================================
TEST_CASE("utility::generateRandomString returns requested length")
{
    std::string code = sharepaste::generateRandomString(15);
    ASSERT_EQ(code.size(), 15u);
}

// ============================================================================
// 1. Utility Tests (src/backend/utility/utility.cpp)
//    Target functions: trimLeadingChar, stringToSplitArray, fetchEnvInt, etc.
// ============================================================================

// TODO: Add your utility test cases here!


// ============================================================================
// 2. RateLimiter Tests (src/backend/ratelimiter/ratelimiter.cpp)
//    Target classes: TokenBucket, IpRateLimiter
// ============================================================================

// TODO: Add your rate limiter test cases here!


// ============================================================================
// 3. Database Manager Tests (src/backend/dbmanager/dbmanager.cpp)
//    Target class: managerSQL (Tip: use ":memory:" for fast, isolated SQLite DBs)
// ============================================================================

// TODO: Add your database test cases here!


int main()
{
    return sharepaste::test::runAllTests();
}

