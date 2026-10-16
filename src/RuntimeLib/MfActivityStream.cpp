// The stream helpers of the activities (RuntimeLib.dll 10009050-1000aa50):
// the configurations of a manipulation (grasp and move paths), the 2D and 3D
// paths, references to nodes and actors, and the ignore masks.
#include "MfActor.h"
#include "MfGCPath.h"
#include "MfMsgData.h"
#include "MfPath2.h"
#include "MfPath3.h"
#include "MfSkeletonNode.h"
#include "MfSkeletonTraversal.h"
#include "MfSpace.h"
#include "MfSplineNode.h"
#include "MfStream.h"
#include "MfWorld.h"

MfStreamError WriteConfig(MfStream &stream, MfActor *actor, MfTransform3 *config);
MfStreamError ReadConfig(MfStream &stream, MfActor *actor, MfTransform3 *&config);
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node);
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node);

// a new grasp path: empty
struct MfNewGCPath : public MfGCPath {
    MfNewGCPath()
    {
        m00 = 0;
        m04 = 0;
        m08 = 0;
        m0c = 0;
        m10 = 0;
        m38 = 0;
        m3c = 0;
    }
};

// a new move path: empty
struct MfNewMTPath : public MfMTPath {
    MfNewMTPath()
    {
        mFrom = 0;
        mTo = 0;
    }
};

// 0x10009050 ?WriteGCPath@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@PAUMfGCPath@@@Z
MfStreamError WriteGCPath(MfStream &stream, MfActor *actor, MfGCPath *path)
{
    WriteConfig(stream, actor, path->m04);
    WriteConfig(stream, actor, path->m08);
    WriteConfig(stream, actor, path->m0c);
    WriteConfig(stream, actor, path->m10);
    stream.WriteBytes(sizeof(MfTransform3), &path->m14);
    stream << path->m30;
    stream << path->m34;
    stream << (int)(path->m38 != 0);
    if (path->m38) {
        for (int i = 0; i < path->m34; i++)
            stream.WriteBytes(sizeof(MfTransform3), &path->m38[i]);
    }
    return stream.Status();
}

// TODO 0x10009110 ?ReadGCPath@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@AAPAUMfGCPath@@@Z: the original expands the MfGCPath destructor and constructor inline (MfGCPath.h declares the destructor out of line) (34% similar)
MfStreamError ReadGCPath(MfStream &stream, MfActor *actor, MfGCPath *&result)
{
    result = 0;
    MfGCPath *path = new MfNewGCPath;
    ReadConfig(stream, actor, path->m04);
    ReadConfig(stream, actor, path->m08);
    ReadConfig(stream, actor, path->m0c);
    ReadConfig(stream, actor, path->m10);
    stream.ReadBytes(sizeof(MfTransform3), &path->m14);
    stream >> path->m30;
    stream >> path->m34;
    int samples = 0;
    stream >> samples;
    if (!stream.Status() && samples && path->m34 > 0) {
        path->m38 = new MfTransform3[path->m34];
        for (int i = 0; i < path->m34; i++)
            stream.ReadBytes(sizeof(MfTransform3), &path->m38[i]);
    } else {
        path->m38 = 0;
    }
    if (stream.Status() || path->m34 < 1) {
        delete path;
        return MF_STREAM_OK;
    }
    result = path;
    return stream.Status();
}

// 0x100093d0 ?WriteMTPath@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@PAUMfMTPath@@@Z
MfStreamError WriteMTPath(MfStream &stream, MfActor *actor, MfMTPath *path)
{
    WriteConfig(stream, actor, path->mFrom);
    WriteConfig(stream, actor, path->mTo);
    return stream.Status();
}

