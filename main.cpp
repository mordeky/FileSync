
#include "ConfigFile.h"
#include "VCXProjSync.h"
#include "MatlabPathSync.h"

#include <iostream>
using namespace std;

/**
 * �÷���
 *   FileSync ConfigFile.xml [-ap PATH1 PATH2 ...]
 * ���磺
 *   E:\Academic\Labs\Matlab\Setting\path\PriorPath.xml -ap E:\Academic\Labs\Matlab\Work\Common\Develop E:\Academic\Labs\Matlab\Setting\path
 */

void main(int argc, TCHAR** argv)
{
	//CXMLParser xml(argv[1]);
	
	try
	{
		ConfigFile cf(argv[1]);
		if(argc > 2)
		{
			// Mordecai Lee, 2010/09/25, Sat., 14:56:44��������ѡ�[-ap PATH1 PATH2 ...]
			if(	CString(argv[2]).CompareNoCase("-ap") == 0 ) // ap -- accept path
				cf.add2AcceptPathList(argc-3, argv+3);
			else
			{
				cout << "Error parameters" << endl;
				return;
			}
		}

		if(cf.checkFileType(cf.m_strTargetName, ".m"))
		{
			MatlabPathSync matSync(&cf);
			matSync.run();
		}
		else if(cf.checkFileType(cf.m_strTargetName, ".vcproj") || cf.checkFileType(cf.m_strTargetName, ".vcxproj"))
		{
			VCXProjSync vcxSync(&cf);
			vcxSync.run();
		}
		else
			cout << "Error target file" << endl;
	}
	catch( TCHAR * str ) {
		cout << "Exception raised: " << str << '\n';
	}
}

/*
//���ַ����������������: http://www.9php.com/FAQ/cxsjl/c/2007/08/120521192232.html
void cout_str()
{
char *str = "ASCII string �����й��� ASCII string";
	wchar_t *wStr = L"Unicode string �����й��� Unicode string";
	TCHAR *tStr = _T("TCHAR string �����й��� TCHAR string");

	cout<<*str<<endl;
	cout<<str<<endl;

	wcout<<*wStr<<endl;
	wcout<<wStr<<endl;

#ifndef _UNICODE
	cout<<*tStr<<endl;
	cout<<tStr<<endl;
#else
	wcout<<*tStr<<endl;
	wcout<<tStr<<endl;
#endif
}
*/
