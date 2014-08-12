#pragma once

#include "XMLParser.h"
#include <tchar.h>
#include <atlstr.h>

struct UNIT
{
	CString strData;
	UNIT *pNext;

	UNIT() : strData(""), pNext(NULL)
	{}
};

struct ACCEPT_PATH
{
	CString strPath;

	UNIT* pOrder;
	UNIT* pRejectFile;
	UNIT* pRejectPath;
	ACCEPT_PATH *pNext;

	ACCEPT_PATH() : strPath(""), pNext(NULL)
	{
		pOrder = new UNIT;
		pRejectFile = new UNIT;
		pRejectPath = new UNIT;
	}

	~ACCEPT_PATH()
	{
		delete pRejectFile;
		delete pRejectPath;
	}
};

class ConfigFile : public CXMLParser
{
public:
	CString m_strConfigFile; // 包含了全路径的xml配置文件
	CString m_strConfigPath; // xml配置文件的路径
	CString m_strConfigName; // xml配置文件的名称

	CString m_strTargetFile; // 包含了全路径的目标文件
	CString m_strTargetPath; // 目标文件的路径
	CString m_strTargetName; // 目标文件的名称
	
	UNIT* m_pRejectFolder;
	UNIT* m_pRejectFolderType;
	UNIT* m_pAcceptFileType;
	UNIT* m_pRejectFileType;

	UNIT* m_pAcceptFile;
	
	ACCEPT_PATH* m_pAcceptPath;

public:
	ConfigFile(TCHAR *pFullFile);
	~ConfigFile();

	void add2List(UNIT* pUnit, CString str, CString strPre = "");
	void add2List(UNIT* pUnit, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName);
	void add2List(ACCEPT_PATH* pPath, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName);
	void add2AcceptPathList(int iLen, TCHAR** pPathList);
	void add2RejectFolderList(CString strTokens);
	void add2RejectFolderTypeList(CString strTokens);
	
	bool isOneRejectFolder(const CString strFolder);
	bool isOneOrderFolder(ACCEPT_PATH* pAcceptPath, const CString strCurRootPath, const CString strFolder);

	bool checkFile(ACCEPT_PATH* pAcceptPath, const CString strPath, CString strFileName);
	bool checkPath(ACCEPT_PATH* pAcceptPath, const CString strPath);

	bool checkFolder(UNIT* ptr, const CString strFolder);

	/**
	 * 检查文件名为 strFileName 的文件，是否属于 ptr 所指向的一系列文件类型中的某一个
	 * 如果是，则返回 true；否则，返回 false.
	 * ptr 为字符串指针，指向一系列文件类型，形如 ".m" -> ".cpp" -> ".xml" -> ".*"，
	 * 其中 ".*" 表示任意文件类型
	 * 注意，每个文件类型都是以 "." 开头的。
	 */
	bool checkFileType(const CString strFileName, UNIT* ptr);

	/**
	 * 重载 checkFileType, 处理单个文件类型的情况
	 */
	bool checkFileType(CString strFileName, CString strFileType);
};
