#ifndef STRING_TYPE_H
#define STRING_TYPE_H
#include <string>

#if !defined(_UNICODE)  // not _UNICODE
typedef char CharType;
typedef std::string StringType;
#else  // _UNICODE
typedef wchar_t CharType;
typedef std::wstring StringType;
#endif  // _UNICODE

#endif  // STRING_TYPE_H