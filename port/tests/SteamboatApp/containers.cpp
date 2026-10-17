// The game's containers as the port gives them (MBContainers.h, port/edits/SteamboatApp.py).
#include "../test.h"
#include "MBContainers.h"

// SBTrashBashNode::ClearBoard erases at a pointer it keeps until begin() == end(). In VC6
// an emptied vector keeps its buffer, so the loop stops after the last element; when the
// port's end() turned into 0 there, the loop ran on into freed memory (the trash game
// crashed after a level).
TEST(vector_erase_at_kept_pointer_stops)
{
    MBVector<int *> v;
    int a = 0, b = 0, c = 0;
    v.push_back(&a);
    v.push_back(&b);
    v.push_back(&c);
    int **p = v.begin();
    int erased = 0;
    while (p != v.end() && erased < 10) {
        v.erase(p);
        erased++;
    }
    CHECK(erased == 3);
    CHECK(v.begin() == v.end());
}

// A vector that never held anything: begin() == end(), and a loop over it does nothing.
TEST(vector_never_used_is_empty)
{
    MBVector<int> v;
    CHECK(v.begin() == v.end());
}
