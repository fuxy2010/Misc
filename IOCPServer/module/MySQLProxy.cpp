#include "stdafx.h"//just for log
#include "ScheduleServer.h"//just for log
#include "GeneralDef.h"
#include "MySQLProxy.h"
#include <sstream>
using namespace ScheduleServer;

CMySQLProxy::CMySQLProxy()
{
}

CMySQLProxy::~CMySQLProxy()
{
}

SS_Error CMySQLProxy::start()
{
	if(NULL == mysql_init(&_mysql))
	{
		return SS_InvalidDBConnection;
	}
	
	if(NULL == mysql_real_connect(&_mysql, _info.server_ip.c_str(),_info.db_username.c_str(), _info.db_password.c_str(),
								_info.db_name.c_str(), MiscTools::parse_string_to_type<unsigned int>(_info.server_port),
								NULL,0))
	{
		return SS_InvalidDBConnection;
	}
	
	if(0 != mysql_set_character_set(&_mysql, "GBK"))
	{
		return SS_InvalidDBConnection;
	}

	return SS_NoErr;
}

SS_Error CMySQLProxy::shutdown()
{
	return SS_NoErr;
}

SS_Error CMySQLProxy::set_connection_info(const DB_SERVER_INFO& info)
{
	_info.server_ip = info.server_ip;
	_info.server_port = info.server_port;
	_info.db_name= info.db_name;
	_info.db_username = info.db_username;
	_info.db_password = info.db_password;

	return SS_NoErr;
}

SS_Error CMySQLProxy::execute_sql(const std::string sql_str, SS_CTX& res_ctx)
{
	LOG_WRITE("<EXE SQL> " + sql_str, 1, true);

	res_ctx.clear();

	res_ctx["success"] = "false";
	
	if(!mysql_query(&_mysql, sql_str.c_str()))
	{
		res_ctx["success"] = "true";
		return SS_NoErr;	
	}

	return SS_ExecFail;
}

SS_Error CMySQLProxy::query_record(const std::string sql_str, SS_CTX& res_ctx)
{
	LOG_WRITE("<QUERY SQL> " + sql_str, 1, true);

	res_ctx.clear();

	res_ctx["success"] = "false";

	{
		//MYSQL_RES *result;
		MYSQL_ROW row;
		MYSQL_FIELD* fd;

		if(!mysql_query(&_mysql, sql_str.c_str()))
		{
			MYSQL_RES* result = mysql_store_result(&_mysql);

			if(NULL != result)
			{
				unsigned long records_num = mysql_num_rows(result);
				unsigned int fields_num = mysql_num_fields(result);

				res_ctx["fields_num"] = MiscTools::parse_type_to_string<unsigned int>(fields_num);
				res_ctx["records_num"] = MiscTools::parse_type_to_string<unsigned long>(records_num);

				MYSQL_FIELD* field;
				vector<string> fields_name;
				while(NULL != (field = mysql_fetch_field(result)))
				{
					fields_name.push_back(field->name);
				}

				for(int i = 0; i < records_num; i++)
				{
					MYSQL_ROW row = mysql_fetch_row(result);
					for(int j = 0; j < fields_num; j++)
					{
						res_ctx[fields_name[j] + "_" + MiscTools::parse_type_to_string<int>(i)] = row[j];
					}
				}
				
				mysql_free_result(result);

				res_ctx["success"] = "true";

				return SS_NoErr;
			}
		}
	}

	res_ctx["success"] = "false";
	res_ctx["fields_num"] = "0";
	res_ctx["records_num"] = "0";

	return SS_QueryFail;

}

std::string CMySQLProxy::get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name)
{
	return "";
}

SS_Error CMySQLProxy::get_field_name(const std::string& table_name, std::vector<std::string>& field_name)
{
	return SS_NoErr;
}

SS_Error CMySQLProxy::get_key_name(const std::string& table_name, std::vector<std::string>& key_name)
{
	return SS_NoErr;
}
