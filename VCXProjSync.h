/**
 * CodeAuthor:  Mordecai Lee
 * CreatedDate: 2009/06/22, Monday, 16:02:34
 * Revision  1: 2014-08-12, Tue, 13:34:00
 * Description: ����ͬ��ָ�����ļ��к��ļ��� .vcproj / (.vcxproj + .vcxproj.filters) �ļ�
 *
 * References:
 * 1. VC�����ļ����������ļ����ļ���. http://www.diybl.com/course/3_program/vc/vc_js/20081010/149673.html
 * 2. XML���ʵ��. http://sadier.cnblogs.com/archive/2006/05/09/99875.html
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

	// ����� strCurRootPath Ϊ·����������"\"��β��������Ϊ��Ϊ�ļ�
	//CString trimFilter(CString strFilter);
	void setRelativePath(CString strCurRootPath);
	int addAllFiles(MSXML2::IXMLDOMElementPtr pParentElement, const CString strCurPath, const CString strCurRelativePath, const CString strCurRelativeFilter);
	MSXML2::IXMLDOMElementPtr addFile(MSXML2::IXMLDOMElementPtr pParentElement, CString strRelativePath, CString strRelativeFilter);
	MSXML2::IXMLDOMElementPtr addFilter(MSXML2::IXMLDOMElementPtr pParentElement, const _variant_t attributeVal, CString strRelativeFilter);
};