// 0x10009410 ?ReadMTPath@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@AAPAUMfMTPath@@@Z
MfStreamError ReadMTPath(MfStream &stream, MfActor *actor, MfMTPath *&result)
{
    result = 0;
    MfMTPath *path = new MfNewMTPath;
    ReadConfig(stream, actor, path->mFrom);
    ReadConfig(stream, actor, path->mTo);
    if (stream.Status()) {
        if (path) {
            if (path->mFrom)
                delete [] path->mFrom;
            if (path->mTo)
                delete [] path->mTo;
            delete path;
        }
        path = 0;
    }
    result = path;
    return stream.Status();
}

// 0x100094d0 ?WriteConfig@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@PAVMfTransform3@@@Z
// A configuration of the actor: the transforms of the root and the hip,
// then the rotations of the other nodes.
MfStreamError WriteConfig(MfStream &stream, MfActor *actor, MfTransform3 *config)
{
    stream << (int)(config != 0);
    if (config) {
        int count = actor->skeletonTraversal()->numNodes();
        for (int i = 0; i < count; i++) {
            if (i <= 1)
                stream.WriteBytes(sizeof(MfTransform3), &config[i]);
            else
                stream.WriteBytes(sizeof(MfRotation3), &config[i].rotation());
        }
    }
    return stream.Status();
}

// 0x10009540 ?ReadConfig@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@AAPAVMfTransform3@@@Z
MfStreamError ReadConfig(MfStream &stream, MfActor *actor, MfTransform3 *&result)
{
    int present;
    result = 0;
    stream >> present;
    if (present && !stream.Status()) {
        MfSkeletonTraversal *traversal = actor->skeletonTraversal();
        int count = traversal->numNodes();
        MfTransform3 *config = new MfTransform3[count];
        for (int i = 0; i < count; i++) {
            if (i <= 1) {
                stream.ReadBytes(sizeof(MfTransform3), &config[i]);
            } else {
                stream.ReadBytes(sizeof(MfRotation3), &config[i].rotation());
                config[i].translation() = ((MfSkeletonNode *)traversal->node(i))->mTransform.translation();
            }
        }
        if (stream.Status()) {
            if (config)
                delete config;
            config = 0;
        }
        result = config;
    }
    return stream.Status();
}

// TODO 0x10009690 ?WritePath@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfPath2@@@Z: the bounds loop and the quantization differ in register and temporary choice; the original calls the imported MfTransform2 assignment (51% similar)
// A ring path as its arc, an open path as its points: every 16th point
// whole, the others as bytes of the step from the point before.
MfStreamError WritePath(MfStream &stream, MfPath2 *path)
{
    stream << path->mOpen;
    if (!path->mOpen) {
        MfTransform2 start;
        start = path->mStart;
        int flags = path->mFlags;
        float radius = path->mRadius;
        float angle = path->mAngle;
        stream.WriteBytes(sizeof(MfTransform2), &start);
        stream << flags;
        stream << radius;
        stream << angle;
    } else {
        int count = path->mCount;
        stream << count;
        MfVector2 max;
        MfVector2 min;
        MfVector2 scale;
        if (count > 2) {
            MfVector2 step = path->Point(1).translation() - path->Point(0).translation();
            max = step;
            min = step;
            for (int i = 2; i < count; i++) {
                step = path->Point(i).translation() - path->Point(i - 1).translation();
                if (step.x() > max.x())
                    max.x() = step.x();
                if (step.x() < min.x())
                    min.x() = step.x();
                max.y() = step.y() > max.y() ? step.y() : max.y();
                min.y() = step.y() < min.y() ? step.y() : min.y();
            }
            scale.x() = 255.0f / (max.x() - min.x());
            scale.y() = 255.0f / (max.y() - min.y());
            stream << min.x();
            stream << min.y();
            stream << 1.0f / scale.x();
            stream << 1.0f / scale.y();
        }
        for (int i = 0; i < count; i++) {
            MfTransform2 &point = path->Point(i);
            if (i % 16 && i != count - 1) {
                MfTransform2 &last = path->Point(i - 1);
                float angle = point.rotation().angle();
                float dx = point.x() - last.x();
                float dy = point.y() - last.y();
                if (angle < 0.0f) {
                    do
                        angle += 6.2831855f;
                    while (angle < 0.0f);
                }
                while (angle > 6.2831855f)
                    angle -= 6.2831855f;
                unsigned char b = (unsigned char)((dx - min.x()) * scale.x());
                stream.WriteBytes(1, &b);
                b = (unsigned char)((dy - min.y()) * scale.y());
                stream.WriteBytes(1, &b);
                b = (unsigned char)(angle / 0.024639944f);
                stream.WriteBytes(1, &b);
            } else {
                stream.WriteBytes(sizeof(MfTransform2), &point);
            }
        }
    }
    return stream.Status();
}

