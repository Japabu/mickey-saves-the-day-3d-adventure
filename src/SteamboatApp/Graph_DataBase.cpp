#include "Graph_DataBase.h"
#include "Graph_BasicNode.h"
#include "Graph_FileStream.h"

// 0x100dfb80 ??1DataBase@Graph@@UAE@XZ
Graph::DataBase::~DataBase()
{
    if (mFactory) {
        delete mFactory;
        mFactory = 0;
    }
    sCurrentDataBase = 0;
    mNodeList.RemoveAll();
}

// TODO 0x100dfc60 ?VerifyFile@DataBase@Graph@@QAE_NPAVStream@2@V?$_SS@D@@@Z: (82% similar) original folds basic_string::npos to -1 (inlined find, Find), VC6 loads it from memory here
bool Graph::DataBase::VerifyFile(Graph::Stream *stream, _SS<char> header)
{
    int length;
    stream->Read(&length);
    if (length >= 0 && length <= 79) {
        char buffer[80];
        stream->Read((unsigned char *)buffer, length);
        buffer[length] = 0;
        if (header.Find(buffer) != -1)
            return true;
    }
    return false;
}

// 0x100dfdd0 ?SetFactory@DataBase@Graph@@QAEXPAVNodeFactory@2@@Z
void Graph::DataBase::SetFactory(Graph::NodeFactory *factory)
{
    if (mFactory)
        delete mFactory;
    mFactory = factory;
}

// 0x100dfe00 ?ReadDB@DataBase@Graph@@QAEPAVBasicNode@2@PAVStream@2@@Z
Graph::BasicNode *Graph::DataBase::ReadDB(Graph::Stream *stream)
{
    if (stream->IsOpen() && VerifyFile(stream, mFactory->GetValidFileHeader())) {
        if (mNodeList.Count())
            mNodeList.RemoveAll();
        Graph::BasicNode *root = ReadNode(stream, 0);
        mNodeList.RemoveAll();
        return root;
    }
    return 0;
}

// 0x100dffa0 ?ReadDB@DataBase@Graph@@QAEPAVBasicNode@2@PBD@Z
Graph::BasicNode *Graph::DataBase::ReadDB(const char *fileName)
{
    Graph::FileStream *stream = new Graph::FileStream;
    if (stream->Open(fileName, Graph::FileStream::ReadMode)
        && VerifyFile(stream, mFactory->GetValidFileHeader())) {
        if (mNodeList.Count())
            mNodeList.RemoveAll();
        Graph::BasicNode *root = ReadNode(stream, 0);
        delete stream;
        mNodeList.RemoveAll();
        return root;
    }
    return 0;
}

// TODO 0x100e00c0 ?ReadNode@DataBase@Graph@@QAEPAVBasicNode@2@PAVStream@2@PAV32@@Z: (64% similar) Find compares with -1 immediates in the original (npos folded), VC6 loads npos
Graph::BasicNode *Graph::DataBase::ReadNode(Graph::Stream *stream, Graph::BasicNode *parent)
{
    mCurrentParentNode = parent;
    _SS<char> header = ReadHeader(stream);
    if (header.Find("INSTANCE") != -1) {
        int index;
        stream->Read(&index);
        return mNodeList.GetAt(index);
    }
    Graph::BasicNode *node = mFactory->CreateNode(stream, header);
    if (node)
        mNodeList.Add(node);
    return node;
}

// 0x100e02e0 ?ReadAttribute@DataBase@Graph@@QAEPAVBasicAttribute@2@PAVStream@2@PAVBasicNode@2@@Z
Graph::BasicAttribute *Graph::DataBase::ReadAttribute(Graph::Stream *stream, Graph::BasicNode *node)
{
    mCurrentParentNode = node;
    _SS<char> header = ReadAttributeHeader(stream);
    return mFactory->CreateAttribute(stream, header);
}

// 0x100e03e0 ??0DataBase@Graph@@IAE@XZ
Graph::DataBase::DataBase()
{
    mFactory = 0;
    mCurrentParentNode = 0;
}

// TODO 0x100e0400 ?ReadHeader@DataBase@Graph@@IAE?AV?$_SS@D@@PAVStream@2@@Z: (83% similar) identical except the copy into the return value: original calls assign(const char *, size_t), VC6 inlines it
_SS<char> Graph::DataBase::ReadHeader(Graph::Stream *stream)
{
    short length = 0;
    stream->Read(&length);
    char *buffer = new char[length + 1];
    stream->Read((unsigned char *)buffer, length);
    buffer[length] = 0;
    _SS<char> header(buffer);
    delete buffer;
    return header;
}

// TODO 0x100e05e0 ?ReadAttributeHeader@DataBase@Graph@@IAE?AV?$_SS@D@@PAVStream@2@@Z: (83% similar) as ReadHeader (assign(const char *, size_t) inlined in the copy)
_SS<char> Graph::DataBase::ReadAttributeHeader(Graph::Stream *stream)
{
    int length = 0;
    stream->Read(&length);
    char *buffer = new char[length + 1];
    stream->Read((unsigned char *)buffer, length);
    buffer[length] = 0;
    _SS<char> header(buffer);
    delete buffer;
    return header;
}

// 0x100dfc40 ??_GDataBase@Graph@@UAEPAXI@Z
// 0x100e0960 ??1?$ValList@PAVBasicNode@Graph@@@@QAE@XZ
// 0x100dfec0 ?GetValidFileHeader@NodeFactory@Graph@@QAE?AV?$_SS@D@@XZ

// Template and inline instances this unit also emits:
// 0x1000e6b0 ?substr@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBE?AV12@II@Z
// 0x1000f9c0 ?append@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ID@Z
// 0x100d2d10 ?Buffer@?$_SS@D@@QAEPADH@Z
// 0x100e07c0 ?resize@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEXI@Z

// static data (zero in the original, .bss)
Graph::DataBase *Graph::DataBase::sCurrentDataBase = 0;
