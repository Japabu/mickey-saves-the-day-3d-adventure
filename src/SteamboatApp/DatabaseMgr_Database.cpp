#include "DatabaseMgr_Database.h"
#include "DatabaseMgr.h"

// 0x10115740 ?AddDBRef@Database@DatabaseMgr@@QAEKXZ
unsigned long DatabaseMgr::Database::AddDBRef()
{
    return ++field_4;
}