// TODO 0x10009b30 ?ReadPath@@YA?AW4MfStreamError@@AAVMfStream@@AAPAVMfPath2@@@Z: the x sum is
// grouped as the original adds it; what is left: the original frame is 8 bytes larger (the arc
// transform and the two vectors in slots of their own), and here the EH state is reset to -1
// before the final Status call, which the original does not do (88% similar)
MfStreamError ReadPath(MfStream &stream, MfPath2 *&result)
{
    MfPath2 *path = new MfPath2;
    int open;
    result = 0;
    stream >> open;
    if (!open) {
        MfTransform2 start;
        int flags;
        float radius;
        float angle;
        stream.ReadBytes(sizeof(MfTransform2), &start);
        stream >> flags;
        stream >> radius;
        stream >> angle;
        path->SetArc(start, flags, radius, angle);
        if (stream.Status()) {
            if (path)
                delete path;
            path = 0;
        }
        result = path;
    } else {
        int count;
        stream >> count;
        path->SetNumPoints(count);
        MfVector2 min;
        MfVector2 step;
        if (count > 2) {
            stream >> min.x();
            stream >> min.y();
            stream >> step.x();
            stream >> step.y();
        }
        for (int i = 0; i < count; i++) {
            MfTransform2 &point = path->Point(i);
            if (i % 16 && i != count - 1) {
                MfTransform2 &last = path->Point(i - 1);
                unsigned char bx, by, ba;
                stream.ReadBytes(1, &bx);
                stream.ReadBytes(1, &by);
                stream.ReadBytes(1, &ba);
                point.translation().x() = (bx * step.x() + last.x()) + min.x();
                point.translation().y() = by * step.y() + last.y() + min.y();
                point.rotation() = ba * 0.024639944f;
            } else {
                stream.ReadBytes(sizeof(MfTransform2), &point);
            }
        }
        if (stream.Status()) {
            if (path)
                delete path;
            path = 0;
        }
        result = path;
    }
    return stream.Status();
}

// TODO 0x10009e70 ?WritePath3@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfPath3@@@Z: the original does not fold the ARC test of NumKeys inside the KEYS branch (tests the known kind again and passes the count through a stack temporary); ours folds it; the ternary form of NumKeys does the same (89% similar)
MfStreamError WritePath3(MfStream &stream, MfPath3 *path)
{
    stream << path->mKind;
    if (path->mKind == MfPath3::KEYS) {
        int count = path->NumKeys();
        stream << count;
        for (int i = 0; i < count; i++)
            stream.WriteBytes(sizeof(MfTransform3), &path->Key(i));
        stream << path->mDuration;
    } else if (path->mKind == MfPath3::SPLINE_KEYS) {
        stream << path->mDuration;
        stream.WriteBytes(sizeof(MfTransform3), &path->mKey0);
        stream.WriteBytes(sizeof(MfTransform3), &path->mKey1);
        stream.WriteBytes(sizeof(MfTransform3), &path->mKey2);
        stream.WriteBytes(sizeof(MfTransform3), &path->mKey3);
    } else if (path->mKind == MfPath3::MOVE) {
        stream << path->mToEnd;
        if (path->mToEnd == 0) {
            stream << path->mFlags;
            stream.WriteBytes(sizeof(MfTransform3), &path->mStart);
            stream.WriteBytes(sizeof(MfVector3), &path->mGoal);
            stream.WriteBytes(sizeof(MfVector3), &path->mDirection);
        } else if (path->mToEnd == 1) {
            stream << path->mFlags;
            stream.WriteBytes(sizeof(MfTransform3), &path->mStart);
            stream.WriteBytes(sizeof(MfTransform3), &path->mEnd);
            stream << path->DurationSeconds();
            stream.WriteBytes(sizeof(MfVector3), &path->mGoal);
        }
    } else if (path->mKind == MfPath3::ARC) {
        stream << path->Angle();
        stream << path->Radius();
        stream << path->mFlags;
        stream.WriteBytes(sizeof(MfTransform3), &path->mStart);
    } else if (path->mKind == MfPath3::SPLINE) {
        WriteNodeReference(stream, path->mSpline);
        stream << path->mDuration;
        stream << path->mReverse;
        stream.WriteBytes(sizeof(MfVector3), &path->mSplineStart);
    }
    return stream.Status();
}

