#include "Logger.h"
#include <errno.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#endif

using namespace std;

// ----------------------------------------------------------------------
// ������: CLogger::CLogger
// ����: chenqg
// ����: CLogger��Ĺ��캯������ʼ����س�Ա
// ����ֵ: ��
// ����: 
//   [void]  
// ----------------------------------------------------------------------
CLogger::CLogger() :
    //_on_screen(false),
    _trace_level(DEFAULT_TRACE_LEVEL),
    _dir(DEFAULT_LOG_DIR),
    _prefix(DEFAULT_LOG_PREFIX)
{
    _logger_ptr_map.clear();
}

// ----------------------------------------------------------------------
// ������: CLogger::~CLogger
// ����: chenqg
// ����: CLogger��������������ر�������־�ļ�
// ����ֵ: ��
// ����: 
//   [void]  
// ----------------------------------------------------------------------
CLogger::~CLogger()
{
    // �ر�������־�ļ�
    std::map<int, CSingleLogger*>::iterator iter = _logger_ptr_map.begin();
    while (iter != _logger_ptr_map.end())
    {
        delete iter->second;
        ++iter;
    }
}

// ----------------------------------------------------------------------
// ������: CLogger::get_current_datetime_str
// ����: wangq
// ����: ��ȡ��ǰ����ʱ�䣬�����ʽ(YYYY-MM-DD HH:mm:ss.xxx)
// ����ֵ: [std::string] ��ǰ�����ַ���
// ����: 
//   [void]  
// ----------------------------------------------------------------------
std::string CLogger::get_current_datetime_str()
{
    struct _timeb tb;
    _ftime64_s(&tb);

    time_t time = tb.time;

    struct tm t;
#ifdef _WIN32
    localtime_s(&t, &time);
#else
    struct tm tr;
    localtime_r(&time, &tr);
    t = tr;
#endif

    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);

    std::ostringstream os;
    os << buf << ".";
    os.fill('0');
    os.width(3);
    os << tb.millitm;
    return os.str();
}

// ----------------------------------------------------------------------
// ������: CLogger::open
// ����: chenqg
// ����: �򿪵�����־�ļ�
// ����ֵ: [void]
// ����: 
//   [const std::string& prefix] ������־��Ϣǰ׺
//   [const std::string& filename] ������־�ļ���
//   [const std::string& dir] ������־�ļ�Ŀ¼
//   [int trace_type] ������־����
// ----------------------------------------------------------------------
void CLogger::open(const std::string& prefix, const std::string& filename, const std::string& dir, int trace_type)
{
    // ������־��Ϣǰ׺
    _prefix = prefix;

    // ���沢��ʽ����־Ŀ¼��Ϣ
	if (dir.empty())
    {
        _dir = DEFAULT_LOG_DIR;
    }
	else 
    {
        _dir = simplify_dir(dir);
    }
	create_dir(_dir);

    // ������һ��־������󣬴���־�ļ�
    CSingleLogger* logger_ptr = new CSingleLogger(trace_type, filename, dir);
    logger_ptr->open();

    // ������־���ͺ���־�ļ���Ϣ
    _logger_ptr_map[trace_type] = logger_ptr;
}

// ----------------------------------------------------------------------
// ������: CLogger::open
// ����: chenqg
// ����: �򿪶����־�ļ�
// ����ֵ: [void]
// ����: 
//   [const std::string& prefix] ������־��Ϣǰ׺
//   [const std::map<int, std::string>& filename_map] ������־�ļ��б�
//   [const std::string& dir] ������־�ļ�Ŀ¼
// ----------------------------------------------------------------------
void CLogger::open(const std::string& prefix, const std::map<int, std::string>& filename_map, const std::string& dir)
{
    // ������־��Ϣǰ׺
    _prefix = prefix;

    // ���沢��ʽ����־Ŀ¼��Ϣ
	if (dir.empty())
    {
        _dir = DEFAULT_LOG_DIR;
    }
	else 
    {
        _dir = simplify_dir(dir);
    }
	create_dir(_dir);

    // ��־�ļ��б�ѭ������
    std::map<int, std::string>::const_iterator iter = filename_map.begin();
    while (iter != filename_map.end())
    {
        // ������һ��־������󣬴���־�ļ�
        CSingleLogger* logger_ptr = new CSingleLogger(iter->first, iter->second, _dir);
        logger_ptr->open();

        // ������־���ͺ���־�ļ���Ϣ
        _logger_ptr_map[iter->first] = logger_ptr;

        ++iter;
    }
}

// ----------------------------------------------------------------------
// ������: CLogger::set_trace_level
// ����: chenqg
// ����: ������־����ȼ�
// ����ֵ: [void]
// ����: 
//   [int trace_level] ������־����ȼ�
// ----------------------------------------------------------------------
void CLogger::set_trace_level(int trace_level)
{
    _trace_level = trace_level;
}

