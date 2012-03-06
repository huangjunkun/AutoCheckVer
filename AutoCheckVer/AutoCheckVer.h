/* -------------------------------------------------------------------------
//	文件名		：	autocheckver.h
//	创建者		：	黄俊坤
//	创建时间	：	2011-06-15 10:44:11
//	功能描述	：	下载库自动化检测提包规范与dll exe版本号匹配情况。
//
// -----------------------------------------------------------------------*/
#ifndef AUTO_CHECK_VEC_H__
#define AUTO_CHECK_VEC_H__

#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <ctype.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cassert>
#include <limits.h>
//#include <char_traits.h>
#include <string>
#include <Windows.h>

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Mpr.lib")
#include "main.h"


namespace junkun
{
using namespace std;

template<typename _TChar>
inline size_t strlenT(const _TChar *string );

template<>
inline size_t strlenT<char>(const char *string )
{
    return strlen( string );
}

template<>
inline size_t strlenT<wchar_t>(const wchar_t *string )
{
    return wcslen( string );
}

template<typename _TChar>
inline _TChar* strcpyT(_TChar* des, const _TChar* src );

template<>
inline char* strcpyT<char>(char* des, const char* src )
{
    return strcpy( des,  src);
}

template<>
inline wchar_t* strcpyT<wchar_t>(wchar_t *des, const wchar_t* src )
{
    return wcscpy(des,  src);
}

//template <typename _TChar >
//inline int tsprintf( _TChar *buffer, const _TChar *format, ... );

//template<>
//inline int tsprintf<char>( char *buffer, const char *format, ... )
//{
//	return sprintf(buffer, format, ...);
//}
//template<>
//inline int tsprintf<wchar_t>( wchar_t *buffer, const wchar_t *format, ... )
//{
//	return wsprintf(buffer, format, ...);
//}


template <typename _TChar >
bool check_folder_exist(const basic_string<_TChar>& strPath)
{
    typedef basic_string<_TChar> TString;
    WIN32_FIND_DATA  wfd;
    bool rValue = false;
    const static TString ALL_FILE_INCLUDED(_T("*.*"));
    HANDLE hFind = ::FindFirstFile(TString(strPath+ALL_FILE_INCLUDED).c_str(), &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = true;
    }
    FindClose(hFind);
    return rValue;
}

template <typename _TChar >
basic_string<_TChar> get_product_ver(const basic_string<_TChar>& wsFileName)
{
    typedef basic_string<_TChar> _TString;
    _TChar FileVerInfoBuffer[2048];
    ::GetFileVersionInfo(wsFileName.c_str(), 0, sizeof(FileVerInfoBuffer), FileVerInfoBuffer);

    VS_FIXEDFILEINFO *FileInfo;
    UINT nBytes=0;
    BOOL res = ::VerQueryValue(FileVerInfoBuffer, _T("\\"), (void**)&FileInfo, &nBytes);
    if (res)
    {
        _TChar VerStr[256];
        //memset(VerStr, 0, NULL);
        // warning: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.
        // To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
        // #define _CRT_SECURE_NO_DEPRECATE /// in stdafx.h
#ifdef _UNICODE
        unsigned VerStrSize = wsprintf(VerStr, _T("%d.%d.%d.%d"),
                                       FileInfo->dwFileVersionMS >> 16,
                                       FileInfo->dwFileVersionMS & 0xffff,
                                       FileInfo->dwFileVersionLS >> 16,
                                       FileInfo->dwFileVersionLS & 0xffff);

#else
        unsigned VerStrSize = sprintf(VerStr, "%d.%d.%d.%d",
                                      FileInfo->dwFileVersionMS >> 16,
                                      FileInfo->dwFileVersionMS & 0xffff,
                                      FileInfo->dwFileVersionLS >> 16,
                                      FileInfo->dwFileVersionLS & 0xffff);
#endif
        return string_t(VerStr, VerStr + VerStrSize);
    }
    return _T("");//none
}

template <typename _TChar>
bool get_ext_file_list(const basic_string<_TChar>& dir, const vector<basic_string<_TChar> >& ext_name_vec, \
                       vector<basic_string<_TChar> >& result)
{
    typedef basic_string<_TChar> _TString;

    WIN32_FIND_DATA	win32_find_data;
    const static basic_string<_TChar> ALL_FILE_INCLUDED(_T("*.*"));
    HANDLE hFind =	::FindFirstFile(_TString(dir+ALL_FILE_INCLUDED).c_str(), &win32_find_data);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        cout_trace(_T("INVALID_HANDLE_VALUE == hFind \n"));
        return false;
    }
    int	errorcode = 0;
    while (true)
    {
        ::FindNextFile(hFind, &win32_find_data);
        errorcode = GetLastError();
        if (errorcode == ERROR_NO_MORE_FILES)
            break;

        _TChar* file_name_end = win32_find_data.cFileName + strlenT(win32_find_data.cFileName);
        assert (file_name_end > win32_find_data.cFileName);
        _TChar* back_move = file_name_end-1;
        while (back_move > win32_find_data.cFileName && *back_move != _TChar('.'))
            --back_move;
        if (back_move > win32_find_data.cFileName)
        {
            const _TString ext(back_move+1, file_name_end);
            typename vector<_TString>::const_iterator find_pos = std::find(ext_name_vec.begin(), ext_name_vec.end(), ext);

            if (ext_name_vec.end() != find_pos)
                result.push_back(_TString(win32_find_data.cFileName));//wsDirectory+
        }
        //else assert (false);/// 后缀名为空情况
    }

