#ifndef _MSSQL_DBPROXY_H_        
#define _MSSQL_DBPROXY_H_

//
////解决Window7 SP1 ADO兼容问题 #import "C:\Program Files\Common Files\System\ADO\msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")
#import "msado60_Backcompat_i386.tlb" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")//http://support.microsoft.com/kb/2517589

using namespace ADODB;
#include "DBProxyI.h"
#include <iostream>
#include <sstream>
#include "MiscTool.h"



namespace Common
{
	class mssql_dbproxy : public db_proxy_interface
	{
	public:
		mssql_dbproxy(void);
		~mssql_dbproxy(void);
	

		// 连接数据库
		virtual db_retcode_t connect(void);

		// 执行数据库操作(适用于创建、删除表或插入、更新、删除记录等操作)
		virtual db_retcode_t exec_dml(const std::string& sql);
		virtual db_retcode_t exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list);

		// 执行数据库操作(适用于插入、更新、删除二进制数据等操作)
		virtual db_retcode_t exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data);

		// 执行数据库查询并获取数据
		virtual db_retcode_t exec_scalar(const std::string& sql, db_data_t& db_data);

		// 执行数据库查询并获取二进制数据
		virtual db_retcode_t exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data);

		//执行数据库查询获得字段属性
		virtual db_retcode_t exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list);

		// 执行数据库查询并获取数据集
		virtual db_retcode_t exec_select(const std::string& sql, db_recordset_t& db_recordset);

		// 断开数据库连接
		virtual db_retcode_t disconect(void);

		// 获取当前记录指定行指定字段（列）数据
	private:
		db_retcode_t get_db_data(long row_index, int column_index, db_data_t& db_data);

		void deal_net_disconnect(const std::string& error_description); //网络掉线处理函数

	private:
		_ConnectionPtr m_pConnection;	
		_RecordsetPtr  m_pRecordset;
		

	};
}

#endif  // _MSSQL_DBPROXY_H_     
