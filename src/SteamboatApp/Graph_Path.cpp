#include "Graph_Path.h"
#include "Graph_BasicNode.h"

// 0x100e1150 ??0Path@Graph@@QAE@XZ
Graph::Path::Path()
{
}

// 0x100e11c0 ??0Path@Graph@@QAE@ABV01@@Z
Graph::Path::Path(const Graph::Path &path) : m_NodeList(path.m_NodeList)
{
}

// 0x100e12a0 ??1Path@Graph@@UAE@XZ
Graph::Path::~Path()
{
}

// 0x100e12d0 ?GetTop@Path@Graph@@QBEPAVBasicNode@2@XZ
Graph::BasicNode *Graph::Path::GetTop() const
{
    int length = GetLength();
    if (length == 0)
        return 0;
    return m_NodeList[length - 1];
}

// 0x100e12f0 ?GetNode@Path@Graph@@QBEPAVBasicNode@2@H@Z
Graph::BasicNode *Graph::Path::GetNode(int index) const
{
    if (index < 0 || index >= GetLength())
        return 0;
    return m_NodeList[index];
}

// 0x100e1320 ?FindNode@Path@Graph@@QBEHPAVBasicNode@2@@Z
int Graph::Path::FindNode(Graph::BasicNode *node) const
{
    for (int i = 0; i < GetLength(); i++) {
        if (node == m_NodeList[i])
            return i;
    }
    return -1;
}

// 0x100e1350 ??APath@Graph@@QBEPAVBasicNode@1@H@Z
Graph::BasicNode *Graph::Path::operator[](int index) const
{
    if (index < 0 || index >= GetLength())
        return 0;
    return m_NodeList[index];
}

// 0x100e1380 ??4Path@Graph@@QAEAAV01@ABV01@@Z
Graph::Path &Graph::Path::operator=(const Graph::Path &path)
{
    m_NodeList = path.m_NodeList;
    return *this;
}

// 0x100e13f0 ??8Path@Graph@@QBE_NABV01@@Z
bool Graph::Path::operator==(const Graph::Path &path) const
{
    int length = GetLength();
    if (length != path.GetLength())
        return false;
    for (int i = 0; i < length; i++) {
        if (m_NodeList[i] != path.m_NodeList[i])
            return false;
    }
    return true;
}


// Matched by this code:
// 0x100e11a0 ??_GPath@Graph@@UAEPAXI@Z
// 0x100e1450 ??_G?$Vec@PAVBasicNode@Graph@@@@UAEPAXI@Z
// 0x100e1470 ??1?$Vec@PAVBasicNode@Graph@@@@UAE@XZ