// ----------------------------------------------------------------------
// ������: CLogger::can_trace
// ����: chenqg
// ����: ������־����ȼ��ж��Ƿ���Ϣд����־�ļ�
// ����ֵ: [bool] true д����־�ļ� false ��д����־�ļ�
// ����: 
//   [int trace_level] ������־����ȼ�
// ----------------------------------------------------------------------
bool CLogger::can_trace(int trace_level)
{
    return (trace_level <= _trace_level);
}

// ----------------------------------------------------------------------
// ������: CLogger::trace_out
// ����: chenqg
// ����: ������־����ȼ�����־���������־��Ϣ�����������з���־�ļ�
// ����ֵ: [void]
// ����: 
//   [const std::string& message] ������־��Ϣ
//   [int trace_level] ������־����ȼ�
//   [int trace_type] ������־����
//   [bool on_screen] �����Ƿ�����Ļ��ʾ��־��Ϣ
// ----------------------------------------------------------------------
void CLogger::trace_out(const std::string& message, int trace_level, int trace_type, bool on_screen)
{
    // �ж��Ƿ���Ϣд����־�ļ�
    if (!can_trace(trace_level)) return;

    // ����־��Ϣǰ�渽�Ӽ�¼ʱ���ǰ׺
    std::ostringstream ss;
    ss << message << " " << _prefix << ": " << get_current_datetime_str() << endl;

    // ������־���Ͳ��Ҷ�Ӧ����־��Ϣ�������
    CSingleLogger* pSingleLogger = _logger_ptr_map[trace_type];
    if (pSingleLogger != NULL)
    {
        // ��¼��־��Ϣ
        pSingleLogger->trace_out(ss.str());
    }
    
    // ����Ļ�����־��Ϣ
    if (true == on_screen)  
    {
        trace_onscreen(ss.str());
    }
}

// ----------------------------------------------------------------------
// ������: CLogger::trace_onscreen
// ����: chenqg
// ����: ��ʾ��־��Ϣ
// ����ֵ: [void]
// ����: 
//   [const std::string& message] ������־��Ϣ
// ----------------------------------------------------------------------
void CLogger::trace_onscreen(const std::string& message)
{
    CSSLocker lock(&_screen_mutex);
    cout << message << endl;
}

// ----------------------------------------------------------------------
// ������: CLogger::get_log_dir
// ����: chenqg
// ����: ��ȡ��־Ŀ¼
// ����ֵ: [std::string] �����־Ŀ¼
// ����: 
//   [void]
// ----------------------------------------------------------------------
std::string CLogger::get_log_dir()
{
    return _dir;
}

// ----------------------------------------------------------------------
// ������: CLogger::create_dir
// ����: wangq
// ����: ����Ŀ¼
// ����ֵ: [void]
// ����: 
//   [const std::string& path] Ŀ¼·��
// ----------------------------------------------------------------------
void CLogger::create_dir(const std::string& path)
{
    int res = -1;
#ifdef _WIN32
    res = ::_mkdir(path.c_str());
#else
    res = ::mkdir(path.c_str(), 0x777);
#endif

    if(res == -1)
    {
        if(errno != EEXIST)
        {
            throw "cannot create directory " + path;
        }
    }
}

// ----------------------------------------------------------------------
// ������: CLogger::simplify_dir
// ����: wangq
// ����: ��׼��Ŀ¼�ַ���
// ����ֵ: [std::string] ת�����Ŀ¼·��
// ����: 
//   [const std::string& path] Ŀ¼·���ַ���
// ----------------------------------------------------------------------
std::string CLogger::simplify_dir(const std::string& path)
{
    string result = path;

    string::size_type pos;

#ifdef _WIN32
    for(pos = 0; pos < result.size(); ++pos)
    {
        if(result[pos] == '\\')
        {
            result[pos] = '/';
        }
    }
#endif

    pos = 0;
    while((pos = result.find("//", pos)) != string::npos)
    {
        result.erase(pos, 1);
    }

    pos = 0;
    while((pos = result.find("/./", pos)) != string::npos)
    {
        result.erase(pos, 2);
    }

    if(result.substr(0, 2) == "./")
    {
        result.erase(0, 2);
    }

    if(result == "/." ||
        result.size() == 4 && isalpha(result[0]) && result[1] == ':' && result[2] == '/' && result[3] == '.')
    {
        return result.substr(0, result.size() - 1);
    }

    if(result.size() >= 2 && result.substr(result.size() - 2, 2) == "/.")
    {
        result.erase(result.size() - 2, 2);
    }

    if(result == "/" || result.size() == 3 && isalpha(result[0]) && result[1] == ':' && result[2] == '/')
    {
        return result;
    }

    if(result.size() >= 1 && result[result.size() - 1] == '/')
    {
        result.erase(result.size() - 1);
    }

    return result;
}

