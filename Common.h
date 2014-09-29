#pragma once
#include <atlstr.h>

extern void addPathSep(CString& strPath);
extern void sepPathFile(const CString strFullFile, CString& FilePath, CString& FileName);

template<typename T>
extern void deleteLink(T* p);

template<typename T>
extern T* movePtr2LinkEnd(T* pUnit);
