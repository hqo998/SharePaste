#pragma once

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace sharepaste::test
{
    struct TestCase
    {
        std::string name;
        std::function<void()> fn;
    };

    inline std::vector<TestCase> &registry()
    {
        static std::vector<TestCase> tests;
        return tests;
    }

    struct Registrar
    {
        Registrar(std::string_view name, std::function<void()> fn)
        {
            registry().push_back(TestCase{std::string(name), std::move(fn)});
        }
    };

    inline int runAllTests()
    {
        int passed = 0;
        int failed = 0;

        std::cout << "========================================\n";
        std::cout << "Running " << registry().size() << " test(s)...\n";
        std::cout << "========================================\n";

        for (const auto &test : registry())
        {
            try
            {
                test.fn();
                std::cout << "[ PASS ] " << test.name << "\n";
                ++passed;
            }
            catch (const std::exception &e)
            {
                std::cerr << "[ FAIL ] " << test.name << "\n"
                          << "         " << e.what() << "\n";
                ++failed;
            }
            catch (...)
            {
                std::cerr << "[ FAIL ] " << test.name << " (unknown exception)\n";
                ++failed;
            }
        }

        std::cout << "========================================\n";
        std::cout << "Results: " << passed << " passed, " << failed << " failed.\n";
        std::cout << "========================================\n";

        return failed == 0 ? 0 : 1;
    }
} // namespace sharepaste::test

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#define TEST_CASE(test_name)                                                              \
    static void CONCAT(test_fn_, __LINE__)();                                             \
    static ::sharepaste::test::Registrar CONCAT(registrar_, __LINE__)(                    \
        test_name, CONCAT(test_fn_, __LINE__));                                           \
    static void CONCAT(test_fn_, __LINE__)()

#define ASSERT_TRUE(condition)                                                            \
    do                                                                                    \
    {                                                                                     \
        if (!(condition))                                                                 \
        {                                                                                 \
            std::ostringstream oss;                                                       \
            oss << __FILE__ << ":" << __LINE__ << " ASSERT_TRUE(" #condition ") failed";  \
            throw std::runtime_error(oss.str());                                          \
        }                                                                                 \
    } while (0)

#define ASSERT_FALSE(condition) ASSERT_TRUE(!(condition))

#define ASSERT_EQ(actual, expected)                                                       \
    do                                                                                    \
    {                                                                                     \
        const auto &act_val = (actual);                                                   \
        const auto &exp_val = (expected);                                                 \
        if (!(act_val == exp_val))                                                        \
        {                                                                                 \
            std::ostringstream oss;                                                       \
            oss << __FILE__ << ":" << __LINE__ << " ASSERT_EQ(" #actual ", " #expected ") failed"; \
            throw std::runtime_error(oss.str());                                          \
        }                                                                                 \
    } while (0)