    ::FindClose(hFind);
    return true;
}



#define SPACE_TCHAR		_T(' ')
#define TAB_TCHAR		_T('\t')
#define RETURN_TCHAR	_T('\n')

template <typename _TChar>
basic_string<_TChar>  trim_tstring(const basic_string<_TChar>& srcString)
{
    if (srcString.empty())
        return basic_string<_TChar>();

    typename basic_string<_TChar>::const_iterator citer = srcString.begin();
    for (; citer != srcString.end(); ++citer)
        if (!(*citer == _TChar(SPACE_TCHAR) || *citer == _TChar(TAB_TCHAR)))
            break;

    typename basic_string<_TChar>::const_reverse_iterator criter = srcString.rbegin();
    for (; criter != srcString.rend(); ++criter)
        if (!(*citer == _TChar(SPACE_TCHAR) || *citer == _TChar(TAB_TCHAR)))
            break;
    typename basic_string<_TChar>::const_iterator end_pos(criter.base());

    return basic_string<_TChar>(citer, end_pos);
}


// #ifdef _UNICODE
// 	const static wstring COMMIT_INFO_TAGS[S_TAGS_SIZE] = \
// 	{	_T("模块文件名："),
// 	_T("负责人："),
// 	_T("代码revision："),
// 	_T("代码提交时间："),
// 	_T("模块版本号："),
// 	_T("版本特征：\n") };
// #else
// 	const static string COMMIT_INFO_TAGS[S_TAGS_SIZE] = \
// 	{	_T("模块文件名："),
// 	_T("负责人："),
// 	_T("代码revision："),
// 	_T("代码提交时间："),
// 	_T("模块版本号："),
// 	_T("版本特征：") };
// #endif

static std::vector<string_t>	COMMIT_INFO_TAGS;
static unsigned S_TAGS_SIZE = 0;//
static string_t s_default_server_path;
//static tstring s_default_txt_file;

