#pragma once
// EG2_Resource.h: a named, typed resource of the EG2 resource library (declaration from
// SteamboatApp.pdb). EG2::ResourceData is incomplete in the PDB; only the two slots that
// Bitmap::LoadFromResource calls are known, by position, so the others are placeholders.
#include "StdString.h"

namespace EG2 {
class Resource;

class ResourceContainer {
public:
    ResourceContainer(const EG2::ResourceContainer &);
    ResourceContainer();
    virtual ~ResourceContainer();
    virtual int CountResources() = 0;
    virtual unsigned char HasResource(_SS<char>) = 0;
    virtual EG2::Resource *GetResource(_SS<char>) = 0;
    virtual EG2::Resource *GetResourceAt(int) = 0;
};

struct ResourceType {
    unsigned short id;
    unsigned short version;
    ResourceType(unsigned short, unsigned short);
    ResourceType();
};

class ResourceData {
public:
    virtual void Slot00() = 0;
    virtual void Slot04() = 0;
    virtual void Slot08() = 0;
    virtual void Slot0C() = 0;
    virtual void Slot10() = 0;
    virtual void Slot14() = 0;
    virtual void Slot18() = 0;
    virtual void Slot1C() = 0;
    virtual void Slot20() = 0;
    virtual void Slot24() = 0;
    virtual void Slot28() = 0;
    virtual void Slot2C() = 0;
    virtual void *Slot30() = 0;     // what it returns goes to Slot34
    virtual void Slot34(void *) = 0;
};

class Resource : public EG2::ResourceContainer {
public:
    virtual ~Resource();
    _SS<char> GetName();
    void SetName(_SS<char>);
    EG2::ResourceType GetType();
    EG2::ResourceData *GetData() { return mData; }
    EG2::ResourceData *SetData(EG2::ResourceData *);
    virtual void SetResourceAt(int, EG2::Resource *) = 0;
protected:
    Resource(const EG2::Resource &);
    Resource(EG2::ResourceType);
    _SS<char> mName;                                    // +0x0004
    EG2::ResourceType mType;                            // +0x0014
    EG2::ResourceData *mData;                           // +0x0018
};
}
