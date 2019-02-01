#ifndef DEF_H
#define DEF_H

//
//  Code Page Default Values.
//
#define CP_ACP                    0           // default to ANSI code page
#define CP_OEMCP                  1           // default to OEM  code page
#define CP_MACCP                  2           // default to MAC  code page
#define CP_THREAD_ACP             3           // current thread's ANSI code page
#define CP_SYMBOL                 42          // SYMBOL translations

#define CP_UTF7                   65000       // UTF-7 translation
#define CP_UTF8                   65001       // UTF-8 translation

//
//  CS2 Engine Struct.
//
struct CS2Head
{
	char Magic[8];
	int SZcompress;
	int SZuncompress;
};

struct CS2InfoHead
{
	int totalsize;
	int UnknownInt32;
	int index1Size;
	int opcodeoffset;
};
#endif // DEF_H
