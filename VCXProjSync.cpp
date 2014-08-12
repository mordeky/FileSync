#include "VCXProjSync.h"
#include "Common.h"
#include <iostream>
using namespace std;

#define REJECT_FILETYPE_LIST     "sln, sdf, opensdf, dsp, ncb, vcproj, user, suo, aps, filters, vcxproj, vcproj"
#define REJECT_FOLDERTYPE_LIST   "%"

VCXProjSync::VCXProjSync(ConfigFile* pConfigFile)
{
	m_pConfig = pConfigFile;
	m_pConfig->add2RejectFolderTypeList(REJECT_FOLDERTYPE_LIST);
	m_pConfig->add2List(pConfigFile->m_pRejectFileType, REJECT_FILETYPE_LIST); // VCX定义的拒绝接受的“文件类型”

	bVCProj = m_pConfig->checkFileType(m_pConfig->m_strTargetName, ".vcproj");

	if(bVCProj)
		m_vc = VC(m_pConfig->m_strTargetFile);
	else
	{
		m_vcx = VCX(m_pConfig->m_strTargetFile);
		m_vcxFilter = VCXFilter(m_pConfig->m_strTargetFile + ".filters");
	}
}


VCXProjSync::~VCXProjSync(void)
{
	if(bVCProj)
	{
		m_vc.save();
		cout << "Synchronise " << m_vc.m_strFile << " successfully!" << endl;
	}else{
		m_vcx.save();
		m_vcxFilter.save();
		cout << "Synchronise " << m_vcx.m_strFile << " successfully!" << endl;
		cout << "Synchronise " << m_vcxFilter.m_strFile << " successfully!" << endl;
	}
}

void VCXProjSync::setRelativePath(CString strCurRootPath)
{
	if(strCurRootPath == "") return;

	CString strVCPath = m_pConfig->m_strTargetPath + "\\";

	if( strCurRootPath == strVCPath )
	{
		m_strCurRelativePath = _T(".\\");
		goto EXIT;
	}

	int i = 0, len = 0, nSepCount = 0;
	while(strCurRootPath.GetAt(i) == strVCPath.GetAt(i))
	{
		if(strCurRootPath.GetAt(i) == '\\')
		{
			len = i + 1;
			nSepCount++;
		}
		i++;
	}

	// 如果第二个 '\\' 前的路径不同，则直接把相对路径置为 strCurRootPath
	if(nSepCount < 2)
	{
		m_strCurRelativePath = strCurRootPath;
		goto EXIT;
	}

	m_strCurRelativePath = "";
	m_strCurRelativeFilter = "";

	if(len < strVCPath.GetLength())
	{
		CString strRes = m_pConfig->m_strTargetPath;
		strRes.Delete(0, len-1);

		int i = -1;
		while(true)
		{
			i = strRes.Find('\\', i+1);
			if(i == -1)
				break;
			m_strCurRelativePath += _T("..\\");
		}
	}

	if(len < strCurRootPath.GetLength())
	{
		m_strCurRelativeFilter = strCurRootPath;
		m_strCurRelativeFilter.Delete(0, len);
		m_strCurRelativePath += m_strCurRelativeFilter;
		m_strCurRelativeFilter.Delete(m_strCurRelativeFilter.GetLength()-1, 1);
	}

EXIT:
	cout << "Current Root Path is: " << strCurRootPath << endl;
	cout << "Current Relative Path is: " << m_strCurRelativePath << "\n\n";
}


MSXML2::IXMLDOMElementPtr VCXProjSync::addFile(MSXML2::IXMLDOMElementPtr pParentElement, CString strRelativePath, CString strRelativeFilter)
{
	if(bVCProj)
		return m_vc.addFile(pParentElement, strRelativePath);

	m_vcx.addFile(strRelativePath);
	MSXML2::IXMLDOMElementPtr pFile = m_vcxFilter.addFile(strRelativePath, strRelativeFilter);
	return pFile;
}


MSXML2::IXMLDOMElementPtr VCXProjSync::addFilter(MSXML2::IXMLDOMElementPtr pParentElement, const _variant_t attributeVal, CString strRelativeFilter)
{
	if(bVCProj)
		return m_vc.addFilter(pParentElement, attributeVal);

	return m_vcxFilter.addFilter(strRelativeFilter);
}

int VCXProjSync::addAllFiles(MSXML2::IXMLDOMElementPtr pParentElement, const CString strCurPath, const CString strCurRelativePath, const CString strCurRelativeFilter)
{
	int iChildren = 0; // 用于记录子节点的个数，如果为0，则删除父节点
	MSXML2::IXMLDOMElementPtr pNode;
	CString strPath, strRelativePath, strRelativeFilter, FilterName;

	strPath = strCurPath + _T("*.*");

	HANDLE hFind = FindFirstFile(strPath, &m_stFindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	while(TRUE)
	{
		if(m_stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(m_stFindFileData.cFileName[0] == '.')
				goto FIND_NEXT;

			if(m_pConfig->isOneRejectFolder(m_stFindFileData.cFileName))
				goto FIND_NEXT;

			strPath = strCurPath + m_stFindFileData.cFileName + "\\";
			strRelativePath = strCurRelativePath + m_stFindFileData.cFileName + "\\";

			if(m_pConfig->checkPath(m_pCurAcceptPath, strPath)) // 检查strPath是否为“拒绝路径”
			{
				if(strCurRelativeFilter.GetLength() == 0)
					strRelativeFilter = m_stFindFileData.cFileName;
				else
					strRelativeFilter = strCurRelativeFilter + "\\" + m_stFindFileData.cFileName;

				pNode = addFilter(pParentElement, m_stFindFileData.cFileName, strRelativeFilter);
				if(addAllFiles(pNode, strPath, strRelativePath, strRelativeFilter))
					iChildren++;
			}
		}
		else
		{
			if(m_pConfig->checkFile(m_pCurAcceptPath, strCurPath, m_stFindFileData.cFileName))
			{
				iChildren++;
				strPath = strCurRelativePath + m_stFindFileData.cFileName;
				addFile(pParentElement, strPath, strCurRelativeFilter);
			}
		}

FIND_NEXT:
		if(!FindNextFile(hFind, &m_stFindFileData))
			break;
	}

	FindClose(hFind);

	if(iChildren == 0)
		DeleteNode(pParentElement);

	return iChildren;
}

void VCXProjSync::run()
{
	cout << "\nStart synchronising..." << endl;

	MSXML2::IXMLDOMElementPtr pNode;

	UNIT* FilePtr = m_pConfig->m_pAcceptFile;

	while(FilePtr && FilePtr->strData != "")
	{
		setRelativePath(FilePtr->strData);

		addFile(m_vc.m_pFilesElement, m_strCurRelativePath, m_strCurRelativeFilter);

		FilePtr = FilePtr->pNext;
	}

	m_pCurAcceptPath = m_pConfig->m_pAcceptPath;

	while(m_pCurAcceptPath && m_pCurAcceptPath->strPath != "")
	{
		m_strCurRootPath = m_pCurAcceptPath->strPath;
		addPathSep(m_strCurRootPath);

		setRelativePath(m_strCurRootPath);

		if(bVCProj)
			addAllFiles(m_vc.m_pFilesElement, m_strCurRootPath, m_strCurRelativePath, m_strCurRelativeFilter);
		else if(addAllFiles(m_vcxFilter.m_pFilesGroup, m_strCurRootPath, m_strCurRelativePath, m_strCurRelativeFilter))
			m_vcxFilter.addFilterX(m_strCurRelativeFilter);

		m_pCurAcceptPath = m_pCurAcceptPath->pNext;
	}
}