// 0x1000a0a0 ?ReadPath3@@YA?AW4MfStreamError@@AAVMfStream@@AAPAVMfPath3@@PAVMfActor@@@Z
MfStreamError ReadPath3(MfStream &stream, MfPath3 *&result, MfActor *actor)
{
    result = 0;
    MfPath3 *path = new MfPath3;
    int kind;
    stream >> kind;
    if (kind == MfPath3::KEYS) {
        int count;
        int duration;
        stream >> count;
        if (!stream.Status() && count > 0) {
            MfTransform3 *keys = new MfTransform3[count];
            for (int i = 0; i < count; i++)
                stream.ReadBytes(sizeof(MfTransform3), &keys[i]);
            stream >> duration;
            if (stream.Status()) {
                if (keys)
                    delete [] keys;
                if (path)
                    delete path;
                return stream.Status();
            }
            path->SetKeys(keys, count, duration);
        }
    } else if (kind == MfPath3::SPLINE_KEYS) {
        MfTransform3 key0;
        MfTransform3 key1;
        MfTransform3 key2;
        MfTransform3 key3;
        int duration;
        stream >> duration;
        stream.ReadBytes(sizeof(MfTransform3), &key0);
        stream.ReadBytes(sizeof(MfTransform3), &key1);
        stream.ReadBytes(sizeof(MfTransform3), &key2);
        stream.ReadBytes(sizeof(MfTransform3), &key3);
        if (stream.Status()) {
            if (path)
                delete path;
            return stream.Status();
        }
        path->SetKeys(key0, key1, key2, key3, duration);
    } else if (kind == MfPath3::SPLINE) {
        MfSkeletonNode *spline;
        int duration;
        int reverse;
        MfVector3 start;
        ReadNodeReference(stream, actor, spline);
        stream >> duration;
        stream >> reverse;
        stream.ReadBytes(sizeof(MfVector3), &start);
        if (stream.Status()) {
            if (path)
                delete path;
            return stream.Status();
        }
        path->SetSpline((MfSplineNode *)spline, reverse, start, duration);
    } else if (kind == MfPath3::MOVE) {
        MfTransform3 from;
        MfTransform3 to;
        MfVector3 goal;
        MfVector3 direction;
        int toEnd;
        int flags;
        stream >> toEnd;
        if (toEnd == 0) {
            stream >> flags;
            stream.ReadBytes(sizeof(MfTransform3), &from);
            stream.ReadBytes(sizeof(MfVector3), &goal);
            stream.ReadBytes(sizeof(MfVector3), &direction);
            if (stream.Status()) {
                if (path)
                    delete path;
                return stream.Status();
            }
            path->SetLine(from, goal, direction, flags);
        } else if (toEnd == 1) {
            float duration;
            stream >> flags;
            stream.ReadBytes(sizeof(MfTransform3), &from);
            stream.ReadBytes(sizeof(MfTransform3), &to);
            stream >> duration;
            stream.ReadBytes(sizeof(MfVector3), &goal);
            if (stream.Status()) {
                if (path)
                    delete path;
                return stream.Status();
            }
            path->SetMove(from, goal, to, duration, flags);
        }
    } else if (kind == MfPath3::ARC) {
        float angle;
        float radius;
        int flags;
        MfTransform3 start;
        stream >> angle;
        stream >> radius;
        stream >> flags;
        stream.ReadBytes(sizeof(MfTransform3), &start);
        if (stream.Status()) {
            if (path)
                delete path;
            return stream.Status();
        }
        path->SetArc(start, flags, radius, angle);
    }
    result = path;
    return stream.Status();
}

