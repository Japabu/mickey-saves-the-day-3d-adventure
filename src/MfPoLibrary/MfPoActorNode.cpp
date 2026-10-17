// MfPoActorNode (MfPoLibrary.dll 0x10023300-0x10026900; the object starts with
// the inline members of MfPoMaterial, MfPoCamera and MfPoLight)
#include "MfPoActorNode.h"
#include "MfPoCamera.h"
#include "MfPoLight.h"
#include "MfPoLibrary.h"
#include "MfAiWorld.h"
#include "MfMaterial.h"
#include "MfPoMaterial.h"
#include "MfPoVector3.h"
#include "MfAiActorNode.h"
#include "MfProperty.h"
#include "MfInterfaceRegistry.h"
#include "MfAiHookManager.h"
#include "MfMiscHookData.h"
#include "MfAiActor.h"
#include "MfTriangleSet.h"
#include "MfAiColNode.h"
#include "MfTransform3.h"
#include "MfPoTransform.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"
#include "MfStringList.h"

// 0x100233c0 ?GetPropTokenTable@MfPoCamera@@EAEPAVMfScPropTokenTable@@XZ
// 0x100233d0 ??0MfPoCamera@@QAE@ABV0@@Z (implicit)
// 0x10023420 ??_EMfPoCamera@@UAEPAXI@Z
// 0x10023470 ?GetPropTokenTable@MfPoLight@@EAEPAVMfScPropTokenTable@@XZ
// 0x10023480 ?SetNodeDeleted@MfPoCamera@@QAEXXZ
// 0x10023490 ??0MfPoLight@@QAE@ABV0@@Z (implicit)
// 0x100234e0 ??4MfPoCamera@@QAEAAV0@ABV0@@Z (implicit)
// 0x10023520 ??_EMfPoLight@@UAEPAXI@Z

// 0x10023570 _$E41
// 0x10023580 _$E38
// 0x100235a0 _$E40
// 0x100235b0 _$E39
static MfString strClassName("ActorNode");
MfPoActorNode *MfPoActorNode::poExemplar;
MfScPropTokenTable *MfPoActorNode::pttPropTokens;
MfScPropertyExtList *MfPoActorNode::pelPropExtensions;

// 0x100235c0 ?GetClassNameA@MfPoActorNode@@MAE?AVMfString@@XZ
MfString MfPoActorNode::GetClassName()
{
    return strClassName;
}

// 0x100235f0 ??0MfPoActorNode@@QAE@PAVMfAiActorNode@@@Z
MfPoActorNode::MfPoActorNode(MfAiActorNode *node) : MfPoBase(1)
{
    mActorNode = node;
}

// 0x10023620 ??0MfPoActorNode@@IAE@VExemplarChild@@@Z
MfPoActorNode::MfPoActorNode(ExemplarChild) : MfPoBase(0), mActorNode(0)
{
}

