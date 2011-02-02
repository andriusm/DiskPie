#include "FileGraph.h"

FileGraph::FileGraph()
{
    //ctor
}

FileGraph::~FileGraph()
{
    //dtor
}

/*
bool dirComparator (DirList a, DirList b)
{
    return (a.fcount >= b.fcount);
}
*/

DirList FileGraph::SearchDirectory(string rootDir, int depth = 0)
{
    DirList dl;
    HANDLE hFile;
    dl.fcount = 0;
    string strPattern;
    string strFilePath;
    WIN32_FIND_DATA FileInformation;
    strPattern = rootDir + "\\*.*";

    dl.dirname = rootDir;
    dl.depth = depth;

    hFile = FindFirstFile(strPattern.c_str(), &FileInformation);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(FileInformation.cFileName[0] != '.')
            {
                strFilePath.erase();
                strFilePath = rootDir + "\\" + FileInformation.cFileName;

                if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    DirList subdir = SearchDirectory(strFilePath, depth+1);
                    if(subdir.fcount>0)
                    {
                        //dl.depth = depth;
                        dl.fcount += subdir.fcount;
                        dl.dirs.push_back(subdir);
                    }
                }
                else
                {
                    //cout << strFilePath << endl;

                    dl.files.push_back(strFilePath);
                    dl.fcount += 1;
                }
            }
        }
        while(FindNextFile(hFile, &FileInformation) == TRUE);
        FindClose(hFile);
        DWORD dwError = GetLastError();
        if(dwError != ERROR_NO_MORE_FILES)
        {
            cout << "Error no: " << dwError << endl;
            dl.fcount = -1;
            return dl;
        }
    }

    return dl;
}

void FileGraph::Sort(DirList dl)
{
    /*
    sort (dl.dirs.begin(), dl.dirs.end(), dirComparator);
    for(int i=0;i<dl.dirs.size();i++)
    {
        Sort(dl.dirs[i]);
    }
    */
}

void FileGraph::FindFiles(string rootDir)
{
    dl = SearchDirectory(rootDir);
    Sort(dl);
}
