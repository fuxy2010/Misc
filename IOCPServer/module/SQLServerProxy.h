#ifndef _SQLSERVER_PROXY_H_      
#define _SQLSERVER_PROXY_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "GeneralDef.h"
#include "IDataBaseProxy.h"
#include "DBProxy.h"
#include "MiscTool.h"
using namespace Common;

namespace ScheduleServer
{
	class CSQLServerProxy : public IDataBaseProxy
    {
    public:
        CSQLServerProxy();
        ~CSQLServerProxy();

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

		//特殊接口
		//根据数据表名获取表信息并生成创建该表的sql语句
		/*==================================================================================================================================
		[source_table_name]:需要复制的表明
		[target_table_name]:生成的新表表明
		函数调用成功，返回创建表的语句，调用失败，返回空值
		注意，此函数所支持的表的属性如下:
		1.字段数据类型仅支持：int, datetime, varchar(size), float
		2.对字段是否为主键有效
		4.对字段是否可为空有效
		5.对字段是否为自增量为1的主键有效
		==================================================================================================================================*/
		virtual std::string get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name);

		//根据表名获得字段名的列表
		virtual SS_Error get_field_name(const std::string& table_name, std::vector<std::string>& field_name);

		//根据表名获得主键字段名的列表
		virtual SS_Error get_key_name(const std::string& table_name, std::vector<std::string>& key_name);

	private:
		db_connection_t _db_connection_info;

		db_proxy _db_proxy;

    };
	
}

#endif  // _SQLSERVER_PROXY_H_       
