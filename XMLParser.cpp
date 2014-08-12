#include "XMLParser.h"

#include <atlstr.h>
#include <iostream>
using namespace std;

#define XML_PARSER_ERROR  "�޷�����DOMDocument���������Ƿ�װ��MS XML Parser ���п�!"

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
	// ��ʼ�� COM ����
	CoInitialize(NULL); 
	HRESULT hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument)); 

	if(!SUCCEEDED(hr))
	{
		cout << XML_PARSER_ERROR << endl;  
		return;
	}

	// ���� xml �ļ�
	pDoc->load(lpXMLFile);

	// ȡ�ø�Ԫ��
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
	* ����createElement()���������ڵ㣬��ѡ��createNode()������
	* ��Ϊ�����ڵ�����xmlns��name space������ʱ��
	* ��createElement()�������ӽڵ��Զ��ػ����xmlns=""���������ԣ�
	* �������������� .vcproj �� .vcxproj ��������ġ�
	*
	* �������ԭ�򡿣�
	*   �����ڵ����XMLNS����ʱ���ӽڵ����ָ��XMLNS���ԣ�
	*   ���ǣ����ӽڵ��XMLNS�����븸�ڵ������ռ���ͬʱ���ӽڵ㲻��ʾXMLNS���ԡ�
	*
	* �������������
	*   �� pParentElement->GetnamespaceURI(); ��ȡ���ڵ�� namespace
	*   �������ã�pParentElement->getAttribute(_T("xmlns"));
	*   ��Ϊ�����ڵ��п��ܲ����� xmlns ���ԡ�
	* 
	* �����Եķ�����
	*   1. �� MSXML2::IXMLDOMNamedNodeMapPtr xmlnsx = pParentElement->Getattributes(); ��ȡ�������ԣ�����֪����ô��ȡ��
	*        long x = xmlnsx->Getlength();
	*	      for(int i = 0; i < x; i++)
	*	         MSXML2::IXMLDOMNodePtr q = xmlnsx->Getitem(i);
	*   2. ����ɾ����pNewChildElement->removeAttribute("xmlns"); ���������� getAttribute(_T("xmlns")) ͬ�������⣬��xmlns���Բ�����ʱ��
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