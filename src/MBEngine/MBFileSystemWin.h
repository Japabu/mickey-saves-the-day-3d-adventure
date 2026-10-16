// MBFileSystemWin.h - declarations for MBEngine.dll MBFileSystemWin.obj
#ifndef MBFILESYSTEMWIN_H
#define MBFILESYSTEMWIN_H

#include "CMBWinFile.h"

extern "C" {
bool MBFileSpecExists(const IMBFileSpec *spec);
bool MBIsFileSpecAFolder(const IMBFileSpec *spec);
bool MBIsFileSpecAFile(const IMBFileSpec *spec);
bool MBIsFileSpecHidden(const IMBFileSpec *spec);
long MBCreateFolder(const IMBFileSpec *folderSpec);
long MBCreateFile(const IMBFileSpec *fileSpec);
long MBDeleteFile(const IMBFileSpec *fileSpec);
void MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);
void MBOpenFileWithClass(const MBGUID &classid, const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);
bool MBIsFileReadOnly(const IMBFileSpec *fileSpec);
long MBSetFileReadOnly(const IMBFileSpec *fileSpec, bool isReadOnly);
}

#endif