#define	MOUDLE_NAME_TAG		COMMIT_INFO_TAGS[0]
#define	DIRECTOR_NAME_TAG	COMMIT_INFO_TAGS[1]
#define	REVISION_NUM_TAG	COMMIT_INFO_TAGS[2]
#define	COMMIT_TIME_TAG		COMMIT_INFO_TAGS[3]
#define	VER_NUM_TAG			COMMIT_INFO_TAGS[4]
#define	VER_INFO_TAG		COMMIT_INFO_TAGS[5]
#define	ARRAY_TAGS_SIZE		S_TAGS_SIZE
#define	ARRAY_TAGS			COMMIT_INFO_TAGS



void load_config(const string_t& config_file = _T(".\\config\\autocheckver.conf"));//

template <typename _TChar>
struct commit_info_t
{
    typedef basic_string<_TChar> TString;

    //const static unsigned S_TAGS_SIZE;

    TString* moudle_name;
    TString* director_name;
    //unsigned __int64 revision_num;
    TString* revision_num;
    TString* commit_time;
    TString* ver_num;
    TString* release_date;
    TString* ver_info;

    commit_info_t()
    {
        _before_ctor();
    }

    commit_info_t(const commit_info_t& obj)
    {
        _before_ctor();
        for (unsigned i =0; i < tag_values.size(); ++i)
            tag_values[i] = obj.tag_values[i];
    }

    commit_info_t& operator = (const commit_info_t& obj)
    {
        _before_ctor();
        for (unsigned i =0; i < tag_values.size(); ++i)
            tag_values[i] = obj.tag_values[i];

        return *this;
    }

// 	basic_istream<_TChar, char_traits<_TChar> >& operator >> (basic_istream<_TChar, char_traits<_TChar> >& ins);
// 	basic_ostream<_TChar, char_traits<_TChar> >& operator << (basic_ostream<_TChar, char_traits<_TChar> >& outs);
//
   friend basic_istream<_TChar, char_traits<_TChar> >& operator >> <>(basic_istream<_TChar, char_traits<_TChar> >& ins,\
           commit_info_t<_TChar>& obj);
   friend basic_ostream<_TChar, char_traits<_TChar> >& operator << <>(basic_ostream<_TChar, char_traits<_TChar> >& outs,\
           const commit_info_t<_TChar>& obj);

//    template <typename char_t>
//    friend basic_istream<char_t, char_traits<char_t> >& operator >> (basic_istream<char_t, char_traits<char_t> >& ins,\
//            commit_info_t<char_t>& obj);
//    template <typename char_t>
//    friend basic_ostream<char_t, char_traits<char_t> >& operator << (basic_ostream<char_t, char_traits<char_t> >& outs,\
//            const commit_info_t<char_t>& obj);

private:
    vector<TString> tag_values;

    void _before_ctor()
    {
        assert (S_TAGS_SIZE);
        tag_values.resize(S_TAGS_SIZE);
        moudle_name = &tag_values[0];
        director_name = &tag_values[1];
        revision_num = &tag_values[2];
        commit_time = &tag_values[3];
        ver_num = &tag_values[4];
        release_date = &tag_values[5];
        ver_info = &tag_values[6];
    }
	friend basic_istream<char_t, char_traits<char_t> >&
		read(basic_istream<char_t, char_traits<char_t> >& ins, commit_info_t<char_t>& obj);
	friend basic_ostream<char_t, char_traits<char_t> >&
		write(basic_ostream<char_t, char_traits<char_t> >& outs, const commit_info_t<char_t>& obj);
}; //commit_info_t _TChar

struct compare_moudle_name_less
{
    template <typename _TChar>
    bool operator()(const commit_info_t<_TChar>& left, const commit_info_t<_TChar>& right) const
    {
        return left.moudle_name < right.moudle_name;
    }
};

struct compare_moudle_name_equal
{
    template <typename _TChar>
    bool operator()(const commit_info_t<_TChar>& left, const commit_info_t<_TChar>& right)
    {
        return left.moudle_name == right.moudle_name;
    }
};

