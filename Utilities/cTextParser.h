#ifndef CTEXTPARSER_H
#define CTEXTPARSER_H
#include <fstream>
using namespace std;

typedef bool (*BOOLFUNC)(const char ch);
#define TEXT_BUFFER 255

inline bool IsWhiteSpace(const char ch){ return (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');}
inline bool IsNumeric(const char ch){ return ( (ch >= '0' && ch <= '9' ) || ch == '-'); }
inline bool IsAlphabetical(const char ch){ return ( (ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z' )  || ch == '_'); }
inline bool IsComment(const char ch) { return ((ch == '#') || (ch =='/'));}
inline float TextToNum(char* pText) {
	float test=0, num = 10;
	bool sign;
	size_t textsize(strlen(pText)),  i(0);
	sign = false;
	while((sign == false) && (i < textsize)){
		switch(pText[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':
			case '.': sign = true; break;
			default: i++; break;
		}
	}
	if (i>= textsize) return 0.0f;
	if(pText[i]=='-'){
		sign = true; 
		i++;
	} else sign = false;
	while ((pText[i]>='0') && (pText[i]<='9')) {
		test*=num;
		test+=(pText[i++]-'0');
	}
	num = 0.1f;
	if ((i<textsize) && (pText[i++]=='.')){
		while ((pText[i]>='0') && (pText[i]<='9')){
			test += (pText[i++]-'0')*num;
			num *= 0.1f;
		}
	}
	if (sign) test = -test;
	return test;
}
inline void GotoNext(ifstream& fin, BOOLFUNC func = IsAlphabetical){// this should be used to goto numbers, or alphabetical stuff. Do not use this to try and goto a open brace or something
	char Token;
	while(!fin.eof()){// keep gpoing, while we are eating through junk
		Token=fin.peek();
		if(func(Token)) return;	
		else if(IsComment(Token)) fin.ignore(TEXT_BUFFER, '\n');// this is the begining of another comment. ignore the whole line
		else fin.get();// eat the junk
	}
}
inline float ReadFloat(ifstream& fin, char c){
	char data[255];
	GotoNext(fin, IsNumeric);// goto next number
	fin.getline(data, TEXT_BUFFER, c);
	return TextToNum(data);
}
inline void GotoNext(ifstream& fin, char v, BOOLFUNC func = IsAlphabetical){// this should be used to goto numbers, or alphabetical stuff. Do not use this to try and goto a open brace or something
	char Token;
	while(!fin.eof()){// keep gpoing, while we are eating through junk
		Token=fin.peek();
		if(func(Token) || Token == v) return;	
		else if(IsComment(Token)) fin.ignore(TEXT_BUFFER, '\n');// this is the begining of another comment. ignore the whole line
		else fin.get();// eat the junk
	}
}
inline void GotoNext(ifstream& fin, char v, bool eat){// this will remove all the junk so that the next fin.get() will be something that is usefull!!
	char Token;
	while(!fin.eof()){// keep gpoing, while we are eating through junk
		Token=fin.peek();
		if(v == Token) break;
		else if(IsComment(Token)) fin.ignore(TEXT_BUFFER, '\n');// this is the begining of another comment. ignore the whole line
		else fin.get();// eat the junk
	}
	if(eat) fin.get();// eat the char
}

#endif