#include <iostream>
#include <fstream>
#include <string>

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

int main(int argc, char **argv) {
	try {
		if (argc!=2) // Permit to input only two parameter: path & codepage(Shift-JIS, GB18030-0, UTF-8)!
			 throw "arg kazu error";
		string path = argv[0], cp = argv[1];

		int codepage = CP_ACP;
		if (cp=="Shift-JIS")
			codepage = 932;
		else if (cp == "GB18030-0")
			codepage = 936;
		else if (cp == "UTF-8")
			codepage = CP_UTF8;
		else if (cp=="UTF-16")
			codepage = -1;

		fstream cstfile(path,ios::in|ios::binary);
		if (!cstfile)  // File path error!
			throw "path error";

		cstfile.seekg(0, cstfile.end);
		size_t filesize = cstfile.tellg();
		cstfile.seekg(0, 0);

		auto buffer=new char[filesize];
		cstfile.read(buffer,filesize);
		cstfile.close();

		CS2Head *head;
		head=(CS2Head*)buffer;
		if (memcmp(head->Magic,cs2magic,8)!=0) // File content error!
			throw "content error";
		if(filesize!=0x10+head->SZcompress) // File broken!
			throw "file broken";

		//////////////////////////////////////////////////////////////////////////

		auto uncompressbuffer=new char[head->SZuncompress+100];
		memset(uncompressbuffer, 0, head->SZuncompress + 100);

		uLongf uncompsize= head->SZuncompress + 100;
		int zstatus = uncompress((unsigned char*)uncompressbuffer, &uncompsize, (const Bytef*)buffer + 0x10, head->SZcompress);

		if (Z_OK!= zstatus || uncompsize!=head->SZuncompress)
			throw "Zlib tennkai error";

		CS2InfoHead* info = (CS2InfoHead*)uncompressbuffer;
		if (uncompsize- sizeof(CS2InfoHead) !=info->totalsize)
			throw "file broken";


	}
	catch (string e) {
		
	}
	catch (...) {

	}

	
}