// TODO 0x10023650 ??0MfPoActorNode@@IAE@VExemplar@MfScData@@@Z: VC5 inline budget: identical for 4150 bytes, then the original still expands the inline MfScClassInfoText constructor for the last property (numTriangles) where this source calls the import (the opposite of MfPoCamera)
MfPoActorNode::MfPoActorNode(MfScData::Exemplar) : MfPoBase(0), mActorNode(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("ActorNode");
    MfScCiMethod *m;
    SetProperty(0, "FlushColModel", MfScDataHandle::CreateData((MfScMethod)&MfPoActorNode::PMFlushColModel, "ActorNode::FlushColModel", 0), 1);
    m = new MfScCiMethod("FlushColModel");
    *ci + m;
    SetProperty(0, "GetBoundingBox", MfScDataHandle::CreateData((MfScMethod)&MfPoActorNode::PMGetBoundingBox, "ActorNode::GetBoundingBox", 2), 1);
    m = new MfScCiMethod("GetBoundingBox");
    *ci + m;
    *m + new MfScMethodParameterInfo("Vector3 min", 0);
    *m + new MfScMethodParameterInfo("Vector3 max", 0);
    SetProperty(0, "CreateMaterialCopy", MfScDataHandle::CreateData((MfScMethod)&MfPoActorNode::PMCreateMaterialCopy, "ActorNode::CreateMaterialCopy", 1), 1);
    m = new MfScCiMethod("CreateMaterialCopy");
    *ci + m;
    *m + new MfScMethodParameterInfo("int materialIndex", 0);
    SetProperty(0, "SetMaterial", MfScDataHandle::CreateData((MfScMethod)&MfPoActorNode::PMSetMaterial, "ActorNode::SetMaterial", 2), 1);
    m = new MfScCiMethod("SetMaterial");
    *ci + m;
    *m + new MfScMethodParameterInfo("int materialIndex", 0);
    *m + new MfScMethodParameterInfo("Material material", 0);
    *ci + new MfScCiProperty("nodeName - String (read only)");
    *ci + new MfScCiProperty("nodeType - int (read only)");
    *ci + new MfScCiProperty("parent - ActorNode (read only)");
    *ci + new MfScCiProperty("firstChild - ActorNode (read only)");
    *ci + new MfScCiProperty("nextSibling - ActorNode (read only)");
    *ci + new MfScCiProperty("owner - Actor (read only)");
    *ci + new MfScCiProperty("globalXform - Transform");
    *ci + new MfScCiProperty("homeXform  - Transform (read only)");
    *ci + new MfScCiProperty("localXform  - Transform");
    *ci + new MfScCiProperty("prevXform  - Transform (read only)");
    *ci + new MfScCiProperty("colType - int");
    *ci + new MfScCiProperty("handleStyle - int (read only)");
    *ci + new MfScCiProperty("splineStart - Transform3 (read only)");
    *ci + new MfScCiProperty("splineEnd - Transform3 (read only)");
    *ci + new MfScCiProperty("renderOption - int");
    *ci + new MfScCiProperty("numTriangles - int (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("colType", 0);
    pttPropTokens->Add("firstChild", 1);
    pttPropTokens->Add("globalXform", 2);
    pttPropTokens->Add("handleStyle", 3);
    pttPropTokens->Add("homeXform", 4);
    pttPropTokens->Add("localXform", 5);
    pttPropTokens->Add("nextSibling", 6);
    pttPropTokens->Add("nodeName", 7);
    pttPropTokens->Add("nodeType", 8);
    pttPropTokens->Add("owner", 9);
    pttPropTokens->Add("parent", 10);
    pttPropTokens->Add("prevXform", 11);
    pttPropTokens->Add("renderOption", 12);
    pttPropTokens->Add("splineStart", 13);
    pttPropTokens->Add("splineEnd", 14);
    pttPropTokens->Add("numTriangles", 15);
    pelPropExtensions = new MfScPropertyExtList;
}

// 0x10024e50 ??1MfPoActorNode@@UAE@XZ
MfPoActorNode::~MfPoActorNode()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar) {
            delete pelPropExtensions;
            pelPropExtensions = 0;
            if (this == poExemplar)
                poExemplar = 0;
        }
    }
}

