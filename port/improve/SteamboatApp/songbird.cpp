// songbird.cpp - improvement over the original (port/edits/improvements.py hooks it in):
// the songs saved in the Songbird activity stay in the song book.
//
// The original wrote a saved song into SBscene.ini opened read-only (OpenPrefStream), and
// through a preferences object that applies its changes only when it dies, after the file
// was released; nothing reached the file. Erasing a song never touched the file at all.
// Saving and erasing now write the whole book into the file opened for writing.
#include "SBSongbirdNode.h"
#include "MBRuntime.h"
#include <stdio.h>

// [songs] song0 .. songN-1 as the book holds them; an empty songN ends the list for
// LoadSongs, which stops at the first empty song (keys after it are never read).
void SBSongbirdNode::PortWriteSongs()
{
    IMBStream *stream = 0;
    MBComPtr<IMBFileSpec> fileSpec(MBIID_IMBFileSpec);
    if (fileSpec) {
        fileSpec->SetPath((mSpaceCode + "scene.ini").c_str());
        MBOpenFile(fileSpec, 7, &stream); // read and write, an existing file
    }
    if (!stream)
        return;
    {
        MBComPtr<IMBPref> pref(MBIID_CMBPref);
        char key[16];
        int n = 0;
        for (MBString *song = mSongs.begin(); song != mSongs.end(); song++, n++) {
            sprintf(key, "song%d", n);
            pref->SetString(stream, "songs", key, *song);
        }
        sprintf(key, "song%d", n);
        MBString none;
        pref->SetString(stream, "songs", key, none);
    }
    stream->Release();
}
