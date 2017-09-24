#include "stdafx.h"//just for log
#include "ScheduleServer.h"//just for log
#include "GeneralDef.h"
#include "SQLServerProxy.h"
#include <sstream>
#include <ObjBase.h>
using namespace ScheduleServer;

CSQLServerProxy::CSQLServerProxy()
{
}

CSQLServerProxy::~CSQLServerProxy()
{
}

SS_Error CSQLServerProxy::start()
{
	if(FAILED(::CoInitialize(NULL)))
		return SS_InvalidDBConnection;

	if (db_retcode_ok != _db_proxy.connect())
		return SS_UserNameOrPasswdError;

	get_module_description().available = true;

	return SS_NoErr;
}

SS_Error CSQLServerProxy::shutdown()
{
	get_module_description().available = false;

	::CoUninitialize();

	return SS_NoErr;
}

SS_Error CSQLServerProxy::set_connection_info(const DB_SERVER_INFO& info)
{
	_db_connection_info.db_type = db_type_mssql;
	_db_connection_info.db_server_addr = info.server_ip;
	_db_connection_info.db_server_port = info.server_port;
	_db_connection_info.db_name = info.db_name;
	_db_connection_info.db_user = info.db_username;
	_db_connection_info.db_password = info.db_password;

	_db_proxy.init(_db_connection_info);

	return SS_NoErr;
}

SS_Error CSQLServerProxy::execute_sql(const std::string sql_str, SS_CTX& res_ctx)
{
	//LOG_WRITE("<EXE SQL> " + sql_str, 1, false);

	res_ctx.clear();

	res_ctx["success"] = "false";

	try
	{
		if (db_retcode_ok != _db_proxy.exec_dml(sql_str))
		{
			//LOG_WRITE("<EXE SQL FAIL> " + sql_str, 1, false);

			return SS_ExecFail;
		}

		res_ctx["success"] = "true";
	}
	catch(...)
	{
		//LOG_WRITE("exception on <EXE SQL> " + sql_str, 1, false);
		return SS_ExecFail;
	}

	return SS_NoErr;	
}

SS_Error CSQLServerProxy::query_record(const std::string sql_str, SS_CTX& res_ctx)
{
	//LOG_WRITE("<QUERY SQL> " + sql_str, 1, false);

	res_ctx.clear();

	res_ctx["success"] = "false";

	try
	{
		db_recordset_t db_recordset;
		vector<string> fieldsname;
		stringstream fields_num;
		stringstream records_num;

		if (db_retcode_ok == _db_proxy.exec_select(sql_str,db_recordset))
		{
			for (unsigned int k = 0; k<db_recordset.field_list.size();k++)
			{
				fieldsname.push_back(db_recordset.field_list[k]);//存储字段名

			}

			fields_num << fieldsname.size();
			records_num << db_recordset.record_list.size();

			res_ctx["fields_num"] = fields_num.str();
			res_ctx["records_num"] = records_num.str();

			for (unsigned long i= 0; i<db_recordset.record_list.size();i++)
			{
				stringstream index;		
				index << i;

				for (unsigned int j = 0; j<db_recordset.record_list[i].size();j++)
				{
					res_ctx[fieldsname[j] + "_" + index.str()] = db_recordset.record_list[i][j].data;
				}
			}

			res_ctx["success"] = "true";

			return SS_NoErr;
		}
	}
	catch(...)
	{
		//LOG_WRITE("exception on <QUERY SQL> " + sql_str, 1, false);
	}

	res_ctx["success"] = "false";
	res_ctx["fields_num"] = "0";
	res_ctx["records_num"] = "0";

	return SS_QueryFail;

}

