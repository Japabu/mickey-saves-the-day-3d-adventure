#pragma once
#include "StdString.h"
namespace Graph { class BasicAttribute; }
namespace Graph { class BasicNode; }
namespace Graph { class Stream; }

namespace Graph {
// Turns the type headers of a scene file into nodes and attributes; knows the
// header a valid file starts with.
class NodeFactory {
public:
    NodeFactory();
    virtual ~NodeFactory();
    virtual Graph::BasicNode *CreateNode(Graph::Stream *, _SS<char>) const = 0;
    virtual Graph::BasicAttribute *CreateAttribute(Graph::Stream *, _SS<char>) const = 0;
    void SetValidFileHeader(_SS<char> header) { mValidFileHeader = header; }
    _SS<char> GetValidFileHeader() { return mValidFileHeader; }
private:
    _SS<char> mValidFileHeader;
};
}
