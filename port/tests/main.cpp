// main.cpp - runs the tests that port/tests/*/ registered (see test.h).
#include "test.h"
#include <string.h>

namespace port_test {
static Test *s_first, **s_last = &s_first;
static int s_failures;

void add(Test *t)
{
    *s_last = t;
    s_last = &t->next;
}

void fail(const char *file, int line, const char *what)
{
    const char *base = strrchr(file, '/');
    printf("    FAIL %s:%d: %s\n", base ? base + 1 : file, line, what);
    s_failures++;
}
}  // namespace port_test

int main(int argc, char **argv)
{
    using namespace port_test;
    const char *filter = argc > 1 ? argv[1] : 0;
    int run = 0, failed = 0;
    for (Test *t = s_first; t; t = t->next) {
        if (filter && !strstr(t->name, filter))
            continue;
        int before = s_failures;
        t->run();
        run++;
        bool ok = s_failures == before;
        failed += !ok;
        printf("%s %s\n", ok ? "ok  " : "FAIL", t->name);
    }
    printf("%d test%s, %d failed\n", run, run == 1 ? "" : "s", failed);
    return failed ? 1 : 0;
}
