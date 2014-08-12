#include "ConfigFile.h"
#include "Common.h"
#include <assert.h>
#include <string.h>
#include <iostream>
using namespace std;

#define LINE_SIZE      100


ConfigFile::ConfigFile(TCHAR *pFullFile) : m_strConfigFile(pFullFile), CXMLParser(pFullFile), m_pRejectFolder(NULL), m_pRejectFolderType(NULL), m_pAcceptFileType(NULL), m_pRejectFileType(NULL)
{
	if(!m_pRootElement)
		throw "Error config file!";

	MSXML2::IXMLDOMElementPtr pElement;
	
	// target file, name and path
	pElement = m_pRootElement->selectSingleNode("TargetFile");
	m_strTargetFile = (char*)(_bstr_t)pElement->getAttribute("value");
	
	sepPathFile(m_strConfigFile, m_strConfigPath, m_strConfigName);
	if(m_strConfigPath.GetLength() == 0)
	{
		char buf[1000];
		GetCurrentDirectory(1000, buf);  //得到当前工作路径
		m_strConfigPath = buf;
		// cout << m_strConfigPath << endl;
	}

	sepPathFile(m_strTargetFile, m_strTargetPath, m_strTargetName);
	if(m_strTargetPath.GetLength() == 0)
	{
		m_strTargetPath = m_strConfigPath;
		m_strTargetFile = m_strTargetPath + '\\' + m_strTargetName;
	}

	cout << "Target file path is: " << m_strTargetPath << endl;
	cout << "Target file name is: " << m_strTargetName << endl;
	
	// Reject folder type: “., +, @” 等开头的文件夹
	m_pRejectFolderType = new UNIT;

	CString strTemp;

	pElement = m_pRootElement->selectSingleNode("RejectFolder");
	if(pElement)
	{
		m_pRejectFolder = new UNIT;
		strTemp = (char*)(_bstr_t)pElement->getAttribute("value");
		add2List(m_pRejectFolder, strTemp);
	}
	
	pElement = m_pRootElement->selectSingleNode("AcceptFileType");
	if(pElement)
	{
		m_pAcceptFileType = new UNIT;
		strTemp = (char*)pElement->Gettext();
		add2List(m_pAcceptFileType, strTemp, ".");
	}
	
	pElement = m_pRootElement->selectSingleNode("RejectFileType");
	if(pElement)
	{
		m_pRejectFileType = new UNIT;
		strTemp = (char*)pElement->Gettext();
		//pElement->get_attributes
		add2List(m_pRejectFileType, strTemp, ".");
	}

	m_pAcceptFile = new UNIT;
	add2List(m_pAcceptFile, m_pRootElement, "AcceptFile");

	m_pAcceptPath = new ACCEPT_PATH;
	add2List(m_pAcceptPath, m_pRootElement, "AcceptPath");
}

void deleteLink(UNIT* p)
{
	UNIT* h;
	while(p)
	{
		h = p->pNext;
		delete p;
		p = h;
	}
}

void deleteLink(ACCEPT_PATH* p)
{
	ACCEPT_PATH* h;
	while(p)
	{
		h = p->pNext;
		delete p;
		p = h;
	}
}

ConfigFile::~ConfigFile()
{
	deleteLink(m_pRejectFolder);
	deleteLink(m_pRejectFolderType);
	deleteLink(m_pAcceptFileType);
	deleteLink(m_pRejectFileType);
	deleteLink(m_pAcceptFile);
	deleteLink(m_pAcceptPath);
}

void ConfigFile::add2RejectFolderList(CString strTokens)
{
	add2List(m_pRejectFolder, strTokens);
}

void ConfigFile::add2RejectFolderTypeList(CString strTokens)
{
	add2List(m_pRejectFolderType, strTokens);
}

void ConfigFile::add2List(UNIT* pUnit, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName)
{
	bool bPath = false;
	if(strNodeName.Right(4) == "Path")
		bPath = true;

	MSXML2::IXMLDOMNodeListPtr pNodeList = pParentElement->selectNodes(strNodeName.GetString());
	for(int index = 0; index < pNodeList->Getlength(); index++)
	{
		MSXML2::IXMLDOMElementPtr pElem = (MSXML2::IXMLDOMElementPtr) pNodeList->Getitem(index);

		pUnit->strData = (char*)(_bstr_t)pElem->getAttribute("value");
		if(bPath) addPathSep(pUnit->strData);

		pUnit->pNext = new UNIT;
		pUnit = pUnit->pNext;
	}
}

void ConfigFile::add2AcceptPathList(int iLen, TCHAR** pPathList)
{
	// Mordecai Lee, 2010/09/25, Sat., 11:26:35
	ACCEPT_PATH* pPath = m_pAcceptPath;

	for(int i = 0; i < iLen; i++)
	{
		pPath->strPath = pPathList[i];
		
		pPath->pNext = new ACCEPT_PATH;
		pPath = pPath->pNext;
	}
}

