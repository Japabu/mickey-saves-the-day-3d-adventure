#pragma once
class MfString;

// The source text of a compilation and where its lines start (0x14 bytes).
class MfPcBuffer {
public:
    MfPcBuffer();
    virtual ~MfPcBuffer();
    int SetText(MfString &text);
    int GetLine(int line, MfString &out);
    int AddLine(int offset);

    int mLineCount;             // +0x04
    char **mLines;              // +0x08 (0x400 entries)
    int mLength;                // +0x0c
    char *mText;                // +0x10 (0x8000 bytes)
};
