// MfColModel (MfActorLib.dll): a triangle set with a bounding-sphere tree over
// it, for collision tests. Layout as in src/RuntimeLib; the
// accessors are inline in the original header.
#ifndef MFPOLIBRARY_MFCOLMODEL_H
#define MFPOLIBRARY_MFCOLMODEL_H
#include "MfStreamObject.h"
#include "MfTransform3.h"
#include "MfVector3.h"
class MfAi;
class MfNode;
class MfSphereNode;
class MfTriangleSet;

class __declspec(dllimport) MfOrientedBox {
public:
    MfTransform3 mTransform;                                // 0x00
    MfVector3 mExtent;                                      // 0x1c
};

class __declspec(dllimport) MfColModel : public MfStreamObject {
public:
    MfColModel();
    MfColModel(const MfColModel &);
    virtual ~MfColModel();
    virtual long StreamType() const;
    virtual MfStreamError ReadFromStream(MfStream &);
    virtual MfStreamError WriteToStream(MfStream &) const;
    virtual void DeleteSphereTree();

    MfTriangleSet *GetTriangleSet() { return mTriangleSet; }
    void SetTriangleSet(MfTriangleSet *);
    MfSphereNode *GetRoot() { return mRoot; }
    void SetRoot(MfSphereNode *root) { mRoot = root; }
    int GetDepth() { return mDepth; }
    void SetDepth(int depth) { mDepth = depth; }
    MfOrientedBox &OrientedBox() { return mBox; }
    void GetBoundingBox(MfVector3 &min, MfVector3 &max);
    void SetBoundingBox(MfVector3 &min, MfVector3 &max);
    int SetUserData(MfAi *userData) { mUserData = userData; return 1; }
    MfAi *GetUserData() { return mUserData; }
    void UpdateSphereTree();
    static const long STREAM_TYPE;

    int m04;                                                // 0x04
    MfTriangleSet *mTriangleSet;                            // 0x08
    int m0c;                                                // 0x0c
    MfSphereNode *mRoot;                                    // 0x10
    int mDepth;                                             // 0x14
    MfOrientedBox mBox;                                     // 0x18
    float mMin[3];                                          // 0x40
    float mMax[3];                                          // 0x4c
    MfAi *mUserData;                                        // 0x58
};

#endif
