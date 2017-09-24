#ifndef _DBPROXY_I_H_        
#define _DBPROXY_I_H_

#include "DBProxy.h"

namespace Common
{
    //
    // 数据库代理接口
    //
    class db_proxy_interface
    {
    public:
        db_proxy_interface(void) : _connected(false){};
        virtual ~db_proxy_interface(void) {};

        // 设置数据库操作日志接口
        void init(const db_connection_t& db_connection)
        {
            _db_connection = db_connection;
            //_logger_ptr = logger_ptr;
        }

        // 判断数据库连接状态
        bool is_connected(void)
        {
            return _connected;
        }

        // 连接数据库
        virtual db_retcode_t connect(void)
        {
            return db_retcode_not_support;
        }

        // 执行数据库操作(适用于创建、删除表或插入、更新、删除记录等操作)
        virtual db_retcode_t exec_dml(const std::string& sql)
        {
            return db_retcode_not_support;
        }

        virtual db_retcode_t exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list)
        {
            return db_retcode_not_support;
        }

        // 执行数据库操作(适用于插入、更新、删除二进制数据等操作)
        virtual db_retcode_t exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data)
        {
            return db_retcode_not_support;
        }

        // 执行数据库查询并获取数据
        virtual db_retcode_t exec_scalar(const std::string& sql, db_data_t& db_data)
        {
            return db_retcode_not_support;
        }

        virtual db_retcode_t exec_scalar_ex(const std::string& sql, const db_data_list_t& db_data_list, db_data_t& db_data)
        {
            return db_retcode_not_support;
        }

        // 执行数据库查询并获取二进制数据
        virtual db_retcode_t exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data)
        {
            return db_retcode_not_support;
        }

		//=============================================================================================by sunyc
		//执行数据库查询获得字段属性
		virtual db_retcode_t exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list)
		{
			return db_retcode_not_support;
		}
		//==========================================================================================================

        // 执行数据库查询并获取数据集
        virtual db_retcode_t exec_select(const std::string& sql, db_recordset_t& db_recordset)
        {
            return db_retcode_not_support;
        }

        virtual db_retcode_t exec_select_ex(const std::string& sql, const db_data_list_t& db_data_list, db_recordset_t& db_recordset)
        {
            return db_retcode_not_support;
        }

        // 断开数据库连接
        virtual db_retcode_t disconect(void)
        {
            return db_retcode_not_support;
        }
    
    protected:
        bool _connected;
        db_connection_t _db_connection;
        //db_logger_ptr _logger_ptr;
    };
}

#endif  // _DBPROXY_I_H_     
