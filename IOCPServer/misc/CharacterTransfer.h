#ifndef _CHARACTER_TRANSFER_H_
#define _CHARACTER_TRANSFER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <windows.h>
#include <vector>
#include <string>

using namespace std; 

//utf8 ת Unicode
std::wstring Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);

	if(ERROR_NO_UNICODE_TRANSLATION == widesize)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	
	if(!widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	
	std::vector<wchar_t> resultstring(widesize);
	
	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
	
	if(convresult != widesize)
	{
		throw std::exception("La falla!");
	}
	
	return std::wstring(&resultstring[0]);
}

//unicode תΪ ascii
string WideByte2Acsi(wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);

	if(ERROR_NO_UNICODE_TRANSLATION == asciisize)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	
	if(!asciisize)
	{
		throw std::exception("Error in conversion.");
	}
	
	std::vector<char> resultstring(asciisize);
	
	int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);
	
	if(convresult != asciisize)
	{
		throw std::exception("La falla!");
	}
	
	return std::string(&resultstring[0]);
}

 

//utf-8 ת ascii
string UTF_82ASCII(string& strUtf8Code)
{
	string strRet("");
	
	//�Ȱ� utf8 תΪ unicode
	wstring wstr = Utf82Unicode(strUtf8Code);
	
	//���� unicode תΪ ascii
	strRet = WideByte2Acsi(wstr);
	
	return strRet;
}

///////////////////////////////////////////////////////////////////////
//ascii ת Unicode
wstring Acsi2WideByte(string& strascii)
{
	int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
	
	if(ERROR_NO_UNICODE_TRANSLATION == widesize)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	
	if(!widesize)
	{
		throw std::exception("Error in conversion.");
	}

    std::vector<wchar_t> resultstring(widesize);
	
	int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);
	
	if(convresult != widesize)
	{
		throw std::exception("La falla!");
	}
	
	return std::wstring(&resultstring[0]);
}

//Unicode ת Utf8
std::string Unicode2Utf8(const std::wstring& widestring)
{
	int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
	
	if(!utf8size)
	{
		throw std::exception("Error in conversion.");
	}
	
	std::vector<char> resultstring(utf8size);
	
	int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);
	
	if(convresult != utf8size)
	{
		throw std::exception("La falla!");
	}
	
	return std::string(&resultstring[0]);
}

//ascii ת Utf8
string ASCII2UTF_8(string& strAsciiCode)
{
	string strRet("");
	
	//�Ȱ� ascii תΪ unicode
	wstring wstr = Acsi2WideByte(strAsciiCode);
	
	//���� unicode תΪ utf8
	strRet = Unicode2Utf8(wstr);
	
	return strRet;
}

#endif//_CHARACTER_TRANSFER_H_
