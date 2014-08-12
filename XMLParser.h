#pragma once

/*#include "msxml.h"  
//#import "msxml.dll"*/
#import "msxml3.dll"

#include <atlstr.h>
#include <atlbase.h> // Includes CComVariant and CComBSTR.

//#include "msxml2.h"
//using namespace MSXML2;

class CXMLParser
{
public:
	CString m_strFile;
	MSXML2::IXMLDOMDocumentPtr m_pDoc;
	MSXML2::IXMLDOMElementPtr m_pRootElement;

public:
	CXMLParser(void);
	CXMLParser(TCHAR* lpXMLFile);
	CXMLParser(CString strFile, CString strFileType);

	~CXMLParser(void);

public:
	void loadXML(CString strXMLFile, MSXML2::IXMLDOMDocumentPtr &pDoc, MSXML2::IXMLDOMElementPtr &pRootElement);
	void loadXML(TCHAR* lpXMLFile, MSXML2::IXMLDOMDocumentPtr &pDoc, MSXML2::IXMLDOMElementPtr &pRootElement);
	
	MSXML2::IXMLDOMElementPtr AddNode(
		MSXML2::IXMLDOMElementPtr pParentElement,
		_bstr_t sChildName, _bstr_t sChildValue);
	static MSXML2::IXMLDOMElementPtr AddNode(
		MSXML2::IXMLDOMDocumentPtr &pDoc, 
		MSXML2::IXMLDOMElementPtr pParentElement,
		_bstr_t sChildName, _bstr_t sChildValue);

	static void DeleteNode(MSXML2::IXMLDOMElementPtr pChildElement);
	void DeleteNode(MSXML2::IXMLDOMElementPtr pParentElement, _bstr_t sChildName);
	void DeleteNodeListWithoutAttributes(MSXML2::IXMLDOMElementPtr pParentElement, _bstr_t sChildName);

	void save()
	{
		m_pDoc->save((TCHAR*)m_strFile.GetString());
	}
};

/* ---------------------------------
 * 循环获取节点属性的方法：之一
 * ---------------------------------

MSXML2::IXMLDOMElementPtr pElem = (MSXML2::IXMLDOMElementPtr) pNode;
CString test = (char*)(_bstr_t)pElem->getAttribute("value");

/* ---------------------------------
 * 循环获取节点属性的方法：之二
 * ---------------------------------

	MSXML2::IXMLDOMNodeListPtr pNodeList = pParentElement->selectNodes(strNodeName.GetString());
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrMap = NULL;

	MSXML2::IXMLDOMNodePtr pNode, pAttr;

	long nListNum, nAttrNum;
	CString strName, strValue;

	nListNum = pNodeList->Getlength();
	for(int index = 0; index < nListNum; index++)
	{
		pNode = pNodeList->Getitem(index);
		//BSTR bstr = pNode->Gettext(); //取得pNode的节点值

		pNode->get_attributes(&pAttrMap);
		nAttrNum = pAttrMap->Getlength();
		
		for(int idx = 0; idx < nAttrNum; idx++) //属性值的循环  
		{   
			pAttrMap->get_item(idx,&pAttr);   
			strName = (char*)(_bstr_t)pAttr->nodeName;
			strValue = (char*)(_bstr_t)pAttr->nodeValue;
			
			pUnit = pUnit->pNext;
		}
	}
*/