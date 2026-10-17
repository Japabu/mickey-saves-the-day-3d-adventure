#pragma once
// MfFileType: the four-character type code of an MfFile or of an object in one.
#include "MfActorLib_api.h"
#include "MfString.h"

// A four-character code ('    ' when unset).
class MFACTOR_API MfFileType {
public:
    MfFileType(long t) { m_type = t; }
    MfFileType(MfString);
    ~MfFileType() {}
    int operator==(MfFileType t) { return m_type == t.m_type; }
    int operator!=(MfFileType t) { return m_type != t.m_type; }
    operator long() { return m_type; }
    MfFileType &operator=(const MfFileType &t) { m_type = t.m_type; return *this; }
private:
    long m_type;
};
