//inspired by https://github.com/sho3la/Learn-Computer-Graphics/tree/master/Tutorial_08
//docs from https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea
#include "FileDialog.h"
#include <Windows.h>


char *FileDialog::Open() {
    char *filter = "All Files (*.*)\0*.*\0";
    OPENFILENAME ofn;

    char filename[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;

    //to remove last dir visited , we should use ofn.lpstrInitialDir = null or set ofn.Flags = OFN_NOCHANGEDIR
    //this has to be set otherwise Opening caised fstream to be broken for shaders etc
    //because windows will set this dir as root instead og .exe of the app as it was originally
    //https://stackoverflow.com/questions/48166280/cant-close-openfilename
    ofn.Flags = OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn)) {
        char *res = new char[MAX_PATH];
        memcpy(res, filename, MAX_PATH);
        return res;
    }

    return nullptr;
}
