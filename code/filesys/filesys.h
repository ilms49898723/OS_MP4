// filesys.h
//  Data structures to represent the Nachos file system.
//
//  A file system is a set of files stored on disk, organized
//  into directories.  Operations on the file system have to
//  do with "naming" -- creating, opening, and deleting files,
//  given a textual file name.  Operations on an individual
//  "open" file (read, write, close) are to be found in the OpenFile
//  class (openfile.h).
//
//  We define two separate implementations of the file system.
//  The "STUB" version just re-defines the Nachos file system
//  operations as operations on the native UNIX file system on the machine
//  running the Nachos simulation.
//
//  The other version is a "real" file system, built on top of
//  a disk simulator.  The disk is simulated using the native UNIX
//  file system (in a file named "DISK").
//
//  In the "real" implementation, there are two key data structures used
//  in the file system.  There is a single "root" directory, listing
//  all of the files in the file system; unlike UNIX, the baseline
//  system does not provide a hierarchical directory structure.
//  In addition, there is a bitmap for allocating
//  disk sectors.  Both the root directory and the bitmap are themselves
//  stored as files in the Nachos file system -- this causes an interesting
//  bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"

#ifdef FILESYS_STUB         // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem {
public:
    FileSystem() {
        for (int i = 0; i < 20; i++) {
            fileDescriptorTable[i] = NULL;
        }
    }

    bool Create(char* name) {
        int fileDescriptor = OpenForWrite(name);

        if (fileDescriptor == -1) {
            return FALSE;
        }

        Close(fileDescriptor);
        return TRUE;
    }

    OpenFile* Open(char* name) {
        int fileDescriptor = OpenForReadWrite(name, FALSE);

        if (fileDescriptor == -1) {
            return NULL;
        }

        return new OpenFile(fileDescriptor);
    }

    int Open(char* name, int unused) {
        int fileid = OpenForReadWrite(name, FALSE);

        if (fileid == -1) {
            return -1;
        }

        if (fileid >= 20) {
            cerr << "fd >= 20!!!" << endl;
            ::Close(fileid);
            return -1;
        }

        fileDescriptorTable[fileid] = new OpenFile(fileid);
        return fileid;
    }

    int Write(char* buffer, int size, int fileid) {
        if (fileDescriptorTable[fileid] == NULL) {
            return -1;
        }

        return fileDescriptorTable[fileid]->Write(buffer, size);
    }

    int Read(char* buffer, int size, int fileid) {
        if (fileDescriptorTable[fileid] == NULL) {
            return -1;
        }

        return fileDescriptorTable[fileid]->Read(buffer, size);
    }

    int Close(int fileid) {
        if (fileDescriptorTable[fileid] == NULL) {
            return 0;
        }

        delete fileDescriptorTable[fileid];
        fileDescriptorTable[fileid] = NULL;
        return 1;
    }

    bool Remove(char* name) {
        return Unlink(name) == 0;
    }

    OpenFile* fileDescriptorTable[20];

};

#else // FILESYS
class FileSystem {
public:
    FileSystem(bool format);        // Initialize the file system.
    // Must be called *after* "synchDisk"
    // has been initialized.
    // If "format", there is nothing on
    // the disk, so initialize the directory
    // and the bitmap of free blocks.
    // MP4 mod tag
    ~FileSystem();

    bool Create(char* name, int initialSize);
    // Create a file (UNIX creat)

    bool CreateDirectory(char* name, char* parent);

    OpenFile* OpenDir(char* inpath);

    OpenFile* Open(char* name);     // Open a file (UNIX open)

    bool Remove(char* name, bool recur);        // Delete a file (UNIX unlink)

    void List(char* listDirectoryName);            // List all the files in the file system

    void RecursiveList(char* listDirectoryName, int tab = 4); // List all the files in the file system

    void Print();           // List all the files and their contents

    int Open(char* name, int unused);

    int Write(char* buffer, int size, int fileid);

    int Read(char* buffer, int size, int fileid);

    int Close(int fileid);

    void SplitPath(char* fullpath, char* parent, char* name);

    void JoinPath(char* dest, char* parent, char* name);

    OpenFile* fileDescriptorTable[20];

private:
    bool isLast[1024];

    OpenFile* freeMapFile;       // Bit map of free disk blocks,
    // represented as a file
    OpenFile* directoryFile;     // "Root" directory -- list of
    // file names, represented as a file

};

#endif // FILESYS

#endif // FS_H
