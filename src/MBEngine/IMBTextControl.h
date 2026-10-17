// IMBTextControl.h - an editable text object.
// Declared first by the includer (still one copy per unit in wave 1): MBString, MBString16.
#ifndef MBENGINE_IMBTEXTCONTROL_H
#define MBENGINE_IMBTEXTCONTROL_H

#include "IMBObject.h"

#include "MBTextControlColorFlag.h"
#include "MBTextControlJustifyFlag.h"
#include "MBTextControlTypeFlag.h"

class IMBFont;
class IMBTextValidator;
class MBColor;
class MBSize;

class IMBTextControl : public IMBObject {
public:
    virtual void SetFont(IMBFont *) = 0;
    virtual void GetFont(IMBFont **) = 0;
    // overloads in reverse vtable order
    virtual void GetText(MBString &) = 0;
    virtual void GetText(MBString16 &) = 0;
    virtual void SetText(const MBString &) = 0;
    virtual void SetText(const MBString16 &) = 0;
    virtual void SetSize(long, long) = 0;
    virtual MBSize GetSize(void) const = 0;
    virtual void SetType(MBTextControlTypeFlag) = 0;
    virtual void SetJustification(MBTextControlJustifyFlag) = 0;
    virtual void SetCharacterLimit(unsigned long) = 0;
    virtual void SetColor(MBTextControlColorFlag, const MBColor &) = 0;
    virtual void SetColorTransparent(MBTextControlColorFlag) = 0;
    virtual void SetValidator(IMBTextValidator *) = 0;
};

#endif
