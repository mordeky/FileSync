#include "XMLParser.h"

#include <atlstr.h>
#include <iostream>
using namespace std;

#define XML_PARSER_ERROR  "无法创建DOMDocument对象，请检查是否安装了MS XML Parser 运行库!"

CXMLParser::CXMLParser(TCHAR* lpXMLFile) : m_strFile(lpXMLFile)
{
	loadXML(m_strFile, m_pDoc, m_pRootElement);
}

CXMLParser::CXMLParser(CString strFile, CString strFileType) : m_strFile(strFile)
{
	loadXML(m_strFile, m_pDoc, m_pRootElement);

	if(!m_pRootElement)
	{
		cerr << endl
			<< "Error! The following file might not exist, or the .xml file includes Chinese:" << endl
			<< "\t" << strFile << endl
			<< "Please check it!" << endl
			<< endl;
		exit(1);
		//throw sprintf("Error %s File!", strFileType.GetString());
	}
}

CXMLParser::CXMLParser(void)
{
}

CXMLParser::~CXMLParser(void)
{
}

void CXMLParser::loadXML(CString strXMLFile, MSXML2::IXMLDOMDocumentPtr &pDoc, MSXML2::IXMLDOMElementPtr &pRootElement)
{
	loadXML((TCHAR*)strXMLFile.GetString(), pDoc, pRootElement);
}

void CXMLParser::loadXML(TCHAR* lpXMLFile, MSXML2::IXMLDOMDocumentPtr &pDoc, MSXML2::IXMLDOMElementPtr &pRootElement)
{
	// 初始化 COM 环境
	CoInitialize(NULL); 
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument)); 

	if(!SUCCEEDED(hr))
	{
		cout << XML_PARSER_ERROR << endl;  
		return;
	}

	// 加载 xml 文件
	pDoc->load(lpXMLFile);

	// 取得根元素
	pRootElement = pDoc->GetdocumentElement();
}

MSXML2::IXMLDOMElementPtr CXMLParser::AddNode(
	MSXML2::IXMLDOMElementPtr pParentElement,
	_bstr_t sChildName, _bstr_t sChildValue)
{
	return AddNode(m_pDoc, pParentElement, sChildName, sChildValue);
}

MSXML2::IXMLDOMElementPtr CXMLParser::AddNode(
	MSXML2::IXMLDOMDocumentPtr &pDoc, 
	MSXML2::IXMLDOMElementPtr pParentElement,
	_bstr_t sChildName, _bstr_t sChildValue)
{
	MSXML2::IXMLDOMElementPtr pNewChildElement;

	/**
	* 不用createElement()方法创建节点，而选用createNode()方法，
	* 因为当父节点中有xmlns（name space）属性时，
	* 用createElement()创建的子节点自动地会加上xmlns=""这样的属性，
	* 而这样的属性是 .vcproj 或 .vcxproj 所不允许的。
	*
	* 【问题的原因】：
	*   当父节点具有XMLNS属性时，子节点必须指定XMLNS属性，
	*   但是，当子节点的XMLNS属性与父节点命名空间相同时，子节点不显示XMLNS属性。
	*
	* 【解决方案】：
	*   用 pParentElement->GetnamespaceURI(); 获取父节点的 namespace
	*   但不能用：pParentElement->getAttribute(_T("xmlns"));
	*   因为：父节点中可能不存在 xmlns 属性。
	* 
	* 【尝试的方案】
	*   1. 用 MSXML2::IXMLDOMNamedNodeMapPtr xmlnsx = pParentElement->Getattributes(); 获取素有属性，但不知道怎么读取：
	*        long x = xmlnsx->Getlength();
	*	      for(int i = 0; i < x; i++)
	*	         MSXML2::IXMLDOMNodePtr q = xmlnsx->Getitem(i);
	*   2. 尝试删除：pNewChildElement->removeAttribute("xmlns"); 但会遇到与 getAttribute(_T("xmlns")) 同样的问题，当xmlns属性不存在时。
	*/

	//pNewChildElement = m_pDoc->createElement(sChildName);

	_bstr_t xmlns = pParentElement->GetnamespaceURI();
	pNewChildElement = pDoc->createNode("element", sChildName, xmlns);

	//if(sChildValue.length())
	pNewChildElement->Puttext(sChildValue);

	pParentElement->appendChild(pNewChildElement);

	return pNewChildElement;
}

void CXMLParser::DeleteNode(MSXML2::IXMLDOMElementPtr pParentElement, _bstr_t sChildName)
{
	MSXML2::IXMLDOMElementPtr pElement = pParentElement->selectSingleNode(sChildName);
	if(pElement)
		pParentElement->removeChild(pElement);
}

void CXMLParser::DeleteNode(MSXML2::IXMLDOMElementPtr pChildElement)
{
	MSXML2::IXMLDOMElementPtr pParentNode = pChildElement->GetparentNode();
	if(pParentNode)
		pParentNode->removeChild(pChildElement);
}

void CXMLParser::DeleteNodeListWithoutAttributes(MSXML2::IXMLDOMElementPtr pParentElement, _bstr_t sChildName)
{
	MSXML2::IXMLDOMNodeListPtr pNodeList = pParentElement->selectNodes(sChildName);
	MSXML2::IXMLDOMNodePtr pChildNode;
	MSXML2::IXMLDOMNamedNodeMapPtr pAttributes;
	for(int i = 0; i < pNodeList->length; i++)
	{	
		pChildNode = pNodeList->Getitem(i);

		pAttributes = pChildNode->Getattributes();

		if(pAttributes->length == 0)
			pParentElement->removeChild(pChildNode);

		/*
		MSXML2::IXMLDOMNodePtr pDOMAttr = attributes->Getitem(0);
		_bstr_t x = pDOMAttr->GetnodeName();
		_variant_t y = pDOMAttr->GetnodeTypedValue();
		if(strcmpi(x, "Label") == 0)
		continue;
		*/
	}
}