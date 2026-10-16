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

static Probe *s_probes;
void add_probe(Probe *p)
{
    p->next = s_probes;
    s_probes = p;
}
}  // namespace port_test

// --probe name: hex input lines on stdin, hex result lines on stdout (port/difftest)
static int run_probe(const char *name)
{
    using namespace port_test;
    Probe *p = s_probes;
    while (p && strcmp(p->name, name))
        p = p->next;
    if (!p) {
        fprintf(stderr, "no probe %s\n", name);
        return 2;
    }
    char line[1 << 16];
    while (fgets(line, sizeof(line), stdin)) {
        std::vector<uint8_t> in, out;
        for (const char *c = line; c[0] && c[1] && c[0] != '\n'; c += 2) {
            unsigned v;
            sscanf(c, "%2x", &v);
            in.push_back((uint8_t)v);
        }
        p->run(in, out);
        for (uint8_t b : out)
            printf("%02x", b);
        printf("\n");
        fflush(stdout);
    }
    return 0;
}

int main(int argc, char **argv)
{
    using namespace port_test;
    if (argc > 2 && !strcmp(argv[1], "--probe"))
        return run_probe(argv[2]);
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
