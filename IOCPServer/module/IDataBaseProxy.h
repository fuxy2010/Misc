// **********************************************************************
// 作者: 傅佑铭
// 版本: 1.0
// 日期: 2011-01 ~ 2011-11
// 修改历史记录: 
// 日期, 作者, 变更内容
// **********************************************************************
#ifndef _I_DB_PROXY_H_
#define _I_DB_PROXY_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "IModule.h"

namespace ScheduleServer
{
	typedef struct tagDB_SERVER_INFO
	{
		std::string server_ip;
		std::string server_port;
		std::string db_name;
		std::string db_username;
		std::string db_password;

		tagDB_SERVER_INFO() : server_ip(""), server_port(""), db_name(""), db_username(""), db_password("")
		{


		}
	}DB_SERVER_INFO;

    class IDataBaseProxy;
    typedef IDataBaseProxy* IDataBaseProxyPtr;

    class IDataBaseProxy: public IModule
    {
    public:
		//启动数据库
		virtual SS_Error start() = 0;
		
		//关闭数据库
		virtual SS_Error shutdown() = 0;

		//数据库配置
		virtual SS_Error set_connection_info(const DB_SERVER_INFO& info) = 0;

        //执行SQL语句
        virtual SS_Error execute_sql(const std::string sql_str, SS_CTX& res_ctx) = 0;
        	
        //查询记录
        virtual SS_Error query_record(const std::string sql_str, SS_CTX& res_ctx) = 0;

		//特殊接口
		//根据数据表名获取表信息并生成创建该表的sql语句
		virtual std::string get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name) = 0;

		//根据表名获得字段名的列表
		virtual SS_Error get_field_name(const std::string& table_name, std::vector<std::string>& field_name) = 0;

		//根据表名获得主键字段名的列表
		virtual SS_Error get_key_name(const std::string& table_name, std::vector<std::string>& key_name) = 0;
    };
}

#endif // _I_DB_PROXY_H_