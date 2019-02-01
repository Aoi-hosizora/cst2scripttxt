#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>

// #include <WinNls.h>
// #include <stdio.h>

#include "typedef.h"

#if _MSC_VER == 1600 //10.0
#ifdef WINDOWS32
#define ZLIP_WINAPI
#endif
#endif
#include "zlib.h"

using namespace std;

char cs2magic[]="CatScene";

wchar_t* c2w(const char *str,int cp)
{
	if (cp==-1)
	{
		return (wchar_t*)*str;
	}
	wchar_t* buffer;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(cp, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n+1];
		memset(buffer, 0, sizeof(wchar_t)*(n + 1));
		MultiByteToWideChar(cp, 0, (const char *)str, int(nu), buffer, int(n));
	}
	return buffer;
}

char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize+1];

	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	return pCStrKey;
}

vector<string> *CSTOpen(string path,string cp) {
	
	int codepage = CP_ACP;
	transform(cp.begin(), cp.end(), cp.begin(), ::toupper);
	if (cp=="SHIFT-JIS" || cp=="SHIFT_JIS")
		codepage = 932;
	else if (cp=="GB18030-0")
		codepage = 936;
	else if (cp=="UTF-8")
		codepage = CP_UTF8;
	else if (cp=="UTF-16")
		codepage = -1;
	else
		throw "Codepage is just suit for Shift-JIS & GB18030-0 & UTF-8 & UTF-16.";

	vector<string> *result = new vector<string>();
	fstream cstfile(path,ios::in|ios::binary);
	if (!cstfile)
		throw "File path error!";

	cstfile.seekg(0, cstfile.end);
	size_t filesize = cstfile.tellg();
	cstfile.seekg(0, 0);

	auto buffer=new char[filesize];
	cstfile.read(buffer,filesize);
	cstfile.close();

	CS2Head *head;
	head=(CS2Head*)buffer;
	if (memcmp(head->Magic,cs2magic,8)!=0)
		throw "File content error!";
	if(filesize!=0x10+head->SZcompress)
		throw "File broken Error!";

	//////////////////////////////////////////////////////////////////////////

	auto uncompressbuffer=new char[head->SZuncompress+100];
	memset(uncompressbuffer, 0, head->SZuncompress + 100);

	uLongf uncompsize= head->SZuncompress + 100;
	int zstatus = uncompress((unsigned char*)uncompressbuffer, &uncompsize, (const Bytef*)buffer + 0x10, head->SZcompress);

	if (Z_OK!= zstatus || uncompsize!=head->SZuncompress)
		throw "File Zip Error";

	CS2InfoHead* info = (CS2InfoHead*)uncompressbuffer;
	if (uncompsize- sizeof(CS2InfoHead) !=info->totalsize)
		throw "File broken Error!";

	char *opcodeptr;
	opcodeptr = uncompressbuffer + sizeof(CS2InfoHead) + info->opcodeoffset;
	int entrys = (info->opcodeoffset - info->index1Size) / 4;
	int *index = (int*)(uncompressbuffer + sizeof(CS2InfoHead) + info->index1Size);

	for (int i=0;i<entrys;i++) {
		int offset=index[i];
		if (opcodeptr[offset]==0x1 && opcodeptr[offset+1]==0x20) {
			if (opcodeptr[offset+2]!=0x0) {
				wchar_t* tmpwc = c2w((const char*)&opcodeptr[offset + 2],codepage);
				string TmpString = wideCharToMultiByte(tmpwc);
				delete tmpwc;
				tmpwc = nullptr;
				result->push_back(TmpString);
			}

		}
		if (opcodeptr[offset] == 0x1 && opcodeptr[offset + 1] == 0x21) {
			if (opcodeptr[offset+2]!=0x0)
			{
				wchar_t* tmpwc = c2w((const char*)&opcodeptr[offset + 2],codepage);
				string TmpString = wideCharToMultiByte(tmpwc);
				delete tmpwc;
				tmpwc = nullptr;
				result->push_back(TmpString);
			}
		}

		if (opcodeptr[offset] == 0x1 && opcodeptr[offset + 1] == 0x30)
		{
			if (opcodeptr[offset+2]==0x30||opcodeptr[offset+2]==0x31||opcodeptr[offset+2]==0x32||opcodeptr[offset+2]==0x33||opcodeptr[offset+2]==0x34||opcodeptr[offset+2]==0x35||opcodeptr[offset+2]==0x36||opcodeptr[offset+2]==0x37||opcodeptr[offset+2]==0x38||opcodeptr[offset+2]==0x39)
			{
				wchar_t* tmpwc = c2w((const char*)&opcodeptr[offset + 2],codepage);
				string TmpString = wideCharToMultiByte(tmpwc);
				delete tmpwc;
				tmpwc = nullptr;
				result->push_back(TmpString);
			}
		}
	}
	delete buffer;
	delete uncompressbuffer;
	return result;
}

int main(int argc, char **argv) {
	try {
		if (argc!=3)
			throw "Permit to input only two parameter: path & codepage(Shift-JIS, GB18030-0, UTF-8)!";
		string path = argv[1], cp = argv[2];

		vector<string> *res;
		res = CSTOpen(path,cp);
		if (res == nullptr)
			throw "error";

		ofstream savefile(path.substr(0,path.size()-4)+".txt");
		if (!savefile)
			throw "Open file Error!";
		for (auto iter = res->begin();iter!=res->end();iter++) {
			cout<<*iter<<endl;
			savefile<<*iter<<endl;
		}
		savefile.close();
		delete res;
	}
	catch (char e[]) {
		cout<<"Error: "<<e<<endl;
		return 1;
	}
	catch (...) {
		cout<<"Something Error!"<<endl;
		return 1;
	}

	
}