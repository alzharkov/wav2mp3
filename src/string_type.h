#ifndef STRING_TYPE_H
#define STRING_TYPE_H
#include <string>

#if !defined(_UNICODE)  // not _UNICODE
typedef char CharType;
typedef std::string StringType;
#define _T(X) X
#else  // _UNICODE
typedef wchar_t CharType;
typedef std::wstring StringType;
#define _T(X) L ## X
#endif  // _UNICODE

#endif  // STRING_TYPE_H