
/**
 * Author:  Mordecai Lee
 * Created: 2009/08/13, Thu., 17:54:31
 * Description: 
 */

#pragma once

#include "ConfigFile.h"
#include <fstream>
using namespace std;

class MatlabPathSync
{
private:
	ofstream m_MFile;
	ConfigFile* m_pConfig;

	ACCEPT_PATH* m_pCurAcceptPath;

	CString m_strCurRootPath;

	WIN32_FIND_DATA m_stFindFileData;

private:
	void writeOnePath(const CString strPath);

public:
	MatlabPathSync(ConfigFile* pConfigFile);
	~MatlabPathSync(void);

	void run();
	void addAllSubPaths(CString strCurRootPath, bool bCheckOrder = true);
};
