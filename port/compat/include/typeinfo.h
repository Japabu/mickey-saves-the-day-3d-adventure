// typeinfo.h - Visual C++ declares type_info in the global namespace. clang's
// Microsoft mode predeclares a ::type_info of its own, which typeid does not return,
// so the name type_info is mapped onto an alias of the std class, both in the global
// namespace and (for the library headers included later) in std.
#ifndef PORT_TYPEINFO_H
#define PORT_TYPEINFO_H
#include <typeinfo>
namespace std { typedef type_info port_type_info; }
typedef std::type_info port_type_info;
#define type_info port_type_info
using std::bad_cast;
using std::bad_typeid;
#endif
