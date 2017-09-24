#ifndef _DBPROXY_H_        
#define _DBPROXY_H_

// 定义支持数据库类型
//#define SUPPORT_DB_MYSQL
//#define SUPPORT_DB_ORACLE
//#define SUPPORT_DB_MSSQL
//#define SUPPORT_DB_SQLITE3

#include <vector>
#include <string>
#include "DBLocker.h"


//#ifndef DBPROXY_API
//#    ifdef DBPROXY_API_EXPORTS
//#        define DBPROXY_API __declspec(dllexport)
//#    else
//#        define DBPROXY_API __declspec(dllimport)
//#    endif
//#endif

#pragma warning(disable:4100) 

namespace Common
{
    // 数据库类型定义
    enum db_type_t
    {
        db_type_mysql = 0,                          // MySQL类型数据库
        db_type_oracle,                             // Oracle类型数据库
        db_type_mssql,                              // MS SQLServer类型数据库
        db_type_db2,                                // DB2类型数据库
        db_type_access,                             // Access
        db_type_sqlite3,                            // SQLite类型数据库
        db_type_berkeleydb                          // BerkeleyDB类型数据库
    };

    // 数据表类型定义
    enum db_table_type_t
    {
        db_table_type_table = 0,                    // 表类型
        db_table_type_view                          // 视图类型
    };

    // 数据类型定义
    enum db_data_type_t
    {
        db_data_type_unknown = 0,                   // 未知类型
        db_data_type_int,                           // 整型类型
        db_data_type_double,                        // 浮点数类型
        db_data_type_string,                        // 字符串类型
        db_data_type_text,                          // 文本类型
		db_data_type_blob,                           // 二进制类型
		db_data_type_datetime
    };

    // 主键类型定义
    enum db_primary_key_type_t
    {
        db_primary_key_type_unique = 0,             // 唯一值类型
        db_primary_key_type_auto_increment          // 自增量类型
    };

    // 返回值类型定义
    enum db_retcode_t
    {
        db_retcode_ok = 0,                          // 数据库查询成功
        db_retcode_fail,                            // 数据库查询失败
        db_retcode_connection_fail,                 // 数据库连接失败
        db_retcode_result_empty,                    // 数据库查询结果为空
        db_retcode_buffer_insufficient,             // 用户缓冲区大小不足(输出BLOB类型数据)
        db_retcode_param_size_overflow,             // 预处理参数数量过多
        db_retcode_param_bind_fail,                 // 预处理参数绑定失败
        db_retcode_not_support                      // 数据库查询功能不支持
    };

    // 二进制数据最大长度定义
    const unsigned long MAX_BLOB_SIZE = 1024*1024;
    
    // 数据库查询预处理参数最大值定义
    const unsigned long MAX_PARAM_SIZE = 255;

    // 缓冲区类型定义
    typedef char db_buffer_t[MAX_BLOB_SIZE];

    // 字段信息类型定义
    typedef std::string db_field_t;

    // 字段信息列表类型定义
    typedef std::vector<db_field_t> db_field_list_t;

    // 主键信息类型定义
    typedef struct db_primary_key_t
    {
        db_primary_key_t(void) : name(""), type(db_primary_key_type_unique) {}
        db_primary_key_t(const std::string& name_,
                         const db_primary_key_type_t type_) : name(name_), type(type_) {}

        std::string name;
        db_primary_key_type_t type;
    } db_primary_key_t;

    // 数据表信息类型定义
    typedef struct db_table_info_t
    {
        db_table_info_t(void) : name(""), table_type(db_table_type_table) {}
        db_table_info_t(const std::string& name_,
                        const db_primary_key_t& primary_key_,
                        const db_field_list_t& field_list_,
                        db_table_type_t table_type_) : name(name_), primary_key(primary_key_), field_list(field_list_), table_type(table_type_) {}

        std::string name;                           // 数据表名称
        db_primary_key_t primary_key;               // 主键信息
        db_field_list_t field_list;                 // 字段列表
        db_table_type_t table_type;                 // 数据表类型
    } db_table_info_t;

	/*============================================================
	written by sunyc  */

	//记录字段属性类型定义
	typedef struct db_field_attr_t 
	{
		db_field_attr_t(void) : name(""), type(0), defined_size(0), field_atrr(0) {}

		std::string name;			//字段名字
		int type;					//字段数据类型
		int defined_size;			//字段定义大小
		int field_atrr;				//字段其余属性，包括是否为主键，是否可为空

	}db_field_attr_t;

	//字段属性集类型定义
	typedef std::vector<db_field_attr_t> db_field_attr_list_t;
	//================================================================================

