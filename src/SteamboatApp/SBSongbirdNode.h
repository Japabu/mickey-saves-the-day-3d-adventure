#pragma once
// SBSongbirdNode.h: the Songbird activity: birds placed on a grid of notes play a song that can
// be saved in the song book (SBSongbirdNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"
#include "ConfirmationBox.h"

struct MBMouseEvent;

class SBSongbirdNode : public SBActivityNode {
public:
    SBSongbirdNode(IMBGameEngine *engine, MBString spaceCode);
    SBSongbirdNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBSongbirdNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    void OnNestClick(long birdID, void *data);
    void OnGridClick(MBMouseEvent *click);
    void OnSongbookClick(void *);
    void OnTrashClick(void *);
    void DrawSong();
    void ClearSongboard();
    ISBSongbird *CloneSongbird(MBComPtr<ISBSongbird>);
    void OnPlaySong(void *);
    void LoadSongs();
    void SaveSong();
    void PlaceNests();
    void InitSongGrid();
    ISBAnimButton *mSongBook;                           // +0x00d0
    ISBAnimButton *mTrash;                              // +0x00d4
    MBVector<MBComPtr<ISBSongbirdNote> > mGridSquares;  // +0x00d8
    MBComPtr<ISBSongbirdNote> *mNoteToPlay;             // +0x00e8
    MBVector<MBString> mSongs;                          // +0x00ec
    MBString *mSongPtr;                                 // +0x00fc
    MBComPtr<ISBAnimButton> mSongbird[7];               // +0x0100
    MBComPtr<ISBAnimButton> mLuciano;                   // +0x011c
    MBComPtr<ISBAnimButton> mCursorSongbird[7];         // +0x0120
    MBComPtr<ISBAnimButton> mBirdPicked;                // +0x013c
    char mNotePicked;                                   // +0x0140
    MBMap<MBComPtr<ISBAnimButton>, char, std::less<MBComPtr<ISBAnimButton> > > mBirdNoteMap; // +0x0144
    MBComPtr<IMBAnim> mSavedCursor;                     // +0x0154
    long mTimeToPlay;                                   // +0x0158
    long mLastNotePos;                                  // +0x015c
    long mCnt;                                          // +0x0160
    bool mMouseClicked;                                 // +0x0164
    bool mClickHandled;                                 // +0x0165
    int mCurrentSong;                                   // +0x0168
    ConfirmationBox mEraseSongDialog;                   // +0x016c
    int mFirstSong;                                     // +0x01bc
};