std::string CSQLServerProxy::get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name)
{

	db_field_attr_list_t db_field_attr_list;
	string sql = "select top 1 * from " + source_table_name;

	string sql_return = "if object_id('[" + target_table_name +"]') is null create table [" + target_table_name + "] ";

	string field_name = "";
	string field_type_str = "";
	string field_defined_size_str = "";
	string field_attr_str = "";

	int field_type = 0;
	int field_defined_size = 0;
	int field_attr = 0;

	if (db_retcode_ok == _db_proxy.exec_get_field_attr(sql, db_field_attr_list))
	{
		int field_max_index = db_field_attr_list.size()-1;

		for ( int i = 0; i < db_field_attr_list.size(); i++)
		{

			field_name = db_field_attr_list[i].name;							//获取字段名字(string型)
			field_type = db_field_attr_list[i].type;
			field_defined_size = db_field_attr_list[i].defined_size;
			field_attr = db_field_attr_list[i].field_atrr;

			//获取字段类型(string型)
			switch(field_type)
			{
			case 3:
				field_type_str = "int";
				break;
			case 200:
				field_type_str = "varchar";
				break;
			case 135:
				field_type_str = "datetime";
				break;
			case 5:
				field_type_str = "float";
				break;
			default:
				field_type_str = "varchar(100)";

			}

			//获取字段定义大小(string型)
			if ("varchar" == field_type_str)				
				field_defined_size_str = MiscTools::parse_type_to_string<int>(field_defined_size);		

			else
				field_defined_size_str = "";

			//获取字段其余属性(string)
			switch(field_attr)
			{
			case 32784:
				field_attr_str = " identity(1,1)";
				break;
			case 16:
				field_attr_str = " identity(1,1)";
				break;
			case 8:
				field_attr_str = " not null";
				break;
			case 24:
				field_attr_str = " not null";
				break;
			default:
				field_attr_str = "";

			}


			if(!i)					//第一个字段
			{
				if ("varchar" == field_type_str)	//如果varchar型，要指定大小
				{
					sql_return += "([";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += "varchar(";
					sql_return += field_defined_size_str;
					sql_return += ")";
					sql_return += field_attr_str;
					sql_return += ",";
				}
				else
				{
					sql_return += "([";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += field_type_str;					
					sql_return += field_attr_str;
					sql_return += ",";

				}
			}

			else if( field_max_index == i)					//最后一个字段
			{
				if ("varchar" == field_type_str)	//如果varchar型，要指定大小
				{
					sql_return += "[";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += "varchar(";
					sql_return += field_defined_size_str;
					sql_return += ")";
					sql_return += field_attr_str;
					//sql_return += ")";
				}
				else
				{
					sql_return += "[";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += field_type_str;					
					sql_return += field_attr_str;
					//sql_return += ")";

				}
			}

			else								//中间字段
			{
				if ("varchar" == field_type_str)	//如果varchar型，要指定大小
				{
					sql_return += "[";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += "varchar(";
					sql_return += field_defined_size_str;
					sql_return += ")";
					sql_return += field_attr_str;
					sql_return += ",";
				}
				else
				{
					sql_return += "[";
					sql_return += field_name;
					sql_return += "] ";
					sql_return += field_type_str;
					sql_return += field_attr_str;
					sql_return += ",";

				}
			}

		}



		std::vector<std::string> key_name;			//取得主键名字
		if( SS_NoErr == get_key_name(source_table_name, key_name))
		{
			int key_max_index = key_name.size() - 1;

			if( true == key_name.empty())				//如果没主键，SQL语句以")"结束
				sql_return += ")";
			else
			{
				sql_return += ",primary key(";
				for (int j = 0; j < key_name.size(); j++)
				{
					if ( key_max_index != j)			//非最后一个主键
					{					
						sql_return += key_name[j];
						sql_return += ",";
					}

					else								//最后一个主键
					{
						sql_return += key_name[j];
						sql_return += "))";
					}
				}
			}
		}

		else
		{
			sql_return += ")";
		}

		return sql_return;
	}
	return "";
}

SS_Error CSQLServerProxy::get_field_name(const std::string& table_name, std::vector<std::string>& field_name)
{
	field_name.clear();

	db_field_attr_list_t db_field_attr_list;
	string sql = "select top 1 * from " + table_name;
	
	if (db_retcode_ok == _db_proxy.exec_get_field_attr(sql, db_field_attr_list))
	{
		for (unsigned int i = 0; i < db_field_attr_list.size(); i++)
		{
			field_name.push_back(db_field_attr_list[i].name);			
		}

		return SS_NoErr;

	}

	return SS_QueryFail;
}

SS_Error CSQLServerProxy::get_key_name(const std::string& table_name, std::vector<std::string>& key_name)
{
	key_name.clear();

	db_field_attr_list_t db_field_attr_list;
	string sql = "select top 1 * from " + table_name;

	int field_attr = 0;

	if (db_retcode_ok == _db_proxy.exec_get_field_attr(sql, db_field_attr_list))
	{
		for ( int i = 0; i < db_field_attr_list.size(); i++ )
		{
			field_attr = db_field_attr_list[i].field_atrr;
			switch(field_attr)
			{
				case 32776:
				case 32792:
				case 32784:
					key_name.push_back(db_field_attr_list[i].name);
					break;
			}

		}

		return SS_NoErr;
	}

	return SS_QueryFail;
}
