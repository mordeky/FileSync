/**
 * CodeAuthor:  Mordecai Lee
 * CreatedDate: 2009/06/22, Monday, 16:02:34
 * Revision  1: 2014-08-12, Tue, 13:34:00
 * Description: 用以同步指定的文件夹和文件到 .vcproj / (.vcxproj + .vcxproj.filters) 文件
 *
 * References:
 * 1. VC遍历文件夹下所有文件和文件夹. http://www.diybl.com/course/3_program/vc/vc_js/20081010/149673.html
 * 2. XML编程实例. http://sadier.cnblogs.com/archive/2006/05/09/99875.html
 */

#pragma once

#include "XMLParser.h"
#include "ConfigFile.h"
#include "VCXML.h"

#define RELATIVE_POINT_LEN  50

class VCXProjSync : public CXMLParser
{
private:
	bool bVCProj;

	VC m_vc;
	VCX m_vcx;
	VCXFilter m_vcxFilter;

	ConfigFile* m_pConfig;
	ACCEPT_PATH* m_pCurAcceptPath;

	CString m_strCurRootPath;
	CString m_strCurRelativePath;
	CString m_strCurRelativeFilter;

	WIN32_FIND_DATA m_stFindFileData;

public:
	VCXProjSync(ConfigFile* pConfigFile);
	~VCXProjSync(void);
	void run();

private:
	void init_vc();
	void init_vcx();
	void init_vcxFilter();

	// 如果以 strCurRootPath 为路径，必须以"\"结尾；否则认为其为文件
	//CString trimFilter(CString strFilter);
	void setRelativePath(CString strCurRootPath);
	int addAllFiles(MSXML2::IXMLDOMElementPtr pParentElement, const CString strCurPath, const CString strCurRelativePath, const CString strCurRelativeFilter);
	MSXML2::IXMLDOMElementPtr addFile(MSXML2::IXMLDOMElementPtr pParentElement, CString strRelativePath, CString strRelativeFilter);
	MSXML2::IXMLDOMElementPtr addFilter(MSXML2::IXMLDOMElementPtr pParentElement, const _variant_t attributeVal, CString strRelativeFilter);
};
