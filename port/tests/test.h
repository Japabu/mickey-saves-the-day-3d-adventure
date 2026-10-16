// test.h - the port's headless tests: TEST(name) { ... CHECK(...); } in any file of
// port/tests/<Module>/, compiled with that module's flags and linked against the game's
// libraries (see port/flags.py, port_tests). port_tests [substring] runs the tests whose
// name contains the substring (all without one) and exits non-zero on a failure.
#ifndef PORT_TESTS_TEST_H
#define PORT_TESTS_TEST_H
#include <math.h>
#include <stdio.h>

namespace port_test {
struct Test {
    const char *name;
    void (*run)();
    Test *next;
};
void add(Test *t);
void fail(const char *file, int line, const char *what);
struct Register {
    Test t;
    Register(const char *name, void (*run)()) : t{name, run, 0} { add(&t); }
};
}  // namespace port_test

#define TEST(name)                                                                \
    static void port_test_##name();                                               \
    static port_test::Register port_test_reg_##name(#name, port_test_##name);     \
    static void port_test_##name()

#define CHECK(cond)                                         \
    do {                                                    \
        if (!(cond))                                        \
            port_test::fail(__FILE__, __LINE__, #cond);     \
    } while (0)

// a float that is a number and within eps of the expected one
#define CHECK_NEAR(value, expected, eps)                                                   \
    do {                                                                                   \
        double port_v = (value), port_e = (expected);                                      \
        if (!(fabs(port_v - port_e) <= (eps))) {                                           \
            char port_msg[256];                                                            \
            snprintf(port_msg, sizeof(port_msg), "%s = %g, expected %g", #value, port_v,   \
                     port_e);                                                              \
            port_test::fail(__FILE__, __LINE__, port_msg);                                 \
        }                                                                                  \
    } while (0)

#endif
