// MfTextureKeyframe and MfTextureKeyList (RuntimeLib.dll 100539b0-10054010):
// the textures a skill puts on its actor's materials ('TKey' keyframes in
// 'TxKL' lists).
#include "MfSkillKeyframes.h"
#include "MfRenderNode.h"
#include "MfRenderer.h"
#include "MfSkeletonTraversal.h"
#include "MfStream.h"

// 0x100539b0 ??0MfTextureKeyframe@@QAE@XZ
MfTextureKeyframe::MfTextureKeyframe()
    : mMaterialIndex(0), mNodeIndex(-1), mPreloaded(0)
{
}

// 0x10053a20 ?CreateCopy@MfTextureKeyframe@@UAEPAVMfKeyframe@@XZ
MfKeyframe *MfTextureKeyframe::CreateCopy()
{
    return new MfTextureKeyframe(*this);
}

// 0x10053aa0 ??0MfTextureKeyframe@@QAE@ABV0@@Z
MfTextureKeyframe::MfTextureKeyframe(const MfTextureKeyframe &key)
    : MfKeyframe(key), mMaterialIndex(key.mMaterialIndex), mNodeIndex(key.mNodeIndex),
      mMaterial(key.mMaterial), mPreloaded(0)
{
}

// 0x10053b10 ??1MfTextureKeyframe@@UAE@XZ
MfTextureKeyframe::~MfTextureKeyframe()
{
    Unload();
}

// 0x10053b70 ?SetMaterial@MfTextureKeyframe@@QAEXABVMfString@@ABVMfMaterial@@@Z
void MfTextureKeyframe::SetMaterial(const MfString &texture, const MfMaterial &material)
{
    mMaterial = material;
}

// 0x10053b90 ?Apply@MfTextureKeyframe@@QAEXPAVMfSkeletonTraversal@@@Z
void MfTextureKeyframe::Apply(MfSkeletonTraversal *nodes)
{
    MfSkeletonNode *node = nodes->node(mNodeIndex);
    if (node) {
        MfRenderNode *renderNode = node->GetRenderNode();
        if (renderNode)
            renderNode->ChangeMaterial(mMaterialIndex, &mMaterial);
    }
}

// 0x10053bc0 ?Preload@MfTextureKeyframe@@QAEXXZ
void MfTextureKeyframe::Preload()
{
    if (!mPreloaded && MfRenderer::Instance())
        mPreloaded = MfRenderer::Instance()->PreloadMaterial(&mMaterial);
}

// 0x10053bf0 ?Unload@MfTextureKeyframe@@QAEXXZ
void MfTextureKeyframe::Unload()
{
    if (mPreloaded) {
        MfRenderer::Instance()->UnloadMaterial(mPreloaded);
        mPreloaded = 0;
    }
}

// 0x10053c20 ?CreateTextureKeyframe@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateTextureKeyframe()
{
    return new MfTextureKeyframe;
}

// 0x10053c80 _$E18 (original $E: the static initializer of the constructor entry, with 10053c90)
// 0x10053c90 _$E17
static MfStreamConstructorEntry keyframeEntry(MfTextureKeyframe::STREAM_TYPE, CreateTextureKeyframe);

// 0x10053cb0 ?StreamType@MfTextureKeyframe@@UBEJXZ
long MfTextureKeyframe::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10053cc0 ?ReadFromStream@MfTextureKeyframe@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTextureKeyframe::ReadFromStream(MfStream &stream)
{
    MfKeyframe::ReadFromStream(stream);
    stream >> mMaterial >> mMaterialIndex >> mNodeIndex;
    return stream.Status();
}

// 0x10053d00 ?WriteToStream@MfTextureKeyframe@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTextureKeyframe::WriteToStream(MfStream &stream) const
{
    MfKeyframe::WriteToStream(stream);
    stream << mMaterial << mMaterialIndex << mNodeIndex;
    return stream.Status();
}

// 0x10053d40 ??0MfTextureKeyList@@QAE@XZ
MfTextureKeyList::MfTextureKeyList()
    : mName("Texture Events"), mMaterialIndex(-1), mNodeIndex(-1)
{
}

// 0x10053dc0 ??0MfTextureKeyList@@QAE@ABV0@@Z
MfTextureKeyList::MfTextureKeyList(const MfTextureKeyList &list)
    : MfKeyList(list), mName(list.mName), mMaterialIndex(list.mMaterialIndex), mNodeIndex(list.mNodeIndex)
{
}

// 0x10053e30 ??1MfTextureKeyList@@UAE@XZ
MfTextureKeyList::~MfTextureKeyList()
{
}

// 0x10053e90 ?InitMaterials@MfTextureKeyList@@QAEXXZ
void MfTextureKeyList::InitMaterials()
{
    MfKeyListNode *pos;
    for (MfTextureKeyframe *key = (MfTextureKeyframe *)GetFirst(pos); key; key = (MfTextureKeyframe *)GetNext(pos))
        key->Preload();
}

// 0x10053ed0 ?CreateTextureKeyList@@YAPAVMfStreamObject@@XZ (original (unnamed): creates the objects of the stream type)
static MfStreamObject *CreateTextureKeyList()
{
    return new MfTextureKeyList;
}

// 0x10053f30 _$E21 (original $E: the static initializer of the constructor entry, with 10053f40)
// 0x10053f40 _$E20
static MfStreamConstructorEntry listEntry(MfTextureKeyList::STREAM_TYPE, CreateTextureKeyList);

// 0x10053f60 ?StreamType@MfTextureKeyList@@UBEJXZ
long MfTextureKeyList::StreamType() const
{
    return STREAM_TYPE;
}

// 0x10053f70 ?ReadFromStream@MfTextureKeyList@@UAE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTextureKeyList::ReadFromStream(MfStream &stream)
{
    if (stream.userFormat >= 16)
        stream >> mName >> mMaterialIndex >> mNodeIndex;
    MfKeyList::ReadFromStream(stream);
    return stream.Status();
}

// 0x10053fc0 ?WriteToStream@MfTextureKeyList@@UBE?AW4MfStreamError@@AAVMfStream@@@Z
MfStreamError MfTextureKeyList::WriteToStream(MfStream &stream) const
{
    stream << mName << mMaterialIndex << mNodeIndex;
    MfKeyList::WriteToStream(stream);
    return stream.Status();
}