// 0x10024ef0 ?Initialize@MfPoActorNode@@SAXXZ
void MfPoActorNode::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoActorNode(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10024f80 ?Shutdown@MfPoActorNode@@SAXXZ
void MfPoActorNode::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x10024fe0 ?GetProperty@MfPoActorNode@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoActorNode::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfTransform3 local;
    MfTransform3 global;
    MfTransform3 home;
    if (mActorNode == (MfAiActorNode *)-1)
        return PEC_INVALID_HANDLE;
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (GetPropertyExt(name, value))
        return PEC_NO_ERROR;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 7:
            *value = MfScDataHandle::CreateData(mActorNode->GetNodeName(), 0);
            break;
        case 15:
            if (mActorNode->GetTriangleSet())
                *value = MfScDataHandle::CreateData(mActorNode->GetTriangleSet()->GetNumTriangles());
            else
                *value = MfScDataHandle::CreateData(0);
            break;
        case 2:
            if (mActorNode->GetNodeType() == MF_NODE_SPLINE) {
                MfTransform3 start;
                mActorNode->GetSplineStart(start);
                *value = MfScDataHandle::CreateData(new MfPoTransform(start));
            } else {
                mActorNode->GetTransform(local, home, global);
                *value = MfScDataHandle::CreateData(new MfPoTransform(global));
            }
            break;
        case 4:
            mActorNode->GetTransform(local, home, global);
            *value = MfScDataHandle::CreateData(new MfPoTransform(home));
            break;
        case 5:
            if (mActorNode->GetNodeType() == MF_NODE_SPLINE) {
                MfTransform3 parent = mActorNode->Parent()->GetGlobalTransform();
                MfTransform3 start;
                mActorNode->GetSplineStart(start);
                start = ~parent * start;
                *value = MfScDataHandle::CreateData(new MfPoTransform(start));
            } else {
                mActorNode->GetTransform(local, home, global);
                *value = MfScDataHandle::CreateData(new MfPoTransform(local));
            }
            break;
        case 11:
            if (mActorNode->GetPrevTransform(global))
                *value = MfScDataHandle::CreateData(new MfPoTransform(global));
            else
                *value = MfScDataHandle::CreateData(0);
            break;
        case 8:
            *value = MfScDataHandle::CreateData(mActorNode->GetNodeType());
            break;
        case 3:
            *value = MfScDataHandle::CreateData(mActorNode->GetHandleStyle());
            break;
        case 0:
            *value = MfScDataHandle::CreateData(mActorNode->ColNode().GetCheckStyle());
            break;
        case 10:
            if (mActorNode->Parent())
                *value = mActorNode->Parent()->GetPiccoloHandle();
            else
                *value = 0;
            break;
        case 1: {
            MfAiActorNode *child = mActorNode->FirstChild();
            if (child)
                *value = child->GetPiccoloHandle();
            else
                *value = 0;
            break;
        }
        case 6: {
            MfAiActorNode *sibling = mActorNode->NextSibling();
            if (sibling)
                *value = sibling->GetPiccoloHandle();
            else
                *value = 0;
            break;
        }
        case 9:
            if (mActorNode->GetActor())
                *value = mActorNode->GetActor()->GetPiccoloHandle();
            else
                *value = 0;
            break;
        case 12:
            *value = MfScDataHandle::CreateData(mActorNode->GetRenderStyle());
            break;
        case 13: {
            MfTransform3 start;
            if (mActorNode->GetSplineStart(start))
                *value = MfScDataHandle::CreateData(new MfPoTransform(start));
            else
                *value = 0;
            break;
        }
        case 14: {
            MfTransform3 end;
            if (mActorNode->GetSplineEnd(end))
                *value = MfScDataHandle::CreateData(new MfPoTransform(end));
            else
                *value = 0;
            break;
        }
        default:
            err = poExemplar->GetProperty(name, value);
            if (err == PEC_NO_SUCH_PROPERTY) {
                MfProperty *property;
                if (mActorNode->GetProperty(name, &property)) {
                    *value = PropertyToPiccolo(property);
                    if (*value)
                        err = PEC_NO_ERROR;
                }
            }
        }
    }
    return err;
}

