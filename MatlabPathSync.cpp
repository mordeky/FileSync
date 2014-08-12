#include "MatlabPathSync.h"
#include "Common.h"
#include <iostream>
using namespace std;

#define REJECT_FOLDER "Private"
#define REJECT_FOLDERTYPE_LIST    "., +, @, %"

MatlabPathSync::MatlabPathSync(ConfigFile* pConfigFile) : m_MFile(pConfigFile->m_strTargetFile)
{
	m_pConfig = pConfigFile;
	m_pConfig->add2RejectFolderList(REJECT_FOLDER);
	m_pConfig->add2RejectFolderTypeList(REJECT_FOLDERTYPE_LIST);

	CString strMFileName = pConfigFile->m_strTargetName;
	strMFileName = strMFileName.Left(strMFileName.GetLength() - 2); // 2 Ϊ ".m" �ĳ���, strMFileName Ϊȥ����չ������ļ���
	m_MFile << "function mypath = " << strMFileName << "\n"
		    << "mypath = [...\n";
}

MatlabPathSync::~MatlabPathSync(void)
{
	m_MFile << "];\n";
		    //<< "path(mypath, pathdef);\n";

	cout << "Synchronise " << m_pConfig->m_strTargetName << " successfully!" << endl;
}

void MatlabPathSync::writeOnePath(const CString strPath)
{
	m_MFile << "'" << strPath << ";'...\n";
}

void MatlabPathSync::run()
{
	cout << "\nStart synchronising..." << endl;

	UNIT* ptr;
	m_pCurAcceptPath = m_pConfig->m_pAcceptPath;

	while(m_pCurAcceptPath && m_pCurAcceptPath->strPath != "")
	{
		m_strCurRootPath = m_pCurAcceptPath->strPath;
		addPathSep(m_strCurRootPath);

		// �����ǰ·�� m_strCurRootPath �£�����Ҫ��˳��������ļ��У��������
		// �磺<AcceptPath value="E:\Academic\Labs\Matlab\Work\" order="Common, Imgs, Reading">
		ptr = m_pCurAcceptPath->pOrder;
		while(ptr && ptr->strData != "")
		{
			CString strCurPath = m_strCurRootPath + ptr->strData;
			addAllSubPaths(strCurPath, false);
			ptr = ptr->pNext;
		}

		addAllSubPaths(m_strCurRootPath);

		m_pCurAcceptPath = m_pCurAcceptPath->pNext;
	}
}

// �ݹ���� strCurRootPath �����е����ļ��У���д�� M-File
void MatlabPathSync::addAllSubPaths(CString strCurRootPath, bool bCheckOrder)
{
	addPathSep(strCurRootPath); // ȷ�� strCurRootPath �� '\' ��β
	writeOnePath(strCurRootPath);

	CString strPath;
	
	strPath = strCurRootPath + _T("*.*");

	HANDLE hFind = FindFirstFile(strPath, &m_stFindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return;

	while(TRUE)
	{
		// ����ļ��е����֣���
		if(m_stFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // ȷ�����ҵ����ļ�Ϊ�ļ���
		{
			/**
			  ����ļ��е����� m_stFindFileData.cFileName Ϊ�������֮һ�����ų�
			    (1) �ļ��е������� '.' ��ͷ��
				    > ���պ��� FindFirstFile, FindNextFile ���������ҵ�����Ϊ '.' �� '..' ���ļ���
					> һ���ǱȽ�������ļ��е������� '.svn' �ļ���
					> �û�һ���޷��½��� '.' ��ͷ���ļ���
				(2) �ļ����ھܾ��б���
				(3) �ļ����������б��У���Ϊ�Ѿ��� run() ����ӹ���
			*/

			if(m_pConfig->isOneRejectFolder(m_stFindFileData.cFileName))
				goto FIND_NEXT;

			if(bCheckOrder && m_pConfig->isOneOrderFolder(m_pCurAcceptPath, strCurRootPath, m_stFindFileData.cFileName))
				goto FIND_NEXT;

			strPath = strCurRootPath + m_stFindFileData.cFileName;
			strPath += "\\";

			if(m_pConfig->checkPath(m_pCurAcceptPath, strPath))
				addAllSubPaths(strPath, bCheckOrder);
		}

FIND_NEXT:
		if(!FindNextFile(hFind, &m_stFindFileData))
			break;
	}
	
	FindClose(hFind);
}