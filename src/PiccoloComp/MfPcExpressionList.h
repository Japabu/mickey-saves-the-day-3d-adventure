#pragma once
#include "MfDblList.h"
#include "MfPcParserItem.h"
class MfPcExpression;

// The arguments of a call or of an allocation, with a cursor of its own
// (0x20 bytes).
class MfPcExpressionList : public MfDblList, public MfPcParserItem {
public:
    MfPcExpressionList() {}
    virtual ~MfPcExpressionList() {}
    MfPcExpression *First() { return (MfPcExpression *)MfDblList::First(mCurrent); }
    MfPcExpression *Next() { return (MfPcExpression *)MfDblList::Next(mCurrent); }
    MfPcExpression *Last() { return (MfPcExpression *)MfDblList::Last(mCurrent); }
    MfPcExpression *Prev() { return (MfPcExpression *)MfDblList::Prev(mCurrent); }

    MfDblListItem *mCurrent;    // +0x1c
};