// TODO 0x10025720 ?SetProperty@MfPoActorNode@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z: calls, inline-versus-import decisions and control flow line up (the imported MfVector3/MfRotation3 constructors and assignments come out of this file as in the original); the frame layout differs: the original puts err at the lowest slot and allocates the temporaries of the spline cases differently
MfScDataObject::MfPropertyErrorCode MfPoActorNode::SetProperty(MfScDataHandle *self, const MfString &name,
                                                               MfScDataHandle *value, int release)
{
    if (mActorNode == (MfAiActorNode *)-1)
        return PEC_INVALID_HANDLE;
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (SetPropertyExt(self, name, value, release))
        return PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 5: {
            MfPoTransform *source = dynamic_cast<MfPoTransform *>(value->GetData());
            if (!source)
                goto badType;
            MfTransform3 xform = source->Transform();
            if (mActorNode->GetNodeType() == MF_NODE_SPLINE) {
                MfTransform3 global = mActorNode->GetGlobalTransform();
                MfTransform3 start;
                mActorNode->GetSplineStart(start);
                MfTransform3 offset = ~global * start;
                xform = xform * ~offset;
            }
            xform.rotation().normalize();
            mActorNode->SetTransform(xform);
            break;
        }
        case 2: {
            MfPoTransform *source = dynamic_cast<MfPoTransform *>(value->GetData());
            if (!source)
                goto badType;
            MfTransform3 xform = source->Transform();
            MfTransform3 parent = mActorNode->Parent()->GetGlobalTransform();
            if (mActorNode->GetNodeType() == MF_NODE_SPLINE) {
                MfTransform3 global = mActorNode->GetGlobalTransform();
                MfTransform3 start;
                mActorNode->GetSplineStart(start);
                xform = ~parent * xform * ~(~global * start);
            } else {
                xform = ~parent * xform;
            }
            xform.rotation().normalize();
            mActorNode->SetTransform(xform);
            break;
        }
        case 0: {
            if (!value->IsType(MfScDataHandle::HT_INTEGER))
                goto badType;
            mActorNode->ColNode().SetCheckStyle(value->GetInt());
            MfMiscHookData data;
            MfAiHookManager *hooks;
            data.pNode = mActorNode;
            data.pActor = mActorNode->GetActor();
            data.value = value->GetInt();
            data.kind = 3;
            MfInterfaceRegistry::GetInterface(MfString("MfAiHookManager"), (MfAi **)&hooks);
            hooks->CallHooks(0x1b, &data);
            hooks->Release();
            break;
        }
        case 12:
            if (!value->IsType(MfScDataHandle::HT_INTEGER))
                goto badType;
            mActorNode->SetRenderStyle(value->GetInt());
            break;
        default: {
            MfProperty *property = PiccoloToProperty(name, value);
            if (!property)
                goto badType;
            mActorNode->SetProperty(property);
            break;
        }
        }
    } else {
badType:
        err = PEC_INVALID_PROPERTY_TYPE;
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x10025de0 ??1MfMiscHookData@@QAE@XZ (implicit; the unwind action of SetProperty)

// 0x10025e30 ?PMFlushColModel@MfPoActorNode@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActorNode::PMFlushColModel(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!InvalidObject(mActorNode, MfString("ActorNode"), MfString("FlushColModel")))
        mActorNode->FlushColModel();
    return 0;
}

// 0x10025eb0 ?PMShareColModel@MfPoActorNode@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActorNode::PMShareColModel(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActorNode, MfString("ActorNode"), MfString("ShareColModel")))
        return 0;
    if (CheckNull(MfString("ActorNode.ShareColModel()"), nArgs, args, 0))
        return 0;
    MfPoActorNode *other = args[0] ? dynamic_cast<MfPoActorNode *>(args[0]->GetData()) : 0;
    if (other) {
        mActorNode->ShareColModel(other->mActorNode);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("ActorNode"), MfString("ShareColModel"), 0, nArgs, args, &typeid(MfPoActorNode));
    return MfScDataHandle::CreateData(0);
}

