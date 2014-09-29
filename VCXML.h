/**
* CodeAuthor:  Mordecai Lee
* CreatedDate: 2014-08-12, Tue, 12:26:38
* Description: 用以处理 .vcxproj 文件
*
*/

#pragma once

#include "XMLParser.h"
#include <atlstr.h>

#include <iostream>
using namespace std;

class VC : public CXMLParser
{
public:
	MSXML2::IXMLDOMElementPtr m_pFilesElement;

public:
	VC(){};
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
	VC(CString strFile) : CXMLParser(strFile, ".vcproj")
	{
		// 处理节点
		DeleteNode(m_pRootElement, _T("Files"));
		DeleteNode(m_pRootElement, _T("Globals"));

		m_pFilesElement = AddNode(m_pRootElement, _T("Files"), _T(""));
		AddNode(m_pRootElement, _T("Globals"), _T(""));
	}

	~VC(void){}

public:
	/*
	  .vcproj 文件的<Filter>和<File>节点的 XML 结构如下：

		<Filter Name="+Classifier">
			<Filter Name="+RSC">
				<File RelativePath="..\FaceRecognition\Main\+Classifier\+RSC\Demo_RSC_AR_disguise.m">
				</File>
			</Filter>
		</Filter>
	*/

	MSXML2::IXMLDOMElementPtr addFile(MSXML2::IXMLDOMElementPtr pParentElement, CString strRelativePath)
	{
		if(strRelativePath.GetLength() == 0)
			return NULL;

		MSXML2::IXMLDOMElementPtr pFile = AddNode(pParentElement, "File", "");
		pFile->setAttribute("RelativePath", strRelativePath.GetString());
		return pFile;
	}

	
	MSXML2::IXMLDOMElementPtr addFilter(MSXML2::IXMLDOMElementPtr pParentElement, const _variant_t attributeVal)
	{
		MSXML2::IXMLDOMElementPtr pFilter = AddNode(pParentElement, "Filter", "");
		pFilter->setAttribute("Name", attributeVal);
		return pFilter;
	}
};

/*****************************************************************************/
class VCX : public CXMLParser
{
public:
	MSXML2::IXMLDOMElementPtr m_pFilesGroup;

public:
	VCX(){}

	VCX(CString strFile) : CXMLParser(strFile, ".vcxproj")
	{
		init();
	}

	VCX(CString strFile, CString strFileType) : CXMLParser(strFile, strFileType)
	{
		init();
	}

	void init()
	{
		DeleteNodeListWithoutAttributes(m_pRootElement, _T("ItemGroup"));
		m_pFilesGroup = AddNode(m_pRootElement, _T("ItemGroup"), _T(""));
	}

	~VCX(void)
	{
		m_pFilesGroup.Release();
	}

public:
	/*
	  .vcproj 文件的<File>节点的 XML 结构如下：
		<!-- File Group -->
		<ItemGroup>
			<None Include="..\testOccIdeas\+RecFig\fuseAction.m" />

			<None Include="..\testOccIdeas\+RecFig\showTrain.m">
			</None>

			<None Include="..\testOccIdeas\+RecFig\show_s.m">
			</None>
		</ItemGroup>
	*/
	MSXML2::IXMLDOMElementPtr addFile(CString strRelativePath)
	{
		if(strRelativePath.GetLength() == 0)
			return NULL;

		MSXML2::IXMLDOMElementPtr pFile = AddNode(m_pFilesGroup, "None", "");
		pFile->setAttribute("Include", strRelativePath.GetString());
		return pFile;
	}
};


/*****************************************************************************/
class VCXFilter : public VCX
{
public:
	MSXML2::IXMLDOMElementPtr m_pFiltersGroup;

public:
	VCXFilter(){}

	VCXFilter(CString strFileName) : VCX(strFileName, ".vcxproj.filters")
	{
		m_pFiltersGroup = AddNode(m_pRootElement, _T("ItemGroup"), _T(""));
	}

	~VCXFilter()
	{
		m_pFiltersGroup.Release();
	}

public:
	
	/*
	  .vcproj.filters 文件的<File>节点的 XML 结构如下：
		<!-- File Group -->
		<ItemGroup>
			<None Include="..\testOccIdeas\+RecFig\fuseAction.m" />

			<None Include="..\testOccIdeas\+RecFig\showTrain.m">
				<Filter>NewFilter1</Filter>
			</None>

			<None Include="..\testOccIdeas\+RecFig\show_s.m">
				<Filter>NewFilter1\NewFilter2</Filter>
			</None>
		</ItemGroup>
	*/
	MSXML2::IXMLDOMElementPtr addFile(CString strRelativePath, CString strRelativeFilter)
	{
		if(strRelativePath.GetLength() == 0)
			return NULL;

		MSXML2::IXMLDOMElementPtr pFile = AddNode(m_pFilesGroup, "None", "");
		pFile->setAttribute("Include", strRelativePath.GetString());

		if(strRelativeFilter.GetLength())
			AddNode(pFile, "Filter", strRelativeFilter.GetString());

		return pFile;
	}
	
	/*
	   .vcproj.filters 文件的<Filter>节点的 XML 结构如下：
		<!-- Filter Group -->
		<ItemGroup>
			<Filter Include="NewFilter1">
				<Extensions>cpp;c</Extensions>
			</Filter>
			<Filter Include="NewFilter1\NewFilter2">
			</Filter>
		</ItemGroup>
	*/
	MSXML2::IXMLDOMElementPtr addFilter(CString strRelativeFilter)
	{
		if(strRelativeFilter.GetLength() == 0)
			return NULL;

		MSXML2::IXMLDOMElementPtr pFilter = AddNode(m_pFiltersGroup, "Filter", "");
		pFilter->setAttribute("Include", strRelativeFilter.GetString());
		return pFilter;
	}

	/**
	 * 把strRelativeFilter所包含的所有Filter都添加进来
	 * 例如：strRelativeFilter = "a111\\b123\\c124";
	 * 将其分解为如下三个Filter：
	 *   <Filter Include="a111"/>
	 *   <Filter Include="a111\b123"/>
	 *   <Filter Include="a111\b123\c124"/>
	 */
	void addFilterX(CString strRelativeFilter)
	{
		if(strRelativeFilter.GetLength() == 0)
			return;

		int a = 0;
		while(a != -1)
		{
			a = strRelativeFilter.Find('\\', a+1);

			if(a == -1)
				break;

			addFilter(strRelativeFilter.Left(a));
		}
		addFilter(strRelativeFilter);
	}
};


/**
   .vcxproj.filters的XML结构如下：

		<Project ToolsVersion="4.0" ...>

			<!-- Filter Group -->
			<ItemGroup>
				<Filter Include="NewFilter1">
					<Extensions>cpp;c</Extensions>
				</Filter>
				<Filter Include="NewFilter1\NewFilter2">
				</Filter>
			</ItemGroup>

			<!-- File Group -->
			<ItemGroup>
				<None Include="..\testOccIdeas\+RecFig\fuseAction.m" />

				<None Include="..\testOccIdeas\+RecFig\showTrain.m">
					<Filter>NewFilter1</Filter>
				</None>

				<None Include="..\testOccIdeas\+RecFig\show_s.m">
					<Filter>NewFilter1\NewFilter2</Filter>
				</None>
			</ItemGroup>

		</Project>
 */