struct ver_num_list
{
    const static unsigned VER_NUM_COUNT = 4;
    unsigned num1;
    unsigned num2;
    unsigned num3;
    unsigned num4;
    ver_num_list()
        : num1(0),  num2(0),  num3(0),  num4(0)
    {}
    ver_num_list(const string_t& str)
    {
        bool ver_num_list_construct_ok = reset(str);
        assert (ver_num_list_construct_ok);//todo: warn ...
    }

    bool reset(const string_t& str);
    string_t str() const;

    bool compare(const ver_num_list& obj) const
    {
        const unsigned* nums_ptr1[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
        const unsigned* nums_ptr2[VER_NUM_COUNT] = { &obj.num1, &obj.num2, &obj.num3, &obj.num4};
        for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
        {
            if (*nums_ptr1[i] != *nums_ptr2[i])
                return false;
        }
        return true;
    }
#if _HAS_TRADITIONAL_STL
    bit_vector compare_ex2(const ver_num_list& obj) const
    {
        bit_vector result(VER_NUM_COUNT);
        const unsigned* nums_ptr1[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
        const unsigned* nums_ptr2[VER_NUM_COUNT] = { &obj.num1, &obj.num2, &obj.num3, &obj.num4};
        for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
        {
            if (*nums_ptr1[i] != *nums_ptr2[i])
                result[i] = (false);
            else
                result[i] = (true);
        }
        return result;
    }
#else
    vector<bool> compare_ex(const ver_num_list& obj) const
    {
        vector<bool> result;
        result.reserve(VER_NUM_COUNT);
        const unsigned* nums_ptr1[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
        const unsigned* nums_ptr2[VER_NUM_COUNT] = { &obj.num1, &obj.num2, &obj.num3, &obj.num4};
        for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
        {
            if (*nums_ptr1[i] != *nums_ptr2[i])
                result.push_back(false);
            else
                result.push_back(true);
        }
        return result;
    }
#endif /* _HAS_TRADITIONAL_STL */

    bitset<VER_NUM_COUNT> compare_ex2(const ver_num_list& obj) const
    {
        bitset<VER_NUM_COUNT> result;
        const unsigned* nums_ptr1[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
        const unsigned* nums_ptr2[VER_NUM_COUNT] = { &obj.num1, &obj.num2, &obj.num3, &obj.num4};
        for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
        {
            if (*nums_ptr1[i] != *nums_ptr2[i])
                result[i] = (false);
            else
                result[i] = (true);
        }
        return result;
    }


    bool compare_ignore_n(const ver_num_list& obj, const unsigned ignore_n) const
    {
        const unsigned* nums_ptr1[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
        const unsigned* nums_ptr2[VER_NUM_COUNT] = { &obj.num1, &obj.num2, &obj.num3, &obj.num4};
        for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
        {
            if (ignore_n == i)
                continue;
            if (*nums_ptr1[i] != *nums_ptr2[i])
                return false;
        }
        return true;
    }
};///ver_num_list

bool operator == (const ver_num_list& left, const ver_num_list& right);
bool compare_ignore_n (const ver_num_list& left, const ver_num_list& right, const unsigned ignore_n);


template <typename _TChar>
basic_ostream<char_t, char_traits<char_t> >&
write( basic_ostream<char_t, char_traits<char_t> >& outs, const commit_info_t<char_t>& obj )
{
	for (unsigned i =0; i < S_TAGS_SIZE; ++i)
	{
		outs << ARRAY_TAGS[i] << obj.tag_values[i] << RETURN_TCHAR;
	}
	return outs;
}

template <typename _TChar>
basic_istream<char_t, char_traits<char_t> >&
read( basic_istream<char_t, char_traits<char_t> >& ins, commit_info_t<char_t>& obj )
{
	typedef basic_string<_TChar> _TString;
	unsigned getline_cout = 0;
	while (ins && getline_cout < S_TAGS_SIZE)
	{
		_TString read_line;
		std::getline(ins, read_line, RETURN_TCHAR);
		if (read_line.empty())
			continue;

		if (S_TAGS_SIZE-1 != getline_cout)// the last value.
		{
			/// 可能字段顺序错乱！需考虑遍历查找匹配赋值，并给予警告！
			unsigned find_pos = read_line.find(COMMIT_INFO_TAGS[getline_cout]);
			if (read_line.npos == find_pos)
			{
				//assert (false);
				unsigned tag_index = 0;
				for (; tag_index < S_TAGS_SIZE; ++tag_index)
				{
					find_pos = read_line.find(COMMIT_INFO_TAGS[tag_index]);
					if (read_line.npos != find_pos)
						break;
				}
				if (tag_index < S_TAGS_SIZE)
				{
					cerr_info(" warning: " << read_line << " .the field's seqeuence Exception.");
					obj.tag_values[tag_index] = trim_tstring(read_line.substr(find_pos + COMMIT_INFO_TAGS[tag_index].size(),
						wstring::npos));
					getline_cout = tag_index;
				}
				else
				{
					/// 没有字段名，强加给当前索引值getline_cout对于的字段域，并警告！
					cerr_info(" warning: " << read_line << " .no field name, Error.");
					obj.tag_values[getline_cout] = read_line;
				}
			}
			else
			{
				obj.tag_values[getline_cout] = trim_tstring(read_line.substr(find_pos + COMMIT_INFO_TAGS[getline_cout].size(),
					wstring::npos));

				cout_trace (obj.tag_values[getline_cout] << " [value] size:" << obj.tag_values[getline_cout].size());

			}
		}

		if (S_TAGS_SIZE-1 == getline_cout)
		{
			// just for VER_INFO_TAG...
			_TString value;
			value += RETURN_TCHAR;
			unsigned find_pos = read_line.find(COMMIT_INFO_TAGS[getline_cout]);
			if ( read_line.npos == find_pos)
			{
				/// 没有字段名，强加给当前索引值getline_cout对于的字段域，并警告！
				value += read_line + RETURN_TCHAR;
				cerr_info(" warning: find no field to match, but force it to assgin default field.");
			}
			{
				// just do it !
				read_line.clear();
				std::getline(ins, read_line, RETURN_TCHAR);
				while (!read_line.empty())
				{
					value += read_line + RETURN_TCHAR;
					read_line.clear();
					std::getline(ins, read_line, RETURN_TCHAR);
				}
				obj.tag_values[getline_cout] = value;
				cout_trace(obj.tag_values[getline_cout] << " [value] size:" << obj.tag_values[getline_cout].size());
				break;
			}
		}
		++getline_cout;
	}
	/// format the ver_num.
	if (!(*obj.ver_num).empty())
	{
		ver_num_list ver_num(*obj.ver_num);
		*obj.ver_num = ver_num.str();
	}

	return ins;
}

// template <typename _TChar>
// basic_istream<_TChar, char_traits<_TChar> >&
// junkun::commit_info_t<_TChar>::operator>>( basic_istream<_TChar, char_traits<_TChar> >& ins )
// {
// 	return read(ins, *this);
// }
// template <typename _TChar>
// basic_ostream<_TChar, char_traits<_TChar> >&
// junkun::commit_info_t<_TChar>::operator<<( basic_ostream<_TChar, char_traits<_TChar> >& outs )
// {
// 	return write(outs, *this);
// }


template <typename _TChar>
basic_istream<_TChar, char_traits<_TChar> >& operator >> (basic_istream<_TChar, char_traits<_TChar> >& ins, \
        commit_info_t<_TChar>& obj)
{
	//return read(ins, obj);
	typedef basic_string<_TChar> _TString;
	unsigned getline_cout = 0;
	while (ins && getline_cout < S_TAGS_SIZE)
	{
		_TString read_line;
		std::getline(ins, read_line, RETURN_TCHAR);
		if (read_line.empty())
			continue;

		if (S_TAGS_SIZE-1 != getline_cout)// the last value.
		{
			/// 可能字段顺序错乱！需考虑遍历查找匹配赋值，并给予警告！
			unsigned find_pos = read_line.find(COMMIT_INFO_TAGS[getline_cout]);
			if (read_line.npos == find_pos)
			{
				//assert (false);
				unsigned tag_index = 0;
				for (; tag_index < S_TAGS_SIZE; ++tag_index)
				{
					find_pos = read_line.find(COMMIT_INFO_TAGS[tag_index]);
					if (read_line.npos != find_pos)
						break;
				}
				if (tag_index < S_TAGS_SIZE)
				{
					cerr_info(" warning: " << read_line << " .the field's seqeuence Exception.");
					obj.tag_values[tag_index] = trim_tstring(read_line.substr(find_pos + COMMIT_INFO_TAGS[tag_index].size(),
						wstring::npos));
					getline_cout = tag_index;
				}
				else
				{
					/// 没有字段名，强加给当前索引值getline_cout对于的字段域，并警告！
					cerr_info(" warning: " << read_line << " .no field name, Error.");
					obj.tag_values[getline_cout] = read_line;
				}
			}
			else
			{
				obj.tag_values[getline_cout] = trim_tstring(read_line.substr(find_pos + COMMIT_INFO_TAGS[getline_cout].size(),
					wstring::npos));

				cout_trace (obj.tag_values[getline_cout] << " [value] size:" << obj.tag_values[getline_cout].size());

			}
		}

		if (S_TAGS_SIZE-1 == getline_cout)
		{
			// just for VER_INFO_TAG...
			_TString value;
			value += RETURN_TCHAR;
			unsigned find_pos = read_line.find(COMMIT_INFO_TAGS[getline_cout]);
			if ( read_line.npos == find_pos)
			{
				/// 没有字段名，强加给当前索引值getline_cout对于的字段域，并警告！
				value += read_line + RETURN_TCHAR;
				cerr_info(" warning: find no field to match, but force it to assgin default field.");
			}
			{
				// just do it !
				read_line.clear();
				std::getline(ins, read_line, RETURN_TCHAR);
				while (!read_line.empty())
				{
					value += read_line + RETURN_TCHAR;
					read_line.clear();
					std::getline(ins, read_line, RETURN_TCHAR);
				}
				obj.tag_values[getline_cout] = value;
				cout_trace(obj.tag_values[getline_cout] << " [value] size:" << obj.tag_values[getline_cout].size());
				break;
			}
		}
		++getline_cout;
	}
	/// format the ver_num.
	if (!(*obj.ver_num).empty())
	{
		ver_num_list ver_num(*obj.ver_num);
		*obj.ver_num = ver_num.str();
	}

	return ins;
}

template <typename _TChar>
basic_ostream<_TChar, char_traits<_TChar> >& operator << (basic_ostream<_TChar, char_traits<_TChar> >& outs,\
        const commit_info_t<_TChar>& obj)
{
	//return write(outs, obj);

	for (unsigned i =0; i < S_TAGS_SIZE; ++i)
	{
		outs << ARRAY_TAGS[i] << obj.tag_values[i] << RETURN_TCHAR;
	}
	return outs;
}

typedef commit_info_t<char_t> commit_info_tchar;
typedef std::pair<string_t, ver_num_list> dll_ver_info;
typedef set<commit_info_tchar, compare_moudle_name_less> set_commit_info;

bool check_revision(const set_commit_info& commit_info_vec, const vector<dll_ver_info>& dll_ver_info_vec);
void check_commit_info(const string_t& dll_dir, const string_t& commit_info_file);

bool init_net_connection(int argc, char* argv[]);
bool uninit_net_connection();

int do_work(int argc, char* argv[]);

}//namespace junkun
#endif
