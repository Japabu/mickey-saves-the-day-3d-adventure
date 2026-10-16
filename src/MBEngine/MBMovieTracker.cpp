// MBEngine.dll unit MBMovieTracker (.\MBEngineRelease\MBMovieTracker.obj)
#include "MBMovieTracker.h"

// 0x10039460 ??0MBMovieTracker@@QAE@XZ
MBMovieTracker::MBMovieTracker(void)
    : mAlreadyListening(false)
{
    _mRefCnt = 0;
}

// 0x10039510 ??1MBMovieTracker@@QAE@XZ
MBMovieTracker::~MBMovieTracker(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-5, this);
    if (mMovieList.size() > 0)
        mMovieList.clear();
}

// 0x10039680 ?SetupTracker@MBMovieTracker@@QAEXXZ
void MBMovieTracker::SetupTracker(void)
{
    if (!mAlreadyListening) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        if (broadcaster) {
            broadcaster->AddListener(-5, this);
            mAlreadyListening = true;
        }
    }
}

// 0x100396f0 ?ListenToEvent@MBMovieTracker@@UAEXJJPAX@Z
void MBMovieTracker::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -5) {
        MBMovieEventStruct *movieEvent = (MBMovieEventStruct *)data;
        if (event == 0)
            AddMovie(movieEvent->movie, movieEvent->id);
        else if (event == 1)
            RemoveMovie(movieEvent->movie, movieEvent->id);
    }
}

// 0x10039730 ?FindMovie@MBMovieTracker@@UAEPAVIMBMovie@@K@Z
IMBMovie *MBMovieTracker::FindMovie(unsigned long id)
{
    MovieList::iterator it = mMovieList.find(id);
    if (it != mMovieList.end() && (*it).second) {
        (*it).second->AddRef();
        return (*it).second;
    }
    return 0;
}

// 0x100397a0 ?AddMovie@MBMovieTracker@@MAEXPAVIMBMovie@@K@Z
void MBMovieTracker::AddMovie(IMBMovie *movie, unsigned long id)
{
    if (movie)
        mMovieList.insert(MovieList::value_type(id, movie));
}

// 0x10039890 ?RemoveMovie@MBMovieTracker@@MAEXPAVIMBMovie@@K@Z
void MBMovieTracker::RemoveMovie(IMBMovie *movie, unsigned long id)
{
    if (movie) {
        MovieList::iterator it = mMovieList.find(id);
        if (it != mMovieList.end() && (*it).second == movie)
            mMovieList.erase(it);
    }
}

// 0x1003b130 ?QueryInterface@MBMovieTracker@@UAEJABUMBGUID@@PAPAX@Z
long MBMovieTracker::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBEventListener *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1003b200 ?AddRef@MBMovieTracker@@UAEKXZ
unsigned long MBMovieTracker::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1003b210 ?Release@MBMovieTracker@@UAEKXZ
unsigned long MBMovieTracker::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt) {
        if (this) {
            this->~MBMovieTracker();
            MBfree_release(this);
        }
    }
    return r;
}

// 0x10039900 ?begin@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAE?AViterator@12@XZ
// 0x10039910 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10039e00 ?erase@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10039ef0 ?_Erase@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10039f50 ?_Insert@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBKPAVIMBMovie@@@2@@Z
// 0x1003a230 ??Eiterator@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAE?AV012@H@Z
// 0x1003a2c0 ?_Lbound@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@IBEPAU_Node@12@ABK@Z
// 0x1003a310 ?_Dec@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAEXXZ
// 0x1003a3b0 ?_Inc@const_iterator@?$_Tree@KU?$pair@$$CBKPAVIMBMovie@@@std@@U_Kfn@?$map@KPAVIMBMovie@@U?$less@K@std@@V?$MBAllocator@PAVIMBMovie@@@@@2@U?$less@K@2@V?$MBAllocator@PAVIMBMovie@@@@@std@@QAEXXZ
// 0x1003a600 ?_Construct@std@@YAXPAU?$pair@$$CBKPAVIMBMovie@@@1@ABU21@@Z
