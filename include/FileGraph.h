#ifndef FILEGRAPH_H
#define FILEGRAPH_H

#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

typedef struct
{
    double startAngle;
    double angle;
    double startRadius;
    double radius;
    bool highlighted;
} Sector;

typedef struct _DirList
{
    vector<_DirList> dirs;
    vector<string> files;
    int fcount;
    int depth;
    string dirname;
    Sector sector;
} DirList;

class FileGraph
{
public:
    FileGraph();
    virtual ~FileGraph();

    DirList dl;

    void FindFiles(string rootDir);
    void Sort(DirList dl);
    DirList SearchDirectory(string rootDir, int depth);

protected:
private:
};

#endif // FILEGRAPH_H
