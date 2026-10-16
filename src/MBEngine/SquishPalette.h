// SquishPalette.h - the palette cache of MBEngine.dll SquishPalette.obj
#ifndef SQUISHPALETTE_H
#define SQUISHPALETTE_H

#include "Squish.h"
#include <map>

template <class K, class V, class Pr> class MBMap : public std::map<K, V, Pr, MBAllocator<V> > {
};

// Palettes are shared by seed (the sum of their entries); a palette nobody references any more is
// dropped the next time one is added. While a movie is streamed out, each palette is written once.
class SquishPaletteManager {
public:
    ~SquishPaletteManager(void) {
        MBMap<unsigned long, SquishPalette *, std::less<unsigned long> >::iterator it = mSquishPaletteMap.begin();
        while (it != mSquishPaletteMap.end()) {
            delete (*it).second;
            it++;
        }
    }
    SquishPalette *Find(long seed) {
        MBMap<unsigned long, SquishPalette *, std::less<unsigned long> >::iterator it = mSquishPaletteMap.find(seed);
        if (it != mSquishPaletteMap.end()) {
            (*it).second->AddRef();
            return (*it).second;
        }
        return 0;
    }
    void Purge(void) {
        MBMap<unsigned long, SquishPalette *, std::less<unsigned long> >::iterator it = mSquishPaletteMap.begin();
        while (it != mSquishPaletteMap.end()) {
            if ((*it).second->AddRef() == 1) {
                delete (*it).second;
                mSquishPaletteMap.erase(it++);
            } else {
                ++it;
            }
        }
    }
    void Add(SquishPalette *palette) {
        Purge();
        mSquishPaletteMap[palette->GetSeed()] = palette;
    }
    void BeginStreamOut(void) {
        mPaletteStreamedMap.clear();
        MBMap<unsigned long, SquishPalette *, std::less<unsigned long> >::iterator it = mSquishPaletteMap.begin();
        while (it != mSquishPaletteMap.end())
            mPaletteStreamedMap[(*it++).second->GetSeed()] = false;
    }
    void EndStreamOut(void) { mPaletteStreamedMap.clear(); }
    bool HasBeenStreamed(unsigned long seed) { return mPaletteStreamedMap[seed]; }
    void MarkStreamed(unsigned long seed) { mPaletteStreamedMap[seed] = true; }
    MBMap<unsigned long, SquishPalette *, std::less<unsigned long> > mSquishPaletteMap;
    MBMap<unsigned long, bool, std::less<unsigned long> > mPaletteStreamedMap;
};

#endif
