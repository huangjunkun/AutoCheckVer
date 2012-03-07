#ifndef __DEFINE_MAIN_H
#define __DEFINE_MAIN_H

#pragma  once

//
// #include <download_interface/download_interface.h>
// #include <download_interface/p2sp_interface.h>
// #include <download_interface/bt_interface.h>
// #include <download_interface/emule_interface.h>

#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <stdexcept>

//using namespace std;
namespace junkun
{

#ifdef _UNICODE

#define strlen_t	wcslen
#define fgets_t		fgetws
#define fputs_t		fputws
#define fscanf_t	fwscanf
#define sscanf_t	swscanf
#define printf_t	wprintf
#define fprintf_t	fwprintf
#define sprintf_t	wsprintf
#define tcout		std::wcout
#define tcerr		std::wcerr
#define tcin		std::wcin
#define string_t	std::wstring
#define char_t		wchar_t
#define ostringstream_t	wostringstream

#else

#define strlen_t	strlen
#define fgets_t		fgets
#define fputs_t		fputs
#define printf_t	printf
#define fscanf_t	fscanf
#define sscanf_t	sscanf
#define fprintf_t	fprintf
#define sprintf_t	sprintf
#define tcout		std::cout
#define tcerr		std::cerr
#define tcin		std::cin
#define string_t	std::string
#define char_t		char
#define ostringstream_t	ostringstream

#endif

#ifndef isdigit_t
#define isdigit_t(ch) (ch >= _T('0') && ch <= _T('9'))
#endif
#ifndef isalpha_t
#define isalpha_t(ch) (((ch) >= _T('a') && (ch) <= _T('z')) || ((ch) >= _T('A') && (ch) <= _T('Z')))
#endif
#ifndef islower_t
#define islower_t(ch) ((ch) >= _T('a') && (ch) <= _T('z'))
#endif
#ifndef isupper_t
#define isupper_t(ch) ((ch) >= _T('A') && (ch) <= _T('Z'))
#endif

const unsigned CHAR_LOWER_UPPER_CHANGE_FACTOR = _T('z') - _T('a');
#ifndef tolower_t
#define tolower_t(ch) (isupper_t(ch) ? ((ch) + CHAR_LOWER_UPPER_CHANGE_FACTOR) : (ch))
//#define tolower_t(ch) isalpha_t(ch) ? (islower_t(ch) ? (ch) : (ch + CHAR_LOWER_UPPER_CHANGE_FACTOR)) : (ch)
#endif
#ifndef toupper_t
#define toupper_t(ch) (islower_t(ch) ? ((ch) - CHAR_LOWER_UPPER_CHANGE_FACTOR) : (ch))
//#define toupper_t(ch) isalpha_t(ch) ? (isupper_t(ch) ? (ch) : (ch + CHAR_LOWER_UPPER_CHANGE_FACTOR)) : (ch)
#endif
//
//////////////////////////////////////////////////////////////////////////
//#define LOG
#ifdef _DEBUG //LOG//

#define cout_trace(msg) do { tcout << __FUNCTION__ << " __cout4log trace:" << (msg) << "\n"; } while(0)
#define cout_debug(msg) do { tcout << __FUNCTION__ << " __cout4log debug:" << (msg) << "\n"; } while(0)
#define cout_info(msg)  do { tcout << __FUNCTION__ << " __cout4log info:" << (msg) << "\n"; } while(0)
#define cerr_info(msg)  do { tcerr << __FUNCTION__ << " __cerr4log info:" << (msg) << "\n"; } while(0)
#define assert_false_notify(msg) do { const bool msg(false); assert((msg)); } while(0)


#else

#define cout_trace(msg) 0
#define cout_debug(msg) 0
#define cout_info(msg)  0
#define cerr_info(msg)  0
#define assert_false_notify(msg) 0

#endif // end _DEBUG

}// namespace junkun

namespace junkun
{

class c_single_file
{
public:
    //		using namespace std;
public:
    c_single_file() : _file(0) {}
    c_single_file(const std::string& filename, const std::string& open_type="wb+")
        : _file(0)
    {
        if ( !open(filename, open_type))
        {
            assert_false_notify (c_single_file_open_return_false);
            throw std::logic_error("c_single_file_open_return_false");
        }
        //return;//error
    }
    c_single_file(const std::wstring& filename, const std::wstring& open_type=L"wb+")
        : _file(0)
    {
        if ( !open(filename, open_type))
        {
            assert_false_notify (c_single_file_open_return_false);
            throw std::logic_error("c_single_file_open_return_false");
        }
        //return;//error
    }

    ~c_single_file()
    {
        close();
    }
    // member function
    bool open(const std::string& filename, const std::string& open_type="rb+")
    {
        _file = fopen(filename.c_str(), open_type.c_str());
        return (NULL != _file) ;
    }

    bool open(const std::wstring& filename, const std::wstring& open_type=L"rb+")
    {
        _file = _wfopen(filename.c_str(), open_type.c_str());
        return (NULL != _file) ;
    }

    size_t write(void* pData,size_t len)
    {
        assert(_file);
        return fwrite(pData, 1, len, _file);
    }

    size_t read(void* pData,size_t len)
    {
        assert(_file);
        return fread(pData, 1, len, _file);
    }

    bool close()
    {
        if (_file)
        {
            int res = fclose(_file);
            _file = NULL;
            return (0==res);
        }
        return false;
    }
    int flush()
    {
        assert (_file);
        return fflush(_file);
    }
    char* gets(char* c_str, unsigned len)
    {
        assert (_file);
        return fgets(c_str, len, _file);
    }
    wchar_t* gets(wchar_t* c_str, unsigned len)
    {
        assert (_file);
        return fgetws(c_str, len, _file);
    }
    int puts(char* c_str)
    {
        assert (_file);
        return fputs(c_str, _file);
    }
    int puts(wchar_t* c_str)
    {
        assert (_file);
        return fputws(c_str, _file);
    }

    int eof()
    {
        assert (_file);
        return feof(_file);
    }
    int error()
    {
        assert (_file);
        return ferror(_file);
    }
    bool is_opened() const
    {
        return NULL != _file;
    }
    FILE* file_ptr()
    {
        return _file;
    }

private:
    FILE* _file;
};//class c_single_file


}// namespace junkun

#endif
