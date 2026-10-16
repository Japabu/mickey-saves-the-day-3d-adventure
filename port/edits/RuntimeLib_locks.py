# Windows CRITICAL_SECTIONs that the decompiled classes keep as raw bytes (0x18, their
# 32-bit size). The port's CRITICAL_SECTION holds pointers: 40 bytes, 8-byte aligned. The
# storage gets room and alignment; the source's (CRITICAL_SECTION *) casts stay.
EDITS = {
    'RuntimeLib/MfMsgDispatcher.h': [
        ('    char mLocks[2][0x18];', '    alignas(16) char mLocks[2][64];', 1)],
    'RuntimeLib/MfSharedMsgQueue.h': [
        ('    char mLock[0x18];', '    alignas(16) char mLock[64];', 1)],
}
