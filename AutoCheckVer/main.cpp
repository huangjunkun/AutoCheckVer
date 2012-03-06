#include "stdafx.h"
#include <iostream>
#include "main.h"
#include "AutoCheckVer.h"

using namespace std;

void test_func(int argc, char* argv[]);



int _tmain(int argc, char* argv[])
{
	{
	using namespace junkun;
	do_work(argc, argv);
	}
///**
	/************************************************************************/
	std::cout << "Hello world!" << std::endl;
	getchar();
	return 0;
}

void test_func(int argc, char* argv[])
{
	// 	{// just test ...
	// 	using namespace junkun;
	// 	init_net_connection(argc, argv);
	// 	uninit_net_connection();
	// 	}
	{
	CHAR szDeviceName[80]; 
	DWORD dwResult, cchBuff = sizeof(szDeviceName); 

	// Call the WNetGetConnection function.
	//
	dwResult = WNetGetConnection("z:", 
		(LPSTR) szDeviceName, 
		&cchBuff); 

	switch (dwResult) 
	{ 
		//
		// Print the connection name or process errors.
		//
	case NO_ERROR: 
		std::cout << szDeviceName << " connect successfully.\n"; 
		break; 
		//
		// The device is not a redirected device.
		//
	case ERROR_NOT_CONNECTED: 
		std::cout << "Device z: not connected.\n"; 
		//
		// The device is not currently connected,
		//  but it is a persistent connection.
		//
	case ERROR_CONNECTION_UNAVAIL: 
		std::cout << "Connection unavailable.\n"; 
		//
		// Call an application-defined error handler.
		//
	default: 
		std::cout << "WNetGetConnection error. err_no:" << dwResult << "\n"; 
		return; 
	}

	}
/**
	{
	using namespace std;
	using namespace junkun;
	const string test_wstr0 = "模块文件名：";
	wstring test_wstr;
	const string test_directory = "D:\\Users\\huangjunkun\\Desktop\\";
	const string test_file = test_directory + "20110607.txt";
	wifstream  fin(test_file.c_str());
	fin >> test_wstr;
	cout << " " << test_wstr0 << "\n";
	//cout << " " << test_wstr << "\n";
	string test_str1;
	//xl::text::transcode::tstring_to_ANSI(test_wstr.c_str(), test_wstr.size(), test_str1);
	cout << " " << test_str1 << "\n";
	string::size_type find_pos = test_str1.find(test_wstr0);
	if (test_str1.npos == find_pos)
		cout << "no find.\n";
	else
		cout << " find it, and find pos:" << find_pos << "\n";

	}
/**
	{
	using namespace std;
	using namespace junkun;

	const basic_string<char_type>  test_directory = _T("D:\\Users\\huangjunkun\\Desktop\\");
	const basic_string<char_type> test_file = test_directory + _T("20110607.txt");
	basic_ifstream<char_type>  fin(test_file.c_str());
	vector<commit_info_t<char_type> > commit_info_vec;
	for (; !fin.eof(); )
	{
		commit_info_t<char_type> commit_info;
		fin >> commit_info;
		if (!commit_info.moudle_name.empty())
		{
			commit_info_vec.push_back(commit_info);
			cout << " commit_info_vec::push_back(...).\n";
		}
	}
	cout << "commit_info_vec.size() " << commit_info_vec.size() << "\n";
	for ( unsigned i = 0; i < commit_info_vec.size(); ++i)
		cout << commit_info_vec[i] << "\n";

	//wcout << " " << TrimString<wchar_t>(L"    hi huangjunkun.     ") << "[end]\n";
	//for (unsigned i = 0; i != ARRAY_TAGS_SIZE; ++i)
	//{
	//	//wcout << ARRAY_TAGS[i] << "\n";
	//	cout << ARRAY_TAGS[i].size() << "\n";
	//}
	////const wstring test_str = L"模块文件名：     backend_agent.dll";
	//wstring test_str;
	//wstring test_str0 = MOUDLE_NAME_TAGW;
	//fin >> test_str;
	//wcout << test_str << "\n";
	//unsigned find_pos = test_str.find(test_str0);
	//if (test_str.npos == find_pos)
	//	cout << "no find.\n";
	//else
	//	cout << "find_pos:" << find_pos << "\n";
	}

/**
	{
	using namespace std;
	using namespace junkun;
	const string test_str = "1, 1,  ?, 2222";//"1.2.?.4"
	const string test_str1 = "1, 1,  111, 2222";//"1.2.?.4"
	ver_num_list test(test_str);
	std::cout << " " << test.str() << "\n";
	ver_num_list test1(test_str1);
	std::cout << " " << test1.str() << "\n";
	cout << " operator == " << (test == test1) << "\n";
	cout << " compare_ignore_n[2] " << compare_ignore_n(test, test1, 2) << "\n";


	}
/**
	{
	using namespace std;
	//const bitset<12> mask(2730ul);
	//cout << "mask =      " << mask << endl;
	//bitset<12> x;
	//cout << "Enter a 12-bit bitset in binary: " << flush;
	//if (cin >> x)
	//{
	//	cout << "x =        " << x << endl;
	//	cout << "As ulong:  " << x.to_ulong() << endl;
	//	cout << "And with mask: " << (x & mask) << endl;
	//	cout << "Or with mask:  " << (x | mask) << endl;
	//}

	bitset<10> a_num;
	a_num[9] = true;
	cout << " " << a_num.to_ulong() << "\n";
	bitset<128> large_num;
	large_num[127] = true;
	cout << " " << large_num.to_ulong() << "\n";
	}

/**
	{
	using namespace std;
	using namespace junkun;
	const wstring test_directory = L"D:\\Program Files\\Thunder Network\\Thunder\\Program\\";
	const wstring test_file = test_directory + L"ThunderPlatform.exe";

	string app_ver = GetProductVersion(test_file);
	cout << "app_ver.size() " << app_ver.size() << "\n";
	cout << app_ver << "\n";
	vector<wstring> result;
	bool res = GetExtFileList(test_directory, wstring(L"dll"), result);
	assert (res);
	for (vector<wstring>::const_iterator citer = result.begin();
		citer != result.end(); ++citer)
	{
		wcout << (*citer) << "\n";
	}
	wcout << "result.size() " << result.size() << "\n";

	for (vector<wstring>::const_iterator citer = result.begin();
		citer != result.end(); ++citer)
	{
		cout << GetProductVersion(*citer) << "\n";
	}
	}
	/************************************************************************/
}
