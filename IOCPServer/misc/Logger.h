#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <list>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

#include "Locker.h"

// ȱʡ��־����
#define DEFAULT_TRACE_TYPE  0

// ȱʡ��־����ȼ�
#define DEFAULT_TRACE_LEVEL 0

// ȱʡ��־Ŀ¼
#define DEFAULT_LOG_DIR     ".//"

// ȱʡ��־��Ϣǰ׺
#define DEFAULT_LOG_PREFIX  ""

typedef enum
{
    RELEASE_LEVEL = 0,      // ��¼����������Ϣ 
    DEBUG_LEVEL_1 = 1,      // ��¼������Ϣ(����ӿڡ������ͷ���ֵ)
    DEBUG_LEVEL_2 = 2,      // ��¼������Ϣ(���ܺ����������ͷ���ֵ)
    DEBUG_LEVEL_3 = 3,      // ��¼������Ϣ(���ݿ������SQL���)
    DEBUG_LEVEL_4 = 4       // ��¼������Ϣ(������Ϣ)
} TRACE_LEVEL;

//
// ��־��¼��
//
class CLogger;
typedef std::map<int, std::string> LOGFILE_BY_TYPE;

class CLogger
{
public:
    CLogger();
    virtual ~CLogger();
    
    // �򿪵�����־�ļ�
    void open(const std::string& prefix, 
              const std::string& filename, 
              const std::string& dir = DEFAULT_LOG_DIR,
              int trace_type = DEFAULT_TRACE_TYPE);

    // �򿪶����־�ļ�
    void open(const std::string& prefix, 
              const LOGFILE_BY_TYPE& filename_map, 
              const std::string& dir = DEFAULT_LOG_DIR);

    // �����Ƿ������־������̨
    void set_on_screen(bool on_screen);

    // ������־����ȼ�
    void set_trace_level(int trace_level);

    // ������־�ȼ��ж��Ƿ������־
    bool can_trace(int trace_level);

    // �����־��Ϣ
    void trace_out(const std::string& message, 
                   int trace_level = DEFAULT_TRACE_LEVEL, 
                   int trace_type = DEFAULT_TRACE_TYPE,
                   bool on_screen = false);

    // ��ʾ��־��Ϣ
    void trace_onscreen(const std::string& message);

    // ��ȡ��־Ŀ¼
    std::string get_log_dir();

    //����Ŀ¼
    void create_dir(const std::string& path);

    //�淶��Ŀ¼��ʽ
    std::string simplify_dir(const std::string& path);

private:
    //
    // ��һ��־��
    //
    class CSingleLogger
    {
    public:
        CSingleLogger(int trace_type, const std::string& filename, const std::string& dir) : 
            _trace_type(trace_type),
            _filename(filename),
            _dir(dir)
        {
            _date = get_current_date_str();
        }

        ~CSingleLogger() 
        {
            close();
        }

        // ������־����
        inline int get_trace_type() 
        {
            return _trace_type;
        }

        // ����־�ļ�
        void open()
        {
            close();

            // ���ifstream���ļ�����·��������
            // ���ô���ҳΪ�������ģ�936�Ǽ������ĵĴ���ҳ��
            std::locale loc1 = std::locale::global(std::locale(".936"));

            std::string filepath = _dir + "/" + _date + " " + _filename;
            _ofs.open(filepath.c_str(), std::ios_base::out | std::ios_base::app);

            //�ָ�ԭ���Ĵ���ҳ
            std::locale::global(std::locale(loc1));
        };

        // �����־��Ϣ
        void trace_out(const std::string& message)
        {
            CSSLocker lock(&_log_mutex);

            // ��ȡ��ǰ������Ϣ
            std::string date = get_current_date_str();

            // ������ڱ仯���ر�ԭ��־�ļ�, �����µ���־�ļ�
            if (date != _date)
            {
                close();

                _date = date;
                open();
            }

            _ofs << message << std::endl;
        };

        // �ر���־�ļ�
        inline void close()
        {
            if (_ofs.is_open())
            {
                _ofs.close();
            }
        };

    private:
        // ��ȡ��ǰ�����ַ����������ʽ(YYYY-MM-DD)
        std::string get_current_date_str()
        {
            time_t timep;
            time(&timep);
            struct tm t;

#ifdef _WIN32
            localtime_s(&t, &timep);
#else
            struct tm tr;
            localtime_r(&time, &tr);
            t = tr;
#endif
            char buf[32] = {0};
            strftime(buf, sizeof(buf), "%Y-%m-%d", &t);

            return std::string(buf);
        }

    private:
        int _trace_type;        // ��־�������
        std::string _filename;  // ��־�ļ���
        std::string _dir;       // ��־�ļ��洢Ŀ¼
        std::string _date;      // ��־��ʼ��¼����
        std::ofstream _ofs;     // ��־�ļ���
        CSSMutex _log_mutex;      // ������
    };

	// ��ȡ��ǰ����ʱ���ַ���
	std::string get_current_datetime_str();

private:
    int _trace_level;                               // ��־����ȼ�
    std::string _dir;                               // ��־�ļ�Ŀ¼
    std::string _prefix;                            // ��־��Ϣǰ׺
    std::map<int, CSingleLogger*> _logger_ptr_map;  // ��־�������б�
    CSSMutex _screen_mutex;                           // ������
};

#endif // _LOGGER_H_
