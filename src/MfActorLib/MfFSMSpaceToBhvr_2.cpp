// MfActorLib.dll 0x10007c10-0x100080c0: MfFSMSpaceToBhvr.
#include "MfFSMSpaceToBhvr.h"
#include "MfStream.h"
#include "MfStreamConstructorEntry.h"

// 0x10007c10 ??1MfFSMSpaceToBhvr@@QAE@XZ
MfFSMSpaceToBhvr::~MfFSMSpaceToBhvr()
{
}

// 0x10007ca0 ??0MfFSMSpaceToBhvr@@QAE@ABV0@@Z
MfFSMSpaceToBhvr::MfFSMSpaceToBhvr(const MfFSMSpaceToBhvr &map)
{
    m_spaceName = map.m_spaceName;
    MfDblListItem *item;
    for (MfString *bhvr = map.m_behaviors.First(item); bhvr; bhvr = map.m_behaviors.Next(item))
        m_behaviors.Add(*bhvr);
}

// 0x10007d70 ?AddActiveBhvr@MfFSMSpaceToBhvr@@QAEXVMfString@@@Z
void MfFSMSpaceToBhvr::AddActiveBhvr(MfString bhvr)
{
    if (!IsActive(bhvr))
        m_behaviors.Add(bhvr);
}

// 0x10007de0 ?RemoveActiveBhvr@MfFSMSpaceToBhvr@@QAEXVMfString@@@Z
void MfFSMSpaceToBhvr::RemoveActiveBhvr(MfString bhvr)
{
    if (m_behaviors.Search(bhvr))
        m_behaviors.Delete(bhvr);
}

// 0x10007e50 ?IsActive@MfFSMSpaceToBhvr@@QAEHVMfString@@@Z
int MfFSMSpaceToBhvr::IsActive(MfString bhvr)
{
    if (m_behaviors.Search(bhvr))
        return 1;
    return 0;
}

// 0x10007ec0 ?fn_10007ec0@@YAPAVMfStreamObject@@XZ (original fn_10007ec0:) stream constructor for 'bmap'
static MfStreamObject *fn_10007ec0()
{
    return new MfFSMSpaceToBhvr;
}

// 0x10007f60 _$E6 (jumps to _$E5)
// 0x10007f70 _$E5 (constructs the stream constructor entry below)
static MfStreamConstructorEntry s_bhvrMapEntry('bmap', fn_10007ec0);

const long MfFSMSpaceToBhvr::STREAM_TYPE = 'bmap';

// 0x10007f90 ?StreamType@MfFSMSpaceToBhvr@@UBEJXZ
long MfFSMSpaceToBhvr::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10007fa0 ?WriteToStream@MfFSMSpaceToBhvr@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfFSMSpaceToBhvr::WriteToStream(MfStream &s) const
{
    s << m_spaceName << m_behaviors.GetCount();
    MfDblListItem *item;
    for (MfString *bhvr = (MfString *)m_behaviors.GetFirst(item); bhvr;
         bhvr = (MfString *)m_behaviors.GetNext(item))
        s << *bhvr;
    return s.Status();
}

// 0x10008000 ?ReadFromStream@MfFSMSpaceToBhvr@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfFSMSpaceToBhvr::ReadFromStream(MfStream &s)
{
    int count;
    MfString bhvr;
    count = 0;
    s >> m_spaceName >> count;
    for (int i = 0; i < count; i++) {
        s >> bhvr;
        if (s.Status() == 0)
            m_behaviors.Add(bhvr);
    }
    return s.Status();
}
