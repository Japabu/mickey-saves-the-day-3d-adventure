#include "MfList.h"
#include "MfListItem.h"

// 0x100058b0 ??0MfList@@QAE@XZ
MfList::MfList()
{
    m_count = 0;
    m_tail = 0;
    m_head = 0;
}

// 0x100058c0 ??1MfList@@QAE@XZ
MfList::~MfList()
{
    removeContents();
}

// 0x100058d0 ?add@MfList@@QAEXPAVMfListItem@@@Z
void MfList::add(MfListItem *item)
{
    addLast(item);
}

// 0x100058e0 ??0MfListIter@@QAE@XZ
MfListIter::MfListIter()
{
    m_list = 0;
}

// 0x100058f0 ??DMfListIter@@QAEPAVMfListItem@@XZ
MfListItem *MfListIter::operator*()
{
    if (m_cur)
        return m_cur->item;
    return 0;
}

// 0x10005900 ?prev@MfListIter@@QAEPAVMfListItem@@XZ
MfListItem *MfListIter::prev()
{
    if (m_cur && m_cur->prev)
        return m_cur->prev->item;
    return 0;
}

// 0x10005920 ?next@MfListIter@@QAEPAVMfListItem@@XZ
MfListItem *MfListIter::next()
{
    if (m_cur && m_cur->next)
        return m_cur->next->item;
    return 0;
}

// 0x10005940 ??0MfList@@QAE@ABV0@@Z
MfList::MfList(const MfList &list)
{
    m_count = 0;
    m_tail = 0;
    m_head = 0;
    for (MfListNode *node = list.m_head; node; node = node->next)
        addLast(node->item);
}

// 0x10005980 ?addLast@MfList@@QAEXPAVMfListItem@@@Z
void MfList::addLast(MfListItem *item)
{
    MfListNode *node = new MfListNode;
    node->item = item;
    if (m_head == 0) {
        m_tail = node;
        m_head = node;
        node->next = 0;
        node->prev = 0;
        m_count++;
        return;
    }
    m_tail->next = node;
    node->prev = m_tail;
    node->next = 0;
    m_tail = node;
    m_count++;
}

// 0x100059d0 ?addFirst@MfList@@QAEXPAVMfListItem@@@Z
void MfList::addFirst(MfListItem *item)
{
    MfListNode *node = new MfListNode;
    node->item = item;
    if (m_head == 0) {
        m_tail = node;
        m_head = node;
        node->next = 0;
        node->prev = 0;
        m_count++;
        return;
    }
    m_head->prev = node;
    node->next = m_head;
    node->prev = 0;
    m_head = node;
    m_count++;
}

// 0x10005a20 ?remove@MfList@@QAEHPAVMfListItem@@@Z
int MfList::remove(MfListItem *item)
{
    for (MfListNode *node = m_head; node; node = node->next) {
        if (node->item == item) {
            removeNode(node);
            return 1;
        }
    }
    return 0;
}

// 0x10005a50 ?removeNode@MfList@@IAEXPAUMfListNode@@@Z
void MfList::removeNode(MfListNode *node)
{
    if (node->prev)
        node->prev->next = node->next;
    else
        m_head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        m_tail = node->prev;
    delete node;
    m_count--;
}

// 0x10005ab0 ?deleteNode@MfList@@IAEXPAUMfListNode@@@Z
void MfList::deleteNode(MfListNode *node)
{
    if (node->item)
        delete node->item;
    removeNode(node);
}

// 0x10005ae0 ?contains@MfList@@QAEHPAVMfListItem@@@Z
int MfList::contains(MfListItem *item)
{
    MfListNode *node;
    for (node = m_head; node; node = node->next) {
        if (node->item == item)
            break;
    }
    return node != 0;
}

// 0x10005b10 ?removeContents@MfList@@QAEXXZ
void MfList::removeContents()
{
    MfListNode *node = m_head;
    while (node) {
        MfListNode *next = node->next;
        delete node;
        node = next;
    }
    m_tail = 0;
    m_head = 0;
}

// 0x10005b40 ?deleteContents@MfList@@QAEXXZ
void MfList::deleteContents()
{
    MfListNode *node = m_head;
    while (node) {
        MfListNode *next = node->next;
        if (node->item)
            delete node->item;
        delete node;
        node = next;
    }
    m_tail = 0;
    m_head = 0;
}

// 0x10005b80 ?start@MfListIter@@QAEXABVMfList@@@Z
void MfListIter::start(const MfList &list)
{
    m_list = (MfList *)&list;
    m_cur = list.m_head;
    m_next = m_cur ? m_cur->next : 0;
}

// 0x10005bb0 ?advance@MfListIter@@QAEXXZ
void MfListIter::advance()
{
    m_cur = m_next;
    m_next = m_cur ? m_cur->next : 0;
}

// 0x10005bd0 ?removeItem@MfListIter@@QAEXXZ
void MfListIter::removeItem()
{
    if (m_cur) {
        m_list->removeNode(m_cur);
        m_cur = 0;
    }
}

// 0x10005bf0 ?deleteItem@MfListIter@@QAEXXZ
void MfListIter::deleteItem()
{
    if (m_cur) {
        m_list->deleteNode(m_cur);
        m_cur = 0;
    }
}

// 0x10005c10 ?start@MfListIterBwd@@QAEXABVMfList@@@Z
void MfListIterBwd::start(const MfList &list)
{
    m_list = (MfList *)&list;
    m_cur = list.m_tail;
    m_next = m_cur ? m_cur->prev : 0;
}

// 0x10005c40 ?advance@MfListIterBwd@@QAEXXZ
void MfListIterBwd::advance()
{
    m_cur = m_next;
    m_next = m_cur ? m_cur->prev : 0;
}

// Inline or compiler-generated members this object emits:
// 0x100061b0 ??4MfList@@QAEAAV0@ABV0@@Z
