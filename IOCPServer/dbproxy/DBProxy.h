#ifndef _DBPROXY_H_        
#define _DBPROXY_H_

// ����֧�����ݿ�����
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
    // ���ݿ����Ͷ���
    enum db_type_t
    {
        db_type_mysql = 0,                          // MySQL�������ݿ�
        db_type_oracle,                             // Oracle�������ݿ�
        db_type_mssql,                              // MS SQLServer�������ݿ�
        db_type_db2,                                // DB2�������ݿ�
        db_type_access,                             // Access
        db_type_sqlite3,                            // SQLite�������ݿ�
        db_type_berkeleydb                          // BerkeleyDB�������ݿ�
    };

    // ���ݱ����Ͷ���
    enum db_table_type_t
    {
        db_table_type_table = 0,                    // ������
        db_table_type_view                          // ��ͼ����
    };

    // �������Ͷ���
    enum db_data_type_t
    {
        db_data_type_unknown = 0,                   // δ֪����
        db_data_type_int,                           // ��������
        db_data_type_double,                        // ����������
        db_data_type_string,                        // �ַ�������
        db_data_type_text,                          // �ı�����
		db_data_type_blob,                           // ����������
		db_data_type_datetime
    };

    // �������Ͷ���
    enum db_primary_key_type_t
    {
        db_primary_key_type_unique = 0,             // Ψһֵ����
        db_primary_key_type_auto_increment          // ����������
    };

    // ����ֵ���Ͷ���
    enum db_retcode_t
    {
        db_retcode_ok = 0,                          // ���ݿ��ѯ�ɹ�
        db_retcode_fail,                            // ���ݿ��ѯʧ��
        db_retcode_connection_fail,                 // ���ݿ�����ʧ��
        db_retcode_result_empty,                    // ���ݿ��ѯ���Ϊ��
        db_retcode_buffer_insufficient,             // �û���������С����(���BLOB��������)
        db_retcode_param_size_overflow,             // Ԥ���������������
        db_retcode_param_bind_fail,                 // Ԥ���������ʧ��
        db_retcode_not_support                      // ���ݿ��ѯ���ܲ�֧��
    };

    // ������������󳤶ȶ���
    const unsigned long MAX_BLOB_SIZE = 1024*1024;
    
    // ���ݿ��ѯԤ����������ֵ����
    const unsigned long MAX_PARAM_SIZE = 255;

    // ���������Ͷ���
    typedef char db_buffer_t[MAX_BLOB_SIZE];

    // �ֶ���Ϣ���Ͷ���
    typedef std::string db_field_t;

    // �ֶ���Ϣ�б����Ͷ���
    typedef std::vector<db_field_t> db_field_list_t;

    // ������Ϣ���Ͷ���
    typedef struct db_primary_key_t
    {
        db_primary_key_t(void) : name(""), type(db_primary_key_type_unique) {}
        db_primary_key_t(const std::string& name_,
                         const db_primary_key_type_t type_) : name(name_), type(type_) {}

        std::string name;
        db_primary_key_type_t type;
    } db_primary_key_t;

    // ���ݱ���Ϣ���Ͷ���
    typedef struct db_table_info_t
    {
        db_table_info_t(void) : name(""), table_type(db_table_type_table) {}
        db_table_info_t(const std::string& name_,
                        const db_primary_key_t& primary_key_,
                        const db_field_list_t& field_list_,
                        db_table_type_t table_type_) : name(name_), primary_key(primary_key_), field_list(field_list_), table_type(table_type_) {}

        std::string name;                           // ���ݱ�����
        db_primary_key_t primary_key;               // ������Ϣ
        db_field_list_t field_list;                 // �ֶ��б�
        db_table_type_t table_type;                 // ���ݱ�����
    } db_table_info_t;

	/*============================================================
	written by sunyc  */

	//��¼�ֶ��������Ͷ���
	typedef struct db_field_attr_t 
	{
		db_field_attr_t(void) : name(""), type(0), defined_size(0), field_atrr(0) {}

		std::string name;			//�ֶ�����
		int type;					//�ֶ���������
		int defined_size;			//�ֶζ����С
		int field_atrr;				//�ֶ��������ԣ������Ƿ�Ϊ�������Ƿ��Ϊ��

	}db_field_attr_t;

	//�ֶ����Լ����Ͷ���
	typedef std::vector<db_field_attr_t> db_field_attr_list_t;
	//================================================================================

    // ��¼�������Ͷ���
    typedef struct db_data_t
    {
        db_data_t(void) : data(""), type(db_data_type_unknown) {}
        db_data_t(const std::string& data_, db_data_type_t type_=db_data_type_unknown) : data(data_), type(type_) {}

        std::string data; 
        db_data_type_t type;
    } db_data_t; 
    
    // ��¼�������������Ͷ���
    typedef struct db_blob_data_t
    {
        db_blob_data_t(void) : data_ptr(NULL), data_len(0) {}
        db_blob_data_t(void * data_ptr_, size_t data_len_) : data_ptr(data_ptr_), data_len(data_len_) {}

        void * data_ptr;                            // ����ָ��
        size_t data_len;                            // ���ݳ���
    } db_blob_data_t;

    // ���ݿ��¼���Ͷ���
    typedef std::vector<db_data_t> db_data_list_t;
    typedef std::vector<db_blob_data_t> db_blob_data_list_t;
    typedef db_data_list_t db_record_t;

    // ���ݿ��¼�б����Ͷ���
    typedef std::vector<db_record_t> db_record_list_t;

    // ���ݼ��ṹ����
    typedef struct db_recordset_t 
    {
        db_field_list_t field_list;                 // �ֶ��б�
        db_record_list_t record_list;               // ��¼�б�
    } db_recordset_t;

    // ���ݼ��б����Ͷ���
    typedef std::vector<db_recordset_t> db_dataset_t;

    // ���ݿ�������Ϣ�ṹ����
    typedef struct db_connection_t
    {
        db_type_t db_type;                          // ���ݿ�����
        std::string db_server_addr;                 // ���ݿ��������ַ
		std::string db_server_port;                // ���ݿ�������˿�
        std::string db_name;                        // ���ݿ�����
        std::string db_user;                        // ���ݿ��û���
        std::string db_password;                    // ���ݿ��û�����
    } db_connection_t;

    //
    //  ���ݿ������־�ӿ���
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
    // ���ݿ������
    //
    class db_proxy_pool;
    class  db_proxy
    {
    public:
        db_proxy(void);
        ~db_proxy(void);
    
    public:
        // ��ʼ�����ݿ�����ӿ�
        void init(const db_connection_t& db_connection);

		db_retcode_t connect();

        // ִ�����ݿ����(�����ڴ�����ɾ�������롢���¡�ɾ����¼�Ȳ���)
        db_retcode_t exec_dml(const std::string& sql);
        db_retcode_t exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list);

        // ִ�����ݿ����(�����ڲ��롢���¡�ɾ�����������ݵȲ���)
        db_retcode_t exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data);

        // ִ�����ݿ��ѯ����ȡ����
        db_retcode_t exec_scalar(const std::string& sql, db_data_t& db_data);

        // ִ�����ݿ��ѯ����ȡ����������
        db_retcode_t exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data);

		//ִ�����ݿ��ѯ����ֶ�����
		db_retcode_t exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list);

        // ִ�����ݿ��ѯ����ȡ���ݼ�
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
    // �ֲ���������װ��
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

    //    // ��ȡ���鳤��
    //    size_t size(void) const
    //    {
    //        return _size;
    //    }

    //    // ��ȡ����ָ��
    //    char * get(void) const
    //    {
    //        return _ptr;
    //    }
    //
    //    // �������鳤��
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
