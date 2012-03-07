// AutoCheckVer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AutoCheckVer.h"
//#include <xl_lib\xl_lib.h>

namespace junkun
{
using namespace std;
using namespace junkun;

bool operator == (const ver_num_list& left, const ver_num_list& right)
{
    return left.compare(right);
}

bool compare_ignore_n (const ver_num_list& left, const ver_num_list& right, const unsigned ignore_n)
{
    return left.compare_ignore_n(right, ignore_n);
}

///目录是否存在的检查：
bool check_revision(const set_commit_info& commit_info_vec, const vector<dll_ver_info>& dll_ver_info_vec)
{
    bool res_check = true;
    for (vector<dll_ver_info>::const_iterator citer = dll_ver_info_vec.begin();
            citer != dll_ver_info_vec.end(); ++citer)
    {
        //typedef set_commit_info::
        const string_t& this_moudle_name = (*citer).first;
        const ver_num_list& this_ver_num = (*citer).second;
        const commit_info_tchar::TString ver_str = this_ver_num.str();
// 			commit_info_tchar compare_value;
// 			*compare_value.moudle_name = this_moudle_name;
// 			set_commit_info::const_iterator find_pos = commit_info_vec.find(compare_value);
        compare_moudle_name_equal moudle_name_equal;
        set_commit_info::const_iterator find_pos = commit_info_vec.end();
        for (set_commit_info::const_iterator citer1 = commit_info_vec.begin(); citer1 != commit_info_vec.end();
                ++citer1)
            if (this_moudle_name == *(*citer1).moudle_name)
                find_pos = citer1;

        if (commit_info_vec.end() != find_pos && compare_ignore_n(ver_str,*(*find_pos).ver_num, 2) )
        {
            cout_trace(" dll:" << (*citer).first << " ver:" << ver_str << " compare OK.");
        }
        else
        {
            /// warning ...
            cerr_info(" dll:" << (*citer).first << " ver:" << ver_str << " compare error!");
            res_check = false;
        }
    }
    if (commit_info_vec.size() != dll_ver_info_vec.size())
    {
        cerr_info(" warning: commit_info items'count: " << commit_info_vec.size()
                  << " dll items'count: " << dll_ver_info_vec.size() << ", not equal.");
        res_check = false;
    }

    return res_check;
}

void check_commit_info(const string_t& dll_dir, const string_t& commit_info_file)
{
    if (!check_folder_exist(dll_dir))
    {
        cerr_info(" error: dll_dir is error!");
        //assert_false_notify (dll_dir_is_error);
        return;
    }
#if defined(_MSC_VER) && (_MSC_VER < 1400)
    string commit_info_file_a;
    xl::text::transcode::tstring_to_ANSI(commit_info_file.c_str(), commit_info_file.size(), commit_info_file_a);
    basic_ifstream<char_t>  fin(commit_info_file_a.c_str());
#else
    basic_ifstream<char_t>  fin(commit_info_file.c_str());
#endif

    if (!fin)
    {
        cerr_info(" error: commit_info_file is error!");
        //assert_false_notify (commit_info_file_is_error);
        return;
    }

    set_commit_info commit_info_vec;
    for (; (fin); )///(fin) && !fin.eof()
    {
        commit_info_tchar commit_info;
        fin >> commit_info;
        //commit_info.operator >> (fin);
        if (!(*commit_info.moudle_name).empty())
        {
            std::pair<set_commit_info::iterator, bool> res_pair;////set_commit_info::_Pairib res_pair;
            res_pair = commit_info_vec.insert(commit_info);
            if (!res_pair.second)
            {
                /// 多次的提包规范，更新后面最新的并给予警告！warning ...
                commit_info_tchar& to_update = const_cast<commit_info_tchar&>(*res_pair.first);
                if (to_update.revision_num < commit_info.revision_num)
                {
                    to_update = commit_info;
                    cerr_info(" warning: this appears repeatedly, and ver_num is newer so update commit_info["
                              << to_update.moudle_name << "]");
                }
                else
                {
                    cerr_info(" error: this appears repeatedly, but ver_num is older." << to_update.moudle_name << "]");
                    assert_false_notify (this_commit_info_appears_repeatedly_but_ver_num_is_older);
                }

            }
            cout_trace(" commit_info_vec::push_back(...).");
        }
    }
    cout_trace("commit_info_vec.size() " << commit_info_vec.size());

    vector<string_t> dll_file_list;
    vector<string_t> default_ext_name;
    default_ext_name.push_back(_T("dll"));
    default_ext_name.push_back(_T("exe"));
    get_ext_file_list(dll_dir, default_ext_name, dll_file_list);
    vector<dll_ver_info> dll_ver_info_vec;
    for (vector<string_t>::iterator iter = dll_file_list.begin();
            iter != dll_file_list.end(); ++iter)
    {
        const string_t dll_full_file_name = dll_dir+(*iter);
        const string_t ver_num = get_product_ver(dll_full_file_name);

        if (!ver_num.empty())
            dll_ver_info_vec.push_back(std::make_pair(*iter, ver_num_list(ver_num)));
        else
        {
            cerr_info("error: this dll[" << dll_full_file_name << "] 's ver_num is empty.");
            assert_false_notify (ver_num_is_empty);
        }
    }
#ifdef RECORD_LOG
//#if 1
    // print commit_info_vec
    for ( set_commit_info::iterator iter = commit_info_vec.begin();
            iter != commit_info_vec.end(); ++iter)
        cout_trace((*iter));

    // print dll_ver_info_vec
    for (vector<dll_ver_info>::iterator iter = dll_ver_info_vec.begin();
            iter != dll_ver_info_vec.end(); ++iter)
    {
        const dll_ver_info& obj(*iter);
        cout_trace(obj.first << ": " << obj.second.str());
    }
#endif
    bool res_check = check_revision(commit_info_vec, dll_ver_info_vec);
    cout_trace(" res_check:" << res_check);
}


bool ver_num_list::reset( const string_t& str )
{
    //int res_sscanf = sscanf(str.c_str(), "%d.%d.%d.%d", &num1, &num2, &num3, &num4);
    //if (EOF != res_sscanf)
    //{
    //	cout_trace ("res_sscanf " << res_sscanf << " EOF != res_sscanf ERROR!");
    //	assert (false);
    //}
    string_t str_num;
    unsigned* nums_ptr[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
    unsigned nums_index = 0;
    string_t::const_iterator before_it = str.begin();
    string_t::const_iterator after_it = before_it;

    for (; after_it != str.end();)
    {
        if (nums_index >= VER_NUM_COUNT)
        {
            return false;
        }

        str_num.clear();
        while ( before_it != str.end() && !isdigit(*before_it) && _T('?') != *before_it)
            ++before_it;
        after_it = before_it;
        while (after_it != str.end() && (isdigit(*after_it) || _T('?') == *after_it))
            ++after_it;

        if (before_it == after_it)
            continue;

        str_num.assign(before_it, after_it);
        //todo: warn ...
        if (_T("?") == str_num)
            *nums_ptr[nums_index++] = UINT_MAX;
        else
#ifdef _UNICODE
            *nums_ptr[nums_index++] = _wtoi(str_num.c_str());
#else
            *nums_ptr[nums_index++] = atoi(str_num.c_str());
#endif

        before_it = after_it;
    }
    if (nums_index == VER_NUM_COUNT)
        return true;
    else
        return false;
}

string_t ver_num_list::str() const
{
    string_t res_str;
    const unsigned* nums_ptr[VER_NUM_COUNT] = { &num1, &num2, &num3, &num4};
    for (unsigned i = 0; i < VER_NUM_COUNT; ++i)
    {
        if (*nums_ptr[i] == UINT_MAX)
        {
            res_str.append(_T("?."));
        }
        else
        {
            char_t num[16];
#ifdef _UNICODE
            _itow(*nums_ptr[i], num, 10);
#else
            _itoa(*nums_ptr[i], num, 10);
#endif
            res_str.append(string_t(num)+_T("."));
        }
    }
    return res_str.substr(0, res_str.size()-1);
}

}//namespace junkun



namespace junkun
{

static const char_t s_net_user_name[32] = _T("downloadlib"),
        s_net_password[32] = _T("test"),
                             s_net_local_name[32] = _T("Q:"),
                                     s_net_remote_name[MAX_PATH] = _T("\\\\192.168.14.219\\download$");//"\\\\192.168.8.188\\media$"

bool init_net_connection(int argc, char* argv[])
{
    NETRESOURCE net_res;
    net_res.dwType = RESOURCETYPE_ANY;
    net_res.lpLocalName = const_cast<char_t*> (s_net_local_name);
    net_res.lpRemoteName = const_cast<char_t*> (s_net_remote_name);
    net_res.lpProvider = NULL;

    DWORD dwResult = WNetAddConnection2(&net_res, (LPSTR)s_net_password, (LPSTR)s_net_user_name, CONNECT_REDIRECT);//
    //assert (NO_ERROR == dwResult);
    // 	ERROR_ALREADY_ASSIGNED
    // 	ERROR_BAD_DEV_TYPE
    // 	ERROR_BAD_DEVICE
    // 	ERROR_BAD_NET_NAME
    // 	ERROR_BAD_PROFILE
    // 	ERROR_BAD_PROVIDER
    // 	ERROR_BUSY
    // 	ERROR_CANCELLED
    // 	ERROR_CANNOT_OPEN_PROFILE
    // 	ERROR_DEVICE_ALREADY_REMEMBERED
    // 	ERROR_EXTENDED_ERROR
    // 	ERROR_INVALID_PASSWORD
    // 	ERROR_NO_NET_OR_BAD_PATH
    // 	ERROR_NO_NETWORK
    if (NO_ERROR != dwResult)
    {
        cerr_info(" error: connect to server failed! err_no:" << dwResult);
        return false;
    }
    else /// NO_ERROR
        return true;
}

bool uninit_net_connection()
{
    DWORD dwRes = WNetCancelConnection2(s_net_remote_name, 0, true);//CONNECT_UPDATE_PROFILE, TRUE
    if (NO_ERROR == dwRes)
        return true;
    else
    {
        cerr_info(" error: disconnect to server failed! err_no:" << dwRes);
        return false;
    }
}

void load_config(const string_t& config_file)
{
    c_single_file conf_file(config_file, _T("r"));
    const char_t THE_CONFIG_NOTE_SYMBOL = _T('#');
    char_t r_buffer[MAX_PATH];
    while (0 != fgets_t(r_buffer, sizeof(r_buffer), conf_file.file_ptr()))
    {
        string_t r_str(r_buffer);
        r_str.resize(r_str.size() - 1);//pass '\n'
        r_str = trim_tstring(r_str);
        if ( !r_str.empty() && r_str[0] != THE_CONFIG_NOTE_SYMBOL)
        {
            const string_t DEFAULT_SERVER_PATH_FIELD_NAME = _T("default_server_path");
            string_t field_value;
            unsigned find_pos = r_str.find(DEFAULT_SERVER_PATH_FIELD_NAME);
            if (string_t::npos != find_pos)
            {
                s_default_server_path = trim_tstring(r_str.substr(find_pos + DEFAULT_SERVER_PATH_FIELD_NAME.size()));
                cout_trace (DEFAULT_SERVER_PATH_FIELD_NAME << s_default_server_path);
                continue;
            }
            const string_t COMMIT_INFO_TAG_FIELD_NAME = _T("commit_info_tag");
            find_pos = r_str.find(COMMIT_INFO_TAG_FIELD_NAME);
            if (string_t::npos != find_pos)
            {
                field_value = trim_tstring(r_str.substr(find_pos + COMMIT_INFO_TAG_FIELD_NAME.size()));
                COMMIT_INFO_TAGS.push_back(field_value);
                cout_trace (COMMIT_INFO_TAG_FIELD_NAME << field_value);
                continue;
            }
            // todo other config ...
            cout_trace ("warning: ignore the string " << r_str);
        }
        else cout_trace ("note:" << r_str);
    }
    assert (COMMIT_INFO_TAGS.size());
    S_TAGS_SIZE = COMMIT_INFO_TAGS.size();
    cout_info("S_TAGS_SIZE " << S_TAGS_SIZE);
}

int do_work(int argc, char* argv[])
{
    using namespace junkun;

    load_config();
    time_t now_time;
    time(&now_time);
    struct tm* local_tm;
    local_tm = localtime(&now_time);

    ostringstream_t oss;//"D:\\Users\\huangjunkun\\Desktop\\"
    //oss << _T("\\\\192.168.14.219\\download\\20110623");/// for test

    cout_trace ("s_default_server_path:" << s_default_server_path);
    cout_info("S_TAGS_SIZE " << S_TAGS_SIZE);
    oss << s_default_server_path << (local_tm->tm_year + 1900);
    unsigned month = local_tm->tm_mon+1;
    if (month < 10)
        oss << "0" << month;
    else
        oss << month;
    unsigned day = local_tm->tm_mday;
    if (day < 10)
        oss << "0" << day;
    else
        oss << day;
    const string_t default_dll_dir = oss.str() + _T("\\release\\");
    const string_t default_txt_file = oss.str() + _T(".txt");

    cout_trace(" " << default_dll_dir << " " << default_txt_file);
    if (local_tm->tm_mday)

        cout_trace(" 输入DLL存放路径，直接回车默认[" << default_dll_dir << "]:");
    char_t dll_dir[MAX_PATH] = {0};
    tcin.getline(dll_dir, sizeof(dll_dir), _T('\n'));
    if (0 == strlenT(dll_dir))
        strcpyT(dll_dir, default_dll_dir.c_str());
    tcin.clear();

    cout_trace(" 输入提包规范包含路径的完整文件名，直接回车默认[" << default_txt_file << "]:");
    char_t txt_file[MAX_PATH] = {0};
    tcin.getline(txt_file, sizeof(txt_file), _T('\n'));
    if (0 == strlenT(txt_file))
        strcpyT(txt_file, default_txt_file.c_str());
    tcin.clear();
    cout_trace(" " << dll_dir << " " << txt_file);

    check_commit_info(string_t(dll_dir), string_t(txt_file));

    return 0;
}
}// namespace junkun

