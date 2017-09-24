#ifndef _MYSQL_PROXY_H_      
#define _MYSQL_PROXY_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "IDataBaseProxy.h"
#include "DBProxy.h"
#include "MiscTool.h"
#include "mysql.h"

using namespace Common;

namespace ScheduleServer
{
	class CMySQLProxy : public IDataBaseProxy
    {
    public:
        CMySQLProxy();
        ~CMySQLProxy();

    public:
        //启动数据库
        virtual SS_Error start();

        //关闭数据库
        virtual SS_Error shutdown();
        
        //数据库配置
		virtual SS_Error set_connection_info(const DB_SERVER_INFO& info);

		//执行SQL语句
		virtual SS_Error execute_sql(const std::string sql_str, SS_CTX& res_ctx);
        	
        //查询记录
        virtual SS_Error query_record(const std::string sql_str, SS_CTX& res_ctx);

		virtual std::string get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name);

		//根据表名获得字段名的列表
		virtual SS_Error get_field_name(const std::string& table_name, std::vector<std::string>& field_name);

		//根据表名获得主键字段名的列表
		virtual SS_Error get_key_name(const std::string& table_name, std::vector<std::string>& key_name);

	private:
		//db_connection_t _db_connection_info;

		//db_proxy _db_proxy;
		DB_SERVER_INFO _info;
		MYSQL _mysql;

    };
	
}

#endif  // _MYSQL_PROXY_H_       
