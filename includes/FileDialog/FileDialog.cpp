//inspired by https://github.com/sho3la/Learn-Computer-Graphics/tree/master/Tutorial_08
//docs from https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea
#include "FileDialog.h"
#include <Windows.h>
#ifndef OFN_NOCHANGEDIR
#define OFN_NOCHANGEDIR
#endif
char * FileDialog::Open()
{
  char* filter = "All Files (*.*)\0*.*\0";
  OPENFILENAME ofn;

  char filename[MAX_PATH] = "";
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.lpstrFilter = filter;
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  //to remove last dir visited , should use ofn.lpstrInitialDir = null or OFN_NOCHANGEDIR but its not defined?
  //ofn.FlagsEx.Cha
  if(GetOpenFileName(&ofn))
  {
    char* res = new char[MAX_PATH];
    memcpy(res, filename,MAX_PATH);
    return res;
  }

  return nullptr;
}
