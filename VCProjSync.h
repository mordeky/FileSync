/**
 * CodeAuthor:  Mordecai Lee
 * CreatedDate: 2009/06/22, Monday, 16:02:34
 * Description: 用以生成 mypathdef.m
 *
 * References:
 * 1. VC遍历文件夹下所有文件和文件夹. http://www.diybl.com/course/3_program/vc/vc_js/20081010/149673.html
 * 2. XML编程实例. http://sadier.cnblogs.com/archive/2006/05/09/99875.html
 */

#pragma once

#include "XMLParser.h"
#include "ConfigFile.h"

#define RELATIVE_POINT_LEN  50

class VCProjSync : public CXMLParser
{
private:
	MSXML2::IXMLDOMElementPtr m_pFilesElement;

	ConfigFile* m_pConfig;
	ACCEPT_PATH* m_pCurAcceptPath;

	CString m_strCurRootPath;
	CString m_strCurRelativePath;

	WIN32_FIND_DATA m_stFindFileData;

public:
	VCProjSync(ConfigFile* pConfigFile);
	~VCProjSync(void);
	void run();

private:
	// 如果以 strCurRootPath 为路径，必须以"\"结尾；否则认为其为文件
	void setRelativePath(CString strCurRootPath);
	void addAllFiles(const CString strCurPath, MSXML2::IXMLDOMElementPtr pParentElement);
};
