#include "VCProjSync.h"
#include "Common.h"
#include <iostream>
using namespace std;

#define REJECT_FILETYPE_LIST    "dsp, ncb, vcproj, user, suo, aps"

/*
  .vcproj 文件的 XML 结构如下：
	<VisualStudioProject ...>
		<Files>
			<Filter Name="+Classifier">
				<Filter Name="+RSC">
					<File RelativePath="..\FaceRecognition\Main\+Classifier\+RSC\Demo_RSC_AR_disguise.m">
					</File>
				</Filter>
			</Filter>
		</Files>
	</VisualStudioProject>
*/

VCProjSync::VCProjSync(ConfigFile* pConfigFile) : CXMLParser((TCHAR*)pConfigFile->m_strTargetFile.GetString())
{
	if(!m_pRootElement)
		throw "Error .vcproj file";

	m_pConfig = pConfigFile;

	// 处理节点
	DeleteNode(m_pRootElement, _T("Files"));
	DeleteNode(m_pRootElement, _T("Globals"));

	m_pFilesElement = AddNode(m_pRootElement, _T("Files"), _T(""));
	AddNode(m_pRootElement, _T("Globals"), _T(""));

	pConfigFile->add2List(pConfigFile->m_pRejectFileType, REJECT_FILETYPE_LIST);
}

VCProjSync::~VCProjSync(void)
{
	m_pDoc->save(m_lpXMLFile);
	cout << "Synchronise " << m_pConfig->m_strTargetName << " successfully!" << endl;
}

void VCProjSync::run()
{
	cout << "\nStart synchronising..." << endl;

	MSXML2::IXMLDOMElementPtr pFilter;

	UNIT* FilePtr = m_pConfig->m_pAcceptFile;

	while(FilePtr && FilePtr->strData != "")
	{
		setRelativePath(FilePtr->strData);

		MSXML2::IXMLDOMElementPtr pFile = AddNode(m_pFilesElement, "File", "");
		pFile->setAttribute("RelativePath", m_strCurRelativePath.GetString());

		FilePtr = FilePtr->pNext;
	}

	m_pCurAcceptPath = m_pConfig->m_pAcceptPath;

	while(m_pCurAcceptPath && m_pCurAcceptPath->strPath != "")
	{
		m_strCurRootPath = m_pCurAcceptPath->strPath;
		addPathSep(m_strCurRootPath);

		setRelativePath(m_strCurRootPath);
		addAllFiles(m_strCurRootPath, m_pFilesElement);

		m_pCurAcceptPath = m_pCurAcceptPath->pNext;
	}
}

void VCProjSync::setRelativePath(CString strCurRootPath)
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
		CString strRes = strCurRootPath;
		strRes.Delete(0, len);
		m_strCurRelativePath += strRes;
	}

EXIT:
	cout << "Current Root Path is: " << strCurRootPath << endl;
	cout << "Current Relative Path is: " << m_strCurRelativePath << "\n\n";
}

/*
	.vcproj 文件的<Filter>节点的 XML 结构如下：

	<Filter Name="+Classifier">
		<Filter Name="+RSC">
			<File RelativePath="..\FaceRecognition\Main\+Classifier\+RSC\Demo_RSC_AR_disguise.m">
			</File>
		</Filter>
	</Filter>
*/
void VCProjSync::addAllFiles(const CString strCurPath, MSXML2::IXMLDOMElementPtr pParentElement)
{	
	int iChildren = 0;
	MSXML2::IXMLDOMElementPtr pFilter;
	CString strPath, strCurRelativePath;

	strPath = strCurPath + _T("*.*");
	strCurRelativePath = m_strCurRelativePath;

	if(strCurPath.GetLength() > m_strCurRootPath.GetLength())
	{
		CString strRes = strCurPath;
		strRes.Delete(0, m_strCurRootPath.GetLength());
		strCurRelativePath += strRes;
	}

	HANDLE hFind = FindFirstFile(strPath, &m_stFindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	while(TRUE)
	{
		if(m_stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(m_stFindFileData.cFileName[0] == '.')
				goto FIND_NEXT;

			if(m_pConfig->isOneRejectFolder(m_stFindFileData.cFileName))
				goto FIND_NEXT;

			strPath = strCurPath + m_stFindFileData.cFileName;
			strPath += "\\";

			if(m_pConfig->checkPath(m_pCurAcceptPath, strPath))
			{
				iChildren++;

				pFilter = AddNode(pParentElement, "Filter", "");
				pFilter->setAttribute("Name", m_stFindFileData.cFileName);

				addAllFiles(strPath, pFilter);
			}
		}
		else
		{
			if(m_pConfig->checkFile(m_pCurAcceptPath, strCurPath, m_stFindFileData.cFileName))
			{
				iChildren++;
				strPath = strCurRelativePath + m_stFindFileData.cFileName;

				AddNode(pParentElement, "File", "")->setAttribute("RelativePath", strPath.GetString());
			}
		}

FIND_NEXT:
		if(!FindNextFile(hFind, &m_stFindFileData))
			break;
	}

	FindClose(hFind);

	if(iChildren == 0)
		DeleteNode(pParentElement);
}
