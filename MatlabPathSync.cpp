#include "MatlabPathSync.h"
#include "Common.h"
#include <iostream>
using namespace std;

#define REJECT_FOLDER "Private"
#define REJECT_FOLDERTYPE_LIST    "., +, @, %"

MatlabPathSync::MatlabPathSync(ConfigFile* pConfigFile) : m_MFile(pConfigFile->m_strTargetFile)
{
	m_pConfig = pConfigFile;
	m_pConfig->add2RejectFolderList(REJECT_FOLDER);
	m_pConfig->add2RejectFolderTypeList(REJECT_FOLDERTYPE_LIST);

	CString strMFileName = pConfigFile->m_strTargetName;
	strMFileName = strMFileName.Left(strMFileName.GetLength() - 2); // 2 为 ".m" 的长度, strMFileName 为去掉扩展名后的文件名
	m_MFile << "function mypath = " << strMFileName << "\n"
		    << "mypath = [...\n";
}

MatlabPathSync::~MatlabPathSync(void)
{
	m_MFile << "];\n";
		    //<< "path(mypath, pathdef);\n";

	cout << "Synchronise " << m_pConfig->m_strTargetName << " successfully!" << endl;
}

void MatlabPathSync::writeOnePath(const CString strPath)
{
	m_MFile << "'" << strPath << ";'...\n";
}

void MatlabPathSync::run()
{
	cout << "\nStart synchronising..." << endl;

	UNIT* ptr;
	m_pCurAcceptPath = m_pConfig->m_pAcceptPath;

	while(m_pCurAcceptPath && m_pCurAcceptPath->strPath != "")
	{
		m_strCurRootPath = m_pCurAcceptPath->strPath;
		addPathSep(m_strCurRootPath);

		// 如果当前路径 m_strCurRootPath 下，有需要按顺序排序的文件夹，则先添加
		// 如：<AcceptPath value="E:\Academic\Labs\Matlab\Work\" order="Common, Imgs, Reading">
		ptr = m_pCurAcceptPath->pOrder;
		while(ptr && ptr->strData != "")
		{
			CString strCurPath = m_strCurRootPath + ptr->strData;
			addAllSubPaths(strCurPath, false);
			ptr = ptr->pNext;
		}

		addAllSubPaths(m_strCurRootPath);

		m_pCurAcceptPath = m_pCurAcceptPath->pNext;
	}
}

// 递归查找 strCurRootPath 下所有的子文件夹，并写入 M-File
void MatlabPathSync::addAllSubPaths(CString strCurRootPath, bool bCheckOrder)
{
	addPathSep(strCurRootPath); // 确保 strCurRootPath 以 '\' 结尾
	writeOnePath(strCurRootPath);

	CString strPath;
	
	strPath = strCurRootPath + _T("*.*");

	HANDLE hFind = FindFirstFile(strPath, &m_stFindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	while(TRUE)
	{
		// 检查文件夹的名字，并
		if(m_stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // 确保所找到的文件为文件夹
		{
			/**
			  如果文件夹的名字 m_stFindFileData.cFileName 为下列情况之一，则排除
			    (1) 文件夹的名字以 '.' 开头：
				    > 按照函数 FindFirstFile, FindNextFile 将会首先找到名字为 '.' 或 '..' 的文件夹
					> 一般是比较特殊的文件夹的名字如 '.svn' 文件夹
					> 用户一般无法新建以 '.' 开头的文件夹
				(2) 文件夹在拒绝列表中
				(3) 文件夹在排序列表中，因为已经在 run() 中添加过了
			*/

			if(m_pConfig->isOneRejectFolder(m_stFindFileData.cFileName))
				goto FIND_NEXT;

			if(bCheckOrder && m_pConfig->isOneOrderFolder(m_pCurAcceptPath, strCurRootPath, m_stFindFileData.cFileName))
				goto FIND_NEXT;

			strPath = strCurRootPath + m_stFindFileData.cFileName;
			strPath += "\\";

			if(m_pConfig->checkPath(m_pCurAcceptPath, strPath))
				addAllSubPaths(strPath, bCheckOrder);
		}

FIND_NEXT:
		if(!FindNextFile(hFind, &m_stFindFileData))
			break;
	}
	
	FindClose(hFind);
}