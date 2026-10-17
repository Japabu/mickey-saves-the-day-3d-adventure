// test.h - the port's headless tests: TEST(name) { ... CHECK(...); } in any file of
// port/tests/<Module>/, compiled with that module's flags and linked with the game's
// modules, none of them loaded (no DllMain runs; see port/flags.py, port_tests). port_tests [substring] runs the tests whose
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

// PROBE(name) { ... }: the port's side of a differential test (port/difftest): it gets
// the input bytes the test generated (in) and appends its results (out); the harness runs
// the original function on the same input in an emulator and compares.
// port_tests --probe name reads one hex line per case and writes one per result.
#include <stdint.h>
#include <string.h>
#include <vector>
namespace port_test {
typedef void (*ProbeFn)(const std::vector<uint8_t> &in, std::vector<uint8_t> &out);
struct Probe {
    const char *name;
    ProbeFn run;
    Probe *next;
};
void add_probe(Probe *p);
struct RegisterProbe {
    Probe p;
    RegisterProbe(const char *name, ProbeFn run) : p{name, run, 0} { add_probe(&p); }
};
// reads a value of type T at offset off of the input
template <class T> T get(const std::vector<uint8_t> &in, size_t off)
{
    T v;
    memcpy(&v, in.data() + off, sizeof(T));
    return v;
}
template <class T> void put(std::vector<uint8_t> &out, const T &v)
{
    const uint8_t *b = (const uint8_t *)&v;
    out.insert(out.end(), b, b + sizeof(T));
}
}  // namespace port_test

#define PROBE(name)                                                                              \
    static void port_probe_##name(const std::vector<uint8_t> &in, std::vector<uint8_t> &out);   \
    static port_test::RegisterProbe port_probe_reg_##name(#name, port_probe_##name);           \
    static void port_probe_##name(const std::vector<uint8_t> &in, std::vector<uint8_t> &out)

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
