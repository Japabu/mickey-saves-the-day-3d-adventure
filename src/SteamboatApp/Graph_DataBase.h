#pragma once
#include "ValList.h"
#include "Graph_NodeFactory.h"
namespace Graph { class BasicAttribute; }
namespace Graph { class BasicNode; }
namespace Graph { class Stream; }

namespace Graph {
// Reads a scene graph from a stream: the singleton keeps the node factory
// that turns type headers into nodes and attributes.
class DataBase {
public:
    static void Create() { sCurrentDataBase = new DataBase; }
    static void Destroy()
    {
        delete sCurrentDataBase;
        sCurrentDataBase = 0;
    }
    static Graph::DataBase *GetInstance()
    {
        if (!sCurrentDataBase)
            sCurrentDataBase = new DataBase;
        return sCurrentDataBase;
    }
    virtual ~DataBase();
    void SetFactory(Graph::NodeFactory *);
    Graph::NodeFactory *GetFactory() { return mFactory; }
    Graph::BasicNode *GetCurrentParentNode() { return mCurrentParentNode; }
    bool VerifyFile(Graph::Stream *, _SS<char>);
    Graph::BasicNode *ReadDB(Graph::Stream *);
    Graph::BasicNode *ReadDB(const char *);
    Graph::BasicNode *ReadNode(Graph::Stream *, Graph::BasicNode *);
    Graph::BasicAttribute *ReadAttribute(Graph::Stream *, Graph::BasicNode *);
protected:
    DataBase(const Graph::DataBase &);
    DataBase();
    _SS<char> ReadHeader(Graph::Stream *);
    _SS<char> ReadAttributeHeader(Graph::Stream *);
private:
    static Graph::DataBase *sCurrentDataBase;
    Graph::NodeFactory *mFactory;
    ValList<Graph::BasicNode *> mNodeList;
    Graph::BasicNode *mCurrentParentNode;
};
}