// EQUIV 0x1000a6e0 ?WriteNodeReference@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfSkeletonNode@@@Z: reviewed: node and stream swap esi/edi and the original saves edi only on the path that uses it; the same instructions otherwise
// A node as its actor's unique ID and its name; -99 for none.
MfStreamError WriteNodeReference(MfStream &stream, MfSkeletonNode *node)
{
    if (!node) {
        stream << -99;
        return stream.Status();
    }
    if (node->mOwner)
        stream << node->mOwner->UniqueID();
    else
        stream << 0;
    if (node->mProto) {
        stream << node->mProto->mName;
        return stream.Status();
    }
    stream << node->mLocalName;
    return stream.Status();
}

// 0x1000a760 ?ReadNodeReference@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@AAPAVMfSkeletonNode@@@Z
// Without the actor the node is the root of actor's space.
MfStreamError ReadNodeReference(MfStream &stream, MfActor *actor, MfSkeletonNode *&node)
{
    MfString name;
    int id;
    stream >> id;
    if (id == -99) {
        node = 0;
        return stream.Status();
    }
    stream >> name;
    node = 0;
    if (g_world) {
        MfActor *owner = (MfActor *)g_world->GetActor(id);
        if (!owner && actor)
            node = actor->mSpace->mRoot;
        else if (owner)
            node = (MfSkeletonNode *)owner->GetNodeByName(name);
    }
    return stream.Status();
}

// 0x1000a880 ?WriteActorReference@@YA?AW4MfStreamError@@AAVMfStream@@PAVMfActor@@@Z
MfStreamError WriteActorReference(MfStream &stream, MfActor *actor)
{
    if (actor)
        stream << actor->UniqueID();
    else
        stream << 0;
    return stream.Status();
}

// 0x1000a8b0 ?ReadActorReference@@YA?AW4MfStreamError@@AAVMfStream@@AAPAVMfActor@@@Z
MfStreamError ReadActorReference(MfStream &stream, MfActor *&actor)
{
    int id;
    stream >> id;
    if (g_world) {
        actor = (MfActor *)g_world->GetActor(id);
        return stream.Status();
    }
    actor = 0;
    return stream.Status();
}

// 0x1000a920 ?WriteMasks@@YA?AW4MfStreamError@@AAVMfStream@@PAHH@Z
// The ignore masks, eight to a byte.
MfStreamError WriteMasks(MfStream &stream, int *masks, int count)
{
    stream << count;
    for (int i = 0; i < count; i += 8) {
        unsigned char bits = 0;
        for (int j = 0; j < 8 && i + j < count; j++) {
            if (masks[i + j])
                bits |= 1;
        }
        stream.WriteBytes(1, &bits);
    }
    return stream.Status();
}

// 0x1000a990 ?ReadMasks@@YA?AW4MfStreamError@@AAVMfStream@@AAPAHAAH@Z
MfStreamError ReadMasks(MfStream &stream, int *&masks, int &count)
{
    masks = 0;
    stream >> count;
    if (count > 0 && !stream.Status()) {
        masks = new int[count];
        for (int i = 0; i < count; i += 8) {
            unsigned char bits;
            stream.ReadBytes(1, &bits);
            for (int j = 0; j < 8 && i + j < count; j++)
                masks[i + j] = bits & 1;
        }
        if (stream.Status())
            delete masks;
    }
    return stream.Status();
}