    // 记录数据类型定义
    typedef struct db_data_t
    {
        db_data_t(void) : data(""), type(db_data_type_unknown) {}
        db_data_t(const std::string& data_, db_data_type_t type_=db_data_type_unknown) : data(data_), type(type_) {}

        std::string data; 
        db_data_type_t type;
    } db_data_t; 
    
    // 记录二进制数据类型定义
    typedef struct db_blob_data_t
    {
        db_blob_data_t(void) : data_ptr(NULL), data_len(0) {}
        db_blob_data_t(void * data_ptr_, size_t data_len_) : data_ptr(data_ptr_), data_len(data_len_) {}

        void * data_ptr;                            // 数据指针
        size_t data_len;                            // 数据长度
    } db_blob_data_t;

    // 数据库记录类型定义
    typedef std::vector<db_data_t> db_data_list_t;
    typedef std::vector<db_blob_data_t> db_blob_data_list_t;
    typedef db_data_list_t db_record_t;

    // 数据库记录列表类型定义
    typedef std::vector<db_record_t> db_record_list_t;

    // 数据集结构定义
    typedef struct db_recordset_t 
    {
        db_field_list_t field_list;                 // 字段列表
        db_record_list_t record_list;               // 记录列表
    } db_recordset_t;

    // 数据集列表类型定义
    typedef std::vector<db_recordset_t> db_dataset_t;

    // 数据库连接信息结构定义
    typedef struct db_connection_t
    {
        db_type_t db_type;                          // 数据库类型
        std::string db_server_addr;                 // 数据库服务器地址
		std::string db_server_port;                // 数据库服务器端口
        std::string db_name;                        // 数据库名称
        std::string db_user;                        // 数据库用户名
        std::string db_password;                    // 数据库用户密码
    } db_connection_t;

    //
    //  数据库操作日志接口类
    //
    /*class DBPROXY_API db_logger
    {
    public:
        virtual ~db_logger(void) {};
        virtual void log(const char* message) {};
        virtual void log(const std::string& message) 
        {
            log(message.c_str());
        };
    };

    typedef db_logger *db_logger_ptr;*/

    //
    // 数据库代理类
    //
    class db_proxy_pool;
    class  db_proxy
    {
    public:
        db_proxy(void);
        ~db_proxy(void);
    
    public:
        // 初始化数据库操作接口
        void init(const db_connection_t& db_connection);

		db_retcode_t connect();

        // 执行数据库操作(适用于创建、删除表或插入、更新、删除记录等操作)
        db_retcode_t exec_dml(const std::string& sql);
        db_retcode_t exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list);

        // 执行数据库操作(适用于插入、更新、删除二进制数据等操作)
        db_retcode_t exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data);

        // 执行数据库查询并获取数据
        db_retcode_t exec_scalar(const std::string& sql, db_data_t& db_data);

        // 执行数据库查询并获取二进制数据
        db_retcode_t exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data);

		//执行数据库查询获得字段属性
		db_retcode_t exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list);

        // 执行数据库查询并获取数据集
        db_retcode_t exec_select(const std::string& sql, db_recordset_t& db_recordset);
        db_retcode_t exec_select_ex(const std::string& sql, const db_data_list_t& db_data_list, db_recordset_t& db_recordset);

    private:
        db_proxy(const db_proxy&);
        db_proxy& operator=(const db_proxy&);

    private:
        db_proxy_pool* _pool_ptr;
		db_mutex _mutex;
    };

    //
    // 局部缓冲区封装类
    //
    //class scoped_buffer
    //{
    //public:
    //    scoped_buffer(size_t size = 0) : _ptr(NULL), _size(size)
    //    {
    //        reset(size);
    //    }

    //    ~scoped_buffer(void)
    //    {
    //        reset();
    //    }

    //    // 获取数组长度
    //    size_t size(void) const
    //    {
    //        return _size;
    //    }

    //    // 获取数组指针
    //    char * get(void) const
    //    {
    //        return _ptr;
    //    }
    //
    //    // 调整数组长度
    //    void reset(size_t size = 0)
    //    {
    //        if (_ptr)
    //        {
    //            delete[] _ptr;
    //            _ptr = NULL;
    //            _size = 0;
    //        }
    //        
    //        if (size > 0)
    //        {
    //            _size = size;
    //            _ptr = new char[size];
    //            ::memset(_ptr, 0, size);
    //        }
    //    }

    //private:
    //    scoped_buffer(const scoped_buffer&);
    //    scoped_buffer& operator=(const scoped_buffer&);

    //private:
    //    char * _ptr;
    //    size_t _size;
    //};
}

#endif  // _DBPROXY_H_
