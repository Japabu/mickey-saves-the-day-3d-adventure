// MfFSMPlayBack, MfVisualConnector (RuntimeLib.dll 1008be20-1008c3b0).
#include <iostream.h>
#include "Mf2DVectorList.h"
#include "MfStateDisplayPos.h"
#include "MfTransitionProperty.h"

// 0x1008be20 ??0MfFSMPlayBack@@QAE@H@Z
MfFSMPlayBack::MfFSMPlayBack(int playback)
{
    mPlayback = playback;
    mType = TYPE;
}

// 0x1008be70 ??1MfFSMPlayBack@@UAE@XZ
MfFSMPlayBack::~MfFSMPlayBack()
{
}

// 0x1008be80 ?Clone@MfFSMPlayBack@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfFSMPlayBack::Clone()
{
    MfFSMPlayBack *copy = new MfFSMPlayBack(mPlayback);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008bee0 ?Write@MfFSMPlayBack@@UAEXAAVostream@@AAHH@Z
void MfFSMPlayBack::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (!mode)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "playback{" << mPlayback << "}" << endl;
}

// 0x1008bfc0 ??0MfVisualConnector@@QAE@NNPAVMf2DVectorList@@@Z
MfVisualConnector::MfVisualConnector(double x, double y, Mf2DVectorList *points)
    : mX(x), mY(y), mPoints(points)
{
    mType = TYPE;
}

// 0x1008c020 ??1MfVisualConnector@@UAE@XZ
MfVisualConnector::~MfVisualConnector()
{
    if (mPoints)
        delete mPoints;
    mPoints = 0;
}

// 0x1008c080 ?Clone@MfVisualConnector@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfVisualConnector::Clone()
{
    Mf2DVectorList *points = new Mf2DVectorList(1);
    MfDblListItem *pli;
    for (Mf2DVector *p = (Mf2DVector *)mPoints->GetFirst(pli); p; p = (Mf2DVector *)mPoints->GetNext(pli)) {
        Mf2DVector *point = new Mf2DVector;
        *point = *p;
        points->Append(point);
    }
    MfVisualConnector *copy = new MfVisualConnector(mX, mY, points);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1008c1d0 ?Write@MfVisualConnector@@UAEXAAVostream@@AAHH@Z
void MfVisualConnector::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    int count = 0;
    if (mode == 1)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "visual_con " << mY << " " << mX << endl;
    os << tabs;
    os << "{" << endl;
    os << tabs;
    MfDblListItem *pli;
    Mf2DVector *last = (Mf2DVector *)mPoints->GetLast(pli);
    for (Mf2DVector *p = (Mf2DVector *)mPoints->GetFirst(pli); p; p = (Mf2DVector *)mPoints->GetNext(pli)) {
        count++;
        os << p->pos.x << " " << p->pos.y;
        if (p != last) {
            os << ",";
            if (count == 2) {
                os << endl;
                count = 0;
                os << tabs;
            }
        }
    }
    os << endl;
    os << tabs;
    os << "}" << endl;
}