void ConfigFile::add2List(ACCEPT_PATH* pPath, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName)
{
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;
	MSXML2::IXMLDOMNodePtr pAttr;
	MSXML2::IXMLDOMNodeListPtr pNodeList = pParentElement->selectNodes(strNodeName.GetString());

	CString strName, strValue;
	for(int index = 0; index < pNodeList->Getlength(); index++)
	{
		MSXML2::IXMLDOMElementPtr pElem = (MSXML2::IXMLDOMElementPtr) pNodeList->Getitem(index);
		pElem->get_attributes(&pAttrMap);

		for(int idx = 0; idx < pAttrMap->Getlength(); idx++) //属性值的循环  
		{   
			pAttrMap->get_item(idx,&pAttr);   
			strName = (char*)(_bstr_t)pAttr->nodeName;
			strValue = (char*)(_bstr_t)pAttr->nodeValue;

			if(strName == "value")
			{
				pPath->strPath = strValue;
				addPathSep(pPath->strPath);
			}else if(strName == "order")
			{
				add2List(pPath->pOrder, strValue);
			}
		}

		add2List(pPath->pRejectFile, pElem, "RejectFile");
		add2List(pPath->pRejectPath, pElem, "RejectPath");
		
		pPath->pNext = new ACCEPT_PATH;
		pPath = pPath->pNext;
	}
}

void ConfigFile::add2List(UNIT* pUnit, CString strTokens, CString strPre)
{
	if(strTokens == "")
		return;

	if(pUnit->strData == ".*")
		return;

	CString resToken;
	int curPos = 0;

	while(pUnit->pNext) pUnit = pUnit->pNext;
	if(pUnit->strData != "")
	{
		pUnit->pNext = new UNIT;
		pUnit = pUnit->pNext;
	}

	resToken = strTokens.Tokenize(", ", curPos);
	while (true)
	{
		pUnit->strData = strPre + resToken;

		resToken = strTokens.Tokenize(_T(", "), curPos);
		if(resToken == "")
			break;

		pUnit->pNext = new UNIT;
		pUnit = pUnit->pNext;
	}
}

/**************************** check *********************************/
bool ConfigFile::checkFolder(UNIT* ptr, const CString strFolder)
{
	while(ptr && ptr->strData != "")
	{
		if(	ptr->strData.CompareNoCase(strFolder) == 0 )
			return true;

		ptr = ptr->pNext;
	}
	
	return false;
}

bool ConfigFile::isOneRejectFolder(const CString strFolder)
{
	return checkFolder(m_pRejectFolderType, strFolder[0]) || checkFolder(m_pRejectFolder, strFolder);
}

bool ConfigFile::isOneOrderFolder(ACCEPT_PATH* pAcceptPath, const CString strCurRootPath, const CString strFolder)
{
	if( pAcceptPath->strPath == strCurRootPath )
		return checkFolder(pAcceptPath->pOrder, strFolder);
	return false;
}

bool ConfigFile::checkFileType(const CString strFileName, UNIT* ptr)
{
	while(ptr && ptr->strData != "")
	{
		if(checkFileType(strFileName, ptr->strData))
			return true;

		ptr = ptr->pNext;
	}
	
	return false;
}

bool ConfigFile::checkFileType(CString strFileName, CString strFileType)
{
	strFileName.Trim();
	strFileType.Trim();

	if(	strFileType == ".*" || strFileType.CompareNoCase(strFileName.Right(strFileType.GetLength())) == 0 )
		return true;

	return false;
}

bool ConfigFile::checkFile(ACCEPT_PATH* pAcceptPath, const CString strPath, CString strFileName)
{
	CString strFullFile = strPath;
	addPathSep(strFullFile);
	strFullFile += strFileName;

	if(strFileName.Left(1) == '.')
		return false;

	UNIT* ptr = pAcceptPath->pRejectFile;
	while(ptr)
	{
		if( strFullFile.CompareNoCase(ptr->strData) == 0 )
			return false;

		ptr = ptr->pNext;
	}

	//strFileName.MakeLower();

	if(checkFileType(strFileName, m_pAcceptFileType))
		return true;

	if(checkFileType(strFileName, m_pRejectFileType))
		return false;

	return true;
}

/* 检查strPath是否为“拒绝路径” */
bool ConfigFile::checkPath(ACCEPT_PATH* pAcceptPath, const CString strPath)
{
	UNIT* ptr = pAcceptPath->pRejectPath;

	do{
		if(strPath.CompareNoCase(ptr->strData) == 0)
			return false;
	}while(ptr = ptr->pNext);

	return true;
}
