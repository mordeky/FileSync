#pragma once

#include <tchar.h>
#include <atlstr.h>
#include "XMLParser.h"
#include "Common.h"

struct UNIT
{
	CString strData;
	UNIT *pNext;

	UNIT() : strData(""), pNext(NULL)
	{}

	~UNIT()
	{}
};

struct ACCEPT_PATH : public UNIT
{
	UNIT* pSubfolderOrder;
	UNIT* pRejectFile;
	UNIT* pRejectPath;
	ACCEPT_PATH *pNext;

	ACCEPT_PATH() : pNext(NULL)
	{
		pSubfolderOrder = new UNIT;
		pRejectFile = new UNIT;
		pRejectPath = new UNIT;
	}

	~ACCEPT_PATH()
	{
		deleteLink(pSubfolderOrder);
		deleteLink(pRejectFile);
		deleteLink(pRejectPath);
	}
};

class ConfigFile : public CXMLParser
{
public:
	CString m_strConfigFile; // ������ȫ·����xml�����ļ�
	CString m_strConfigPath; // xml�����ļ���·��
	CString m_strConfigName; // xml�����ļ�������

	CString m_strTargetFile; // ������ȫ·����Ŀ���ļ�
	CString m_strTargetPath; // Ŀ���ļ���·��
	CString m_strTargetName; // Ŀ���ļ�������
	
	UNIT* m_pRejectFolder;
	UNIT* m_pRejectFolderType;
	UNIT* m_pAcceptFileType;
	UNIT* m_pRejectFileType;

	UNIT* m_pAcceptFile;
	
	ACCEPT_PATH* m_pAcceptPath;

public:
	ConfigFile(TCHAR *pFullFile);
	~ConfigFile();

	UNIT* add2SettingList(CString strNodeName, CString strPre = "");

	UNIT* add2List(UNIT* pUnit, CString str, CString strPre = "");

	template<typename T>
	T* addInnerPaths2PathList(MSXML2::IXMLDOMElementPtr pElement, T* pUnit);
	template<typename T>
	void addInnerPaths2PathList(T* pUnit, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName);

	void add2AcceptPathList(ACCEPT_PATH* pPath, MSXML2::IXMLDOMElementPtr pParentElement, CString strNodeName);

	void add2AcceptPathList(int iLen, TCHAR** pPathList);
	ACCEPT_PATH* addRootPath2AcceptPathList(MSXML2::IXMLDOMElementPtr pElement, ACCEPT_PATH* pPath);

	void add2RejectFolderList(CString strTokens);
	void add2RejectFolderTypeList(CString strTokens);
	
	bool isOneRejectFolder(const CString strFolder);
	bool isOneOrderFolder(ACCEPT_PATH* pAcceptPath, const CString strCurRootPath, const CString strFolder);

	bool checkFile(ACCEPT_PATH* pAcceptPath, const CString strPath, CString strFileName);
	bool checkPath(ACCEPT_PATH* pAcceptPath, const CString strPath);

	bool isFolderBeginWith(UNIT* ptr, const CString strFolder);
	bool isFolderEqualTo(UNIT* ptr, const CString strFolder);

	/**
	 * ����ļ���Ϊ strFileName ���ļ����Ƿ����� ptr ��ָ���һϵ���ļ������е�ĳһ��
	 * ����ǣ��򷵻� true�����򣬷��� false.
	 * ptr Ϊ�ַ���ָ�룬ָ��һϵ���ļ����ͣ����� ".m" -> ".cpp" -> ".xml" -> ".*"��
	 * ���� ".*" ��ʾ�����ļ�����
	 * ע�⣬ÿ���ļ����Ͷ����� "." ��ͷ�ġ�
	 */
	bool checkFileType(const CString strFileName, UNIT* ptr);

	/**
	 * ���� checkFileType, �������ļ����͵����
	 */
	bool checkFileType(CString strFileName, CString strFileType);
};
