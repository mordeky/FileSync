#include "Common.h"

void addPathSep(CString& strPath)
{
	if(strPath.Right(1) != '\\')
		strPath += _T("\\");
}

void sepPathFile(const CString strFullFile, CString& FilePath, CString& FileName)
{
	// m_strTargetFile = "a11\\b22\\c33";
	int a = strFullFile.ReverseFind('\\');

	if(a == -1)
		FileName = strFullFile;
	else
	{
		// FileName = strrchr(strFullFile.GetString(), '\\') + 1;
		FileName = strFullFile.Right(strFullFile.GetLength()-a-1);
		FilePath = strFullFile.Left(a);
	}
}