// 0x10025ff0 ?PMGetBoundingBox@MfPoActorNode@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActorNode::PMGetBoundingBox(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActorNode, MfString("ActorNode"), MfString("GetBoundingBox")))
        return 0;
    if (CheckNull(MfString("ActorNode.GetBoundingBox()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_OBJECT) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        MfPoVector3 *low = args[0] ? dynamic_cast<MfPoVector3 *>(args[0]->GetData()) : 0;
        MfPoVector3 *high = args[1] ? dynamic_cast<MfPoVector3 *>(args[1]->GetData()) : 0;
        if (low && high) {
            mActorNode->GetContainer(low->Vector(), high->Vector());
            return MfScDataHandle::CreateData(1);
        }
    }
    InvalidType(MfString("ActorNode"), MfString("GetBoundingBox"), 0, nArgs, args,
                &typeid(MfPoVector3), &typeid(MfPoVector3));
    return MfScDataHandle::CreateData(0);
}

// 0x10026180 ?PMCreateMaterialCopy@MfPoActorNode@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActorNode::PMCreateMaterialCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActorNode, MfString("ActorNode"), MfString("CreateMaterialCopy")))
        return 0;
    if (CheckNull(MfString("ActorNode.CreateMaterialCopy()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        const MfMaterial *material = mActorNode->GetMaterial(args[0]->GetInt());
        if (!material) {
            BadParameters(MfString("ActorNode.CreateMaterialCopy()   Node has no material or the material index is out of range."));
        } else {
            MfMaterial *copy = g_paiWorld->CreateMaterial();
            *copy = *material;
            return MfScDataHandle::CreateData(new MfPoMaterial(copy, 1));
        }
    } else {
        InvalidType(MfString("ActorNode"), MfString("CreateMaterialCopy"), 0, nArgs, args, &typeid(int));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10026310 ?PMSetMaterial@MfPoActorNode@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActorNode::PMSetMaterial(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mActorNode, MfString("ActorNode"), MfString("SetMaterial")))
        return 0;
    if (CheckNull(MfString("ActorNode.SetMaterial()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        int index = args[0]->GetInt();
        MfPoMaterial *material = args[1] ? dynamic_cast<MfPoMaterial *>(args[1]->GetData()) : 0;
        if (mActorNode->SetMaterial(index, material->Material()))
            return MfScDataHandle::CreateData(1);
        BadParameters(MfString("ActorNode.SetMaterial()   The material index is out of range."));
    } else {
        InvalidType(MfString("ActorNode"), MfString("SetMaterial"), 0, nArgs, args,
                    &typeid(int), &typeid(MfPoMaterial));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10026480 ?InitConstants@MfPoActorNode@@SA?AVMfString@@XZ
MfString MfPoActorNode::InitConstants()
{
    MfString s;
    s << "global " << "MF_NODE_CAMERA" << " = " << MF_NODE_CAMERA << ";";
    s << "global " << "MF_NODE_FLOOR" << " = " << MF_NODE_FLOOR << ";";
    s << "global " << "MF_NODE_GEOMETRY" << " = " << MF_NODE_GEOMETRY << ";";
    s << "global " << "MF_NODE_HANDLE" << " = " << MF_NODE_HANDLE << ";";
    s << "global " << "MF_NODE_LIGHT" << " = " << MF_NODE_LIGHT << ";";
    s << "global " << "MF_NODE_SPLINE" << " = " << MF_NODE_SPLINE << ";";
    s << "global " << "MF_HANDLE_NODE_CYLINDER" << " = " << MF_HANDLE_NODE_CYLINDER << ";";
    s << "global " << "MF_HANDLE_NODE_DISC" << " = " << MF_HANDLE_NODE_DISC << ";";
    s << "global " << "MF_HANDLE_NODE_ERROR" << " = " << MF_HANDLE_NODE_ERROR << ";";
    s << "global " << "MF_HANDLE_NODE_ORIENTED_CYLINDER" << " = " << MF_HANDLE_NODE_ORIENTED_CYLINDER << ";";
    s << "global " << "MF_HANDLE_NODE_ORIENTED_DISC" << " = " << MF_HANDLE_NODE_ORIENTED_DISC << ";";
    s << "global " << "MF_HANDLE_NODE_POINT" << " = " << MF_HANDLE_NODE_POINT << ";";
    s << "global " << "MF_CCS_ON" << " = " << 15 << ";";
    s << "global " << "MF_CCS_OFF" << " = " << 0 << ";";
    s << "global " << "MF_CCS_TRIANGLES" << " = " << 1 << ";";
    s << "global " << "MF_CCS_SPHERE" << " = " << 2 << ";";
    s << "global " << "MF_CCS_RAY_FWD" << " = " << 4 << ";";
    s << "global " << "MF_CCS_HIDDEN_PICK" << " = " << 16 << ";";
    s << "global " << "MF_RENDER_INVISIBLE" << " = " << MF_RENDER_INVISIBLE << ";";
    s << "global " << "MF_RENDER_SOLID" << " = " << MF_RENDER_SOLID << ";";
    s << "global " << "MF_RENDER_WIREFRAME" << " = " << MF_RENDER_WIREFRAME << ";";
    return s;
}

// 0x100268a0 ?FilloutPropertyNameList@MfPoActorNode@@MAEXPAVMfStringList@@@Z
void MfPoActorNode::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x100268c0 ?GetInstanceProperties@MfPoActorNode@@MAEXPAVMfStringList@@H@Z
void MfPoActorNode::GetInstanceProperties(MfStringList *list, int sort)
{
    MfScDataObject::GetInstanceProperties(list, sort);
    if (mActorNode != (MfAiActorNode *)-1 && mActorNode != 0) {
        mActorNode->GetPropertyNames(list);
        if (sort)
            list->Sort(1);
    }
}

// 0x10026dd0 ?ActorNode@MfPoActorNode@@QAEPAVMfAiActorNode@@XZ (inline)
