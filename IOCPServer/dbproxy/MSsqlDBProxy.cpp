#include "stdafx.h"
#include "ScheduleServer.h"//just for log
#include "MSsqlDBProxy.h"

using namespace std;
using namespace Common;
using namespace ScheduleServer;//just for log


mssql_dbproxy::mssql_dbproxy(void):
m_pConnection(NULL),
m_pRecordset(NULL)
{
	//::CoInitialize(NULL);

}
mssql_dbproxy::~mssql_dbproxy()
{
	if (_connected)
	{
		disconect();		
	}
	//::CoUninitialize();
}

//掉线处理函数
void mssql_dbproxy::deal_net_disconnect(const std::string& error_description)
{
	string net_read_error = "[DBNETLIB][ConnectionRead (recv()).]一般性网络错误。请检查网络文档。";
	string net_write_error = "[DBNETLIB][ConnectionWrite (send()).]一般性网络错误。请检查网络文档。";
	string connect_fail = "连接失败";

	if((net_read_error == error_description) || (net_write_error == error_description)|| (connect_fail == error_description))//网络中断则重连	
	{
		_connected = false;

		if(NULL != m_pConnection)
		{
			if(m_pConnection->State)
				m_pConnection->Close();
		}
	}


}
// 连接数据库
db_retcode_t mssql_dbproxy::connect(void)
{
	if(FAILED(::CoInitialize(NULL)))//连接前必须先初始化COM组件，因为如果断线后，再次恢复时，会进行重连，此时断线前初始化的COM组件可能已经失效，必须重新初始化
		return db_retcode_fail;

	try
	{
		if (FAILED(m_pConnection.CreateInstance("ADODB.Connection")))
		{
			_connected = false;
			return db_retcode_fail;
		}

		stringstream stre;
		stre << "Provider=SQLOLEDB.1; " << "Data Source=" << _db_connection.db_server_addr << "," << _db_connection.db_server_port 
			<< "; "<< "Initial Catalog=" << _db_connection.db_name << "; " << "User ID=" << _db_connection.db_user 
			<< "; " << "Password=" << _db_connection.db_password;
		_bstr_t connectionstr = stre.str().c_str();

		if (FAILED(m_pConnection->Open(connectionstr,"", "", NULL)))
		{
			_connected = false;
			::CoUninitialize();
			return db_retcode_fail;
		}

		_connected = true;
		::CoUninitialize();

		return db_retcode_ok;		
	}

	catch(_com_error e)
	{
		if(NULL != m_pConnection)
		{
			if(m_pConnection->State)
				m_pConnection->Close();
		}
				
		//LOG_WRITE("<EXE SQL> " << e.Description() << " fail!", 1, true);
		::CoUninitialize();
		_connected = false;

		return db_retcode_fail;
	}

}


// 执行数据库操作(适用于创建、删除表或插入、更新、删除记录等操作)
db_retcode_t mssql_dbproxy::exec_dml(const std::string& sql)
{

	if (!_connected ) return db_retcode_connection_fail;
	try
	{
		//if (!_connected ) return db_retcode_connection_fail;

		_bstr_t bstr = sql.c_str();

		if (FAILED(m_pConnection->Execute(bstr, NULL, adCmdText)))
		{
			return db_retcode_fail;
		}

		return db_retcode_ok;

	}

	catch(_com_error e)
	{	
		string error_description =  MiscTools::parse_type_to_string<_bstr_t>(e.Description());
		
		deal_net_disconnect(error_description);

		//LOG_WRITE("<EXE SQL> " << error_description << " fail!", 1, true);
		return db_retcode_fail;
	}

}

//获取记录中指定行指定字段（列）的数据
db_retcode_t mssql_dbproxy::get_db_data(long row_index, int column_index, db_data_t& db_data)
{


	long rows = m_pRecordset->RecordCount;//获得记录集中记录行数
	int columns = m_pRecordset->Fields->GetCount();//获取记录集中列数
	int data_type = 0;

	_variant_t result = NULL;
	result.vt =VT_BSTR;

	if ((rows > 0) && (row_index <= rows) && (column_index <= columns))
	{

		_variant_t index = NULL;
		index.vt = VT_I2;
		index.iVal = column_index;
		data_type = m_pRecordset->Fields->GetItem(index)->GetType();//获取字段数据类型

		_variant_t rowsvalue = m_pRecordset->GetRows(rows);
		long lo[2] = {0,0};
		lo[0] = column_index;
		lo[1] = row_index;
		/*_variant_t result = NULL;
		result.vt =VT_BSTR;*/
		SafeArrayGetElement(rowsvalue.parray,lo,&result);


		if (result.vt != VT_NULL)//如果字段值为NULL，用stringstream转换会出错
		{
			stringstream datastream;
			switch (data_type)
			{
			case adVarChar:
				{
					//stringstream datastream;	
					datastream << (_bstr_t)result;
					db_data.data = datastream.str();
					db_data.type = db_data_type_string;
					break;
				}
			case adDouble:
				{
					//stringstream datastream;
					datastream << (_bstr_t)result;
					db_data.data = datastream.str();
					db_data.type = db_data_type_double;
					break;
				}

			case adInteger:
				{
					//stringstream datastream;
					datastream << (_bstr_t)result;
					db_data.data = datastream.str();
					db_data.type = db_data_type_int;
					break;
				}

			case adDBTimeStamp:
				{
					//stringstream datastream;
					datastream << (_bstr_t)result;
					db_data.data = datastream.str();
					db_data.type = db_data_type_datetime;
					break;
				}

			default:
				{
					//stringstream datastream;
					datastream << (_bstr_t)result;
					db_data.data = datastream.str();
					db_data.type = db_data_type_unknown;

				}

			}	
		}

		else
		{
			db_data.data = "";//"NULL";
			db_data.type = db_data_type_unknown;
		}
		return db_retcode_ok;
	}

	//LOG_WRITE("没有符合查询条件的记录或者指定索引超出上限", 1, false);
	return db_retcode_fail;
	

}

db_retcode_t mssql_dbproxy::exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list){return db_retcode_ok;}

// 执行数据库操作(适用于插入、更新、删除二进制数据等操作)
 db_retcode_t mssql_dbproxy::exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data){return db_retcode_ok;}

// 执行数据库查询并获取数据
 db_retcode_t mssql_dbproxy::exec_scalar(const std::string& sql, db_data_t& db_data)
 {//这里要注意使用recordset 的open方法
	 if (!_connected ) return db_retcode_connection_fail;
	 try
	 {
		 if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
		 {

			 return db_retcode_fail;
		 }

		 _bstr_t bstr = sql.c_str();//要把string型数据转换下
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }
		 
		 db_retcode_t result = get_db_data(0,0,db_data);//获取记录中第一条第一个字段的值
		 m_pRecordset->Close();

		 return result;

	 }

	 catch(_com_error e)
	 {
		 if(NULL != m_pRecordset)
		 {
			 if(m_pRecordset->State)
				 m_pRecordset->Close();
		 }

		 string error_description =  MiscTools::parse_type_to_string<_bstr_t>(e.Description());

		 deal_net_disconnect(error_description);

		 //LOG_WRITE("<EXE SQL> " << error_description << " fail!", 1, true);
		 return db_retcode_fail;
	 }
	 
 }

// 执行数据库查询并获取二进制数据
 db_retcode_t mssql_dbproxy::exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data){return db_retcode_ok;}

 //执行数据库查询获得字段属性
 db_retcode_t mssql_dbproxy::exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list)
 {
	 if (!_connected ) return db_retcode_connection_fail;

	 try
	 {
		 db_field_attr_list.clear();

		 if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
		 {
			 return db_retcode_fail;
		 }

		 _bstr_t bstr = sql.c_str();//要把string型数据转换下
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }

		 int cloumns = m_pRecordset->Fields->GetCount();
		 long rows = m_pRecordset->RecordCount;

		 if (rows >= 0)
		 {
			 _variant_t index = NULL;
			 index.vt = VT_I2; //short整型 

			 db_field_attr_t db_field_attr;
			 for (int i = 0; i < cloumns; i++)//获取所有字段属性信息
			 {
				 index.iVal = i;		
				// db_field_attr_t db_field_attr;
				 db_field_attr.name = m_pRecordset->Fields->GetItem(index)->GetName();						//获取字段名
				 db_field_attr.defined_size = m_pRecordset->Fields->GetItem(index)->DefinedSize;			//获取字段定义大小
				 db_field_attr.type = m_pRecordset->Fields->GetItem(index)->Type;							//获取字段类型
				 db_field_attr.field_atrr = m_pRecordset->Fields->GetItem(index)->Attributes;				//获取字段其余属性，如是否为主键，是否可为空

				 db_field_attr_list.push_back(db_field_attr);

			 }

			 m_pRecordset->Close();
			 return db_retcode_ok;
		 }

		 //LOG_WRITE("查询出错", 1, false);
		 m_pRecordset->Close();
		 return db_retcode_fail;

	 }

	 catch(_com_error e)
	 {	 

		 if(NULL != m_pRecordset)
		 {
			 if(m_pRecordset->State)
				 m_pRecordset->Close();
		 }

		 string error_description =  MiscTools::parse_type_to_string<_bstr_t>(e.Description());

		 deal_net_disconnect(error_description);

		 //LOG_WRITE("<EXE SQL> " << error_description << " fail!", 1, true);
		 return db_retcode_fail;

	 }

 }

// 执行数据库查询并获取数据集
 db_retcode_t mssql_dbproxy::exec_select(const std::string& sql, db_recordset_t& db_recordset)
 
 {	
	 if (!_connected ) return db_retcode_connection_fail;
	
	 try
	 {
		 db_recordset.field_list.clear();
		 db_recordset.record_list.clear();

		 if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
		 {
			 return db_retcode_fail;
		 }

		 _bstr_t bstr = sql.c_str();//要把string型数据转换下
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }

		 int cloumns = m_pRecordset->Fields->GetCount();
		 long rows = m_pRecordset->RecordCount;

		 if( rows > 0 )
		 {
			 _variant_t index = NULL;
			 index.vt = VT_I2; //short整型 

			 db_field_t db_field = "";
			 for (int i = 0; i < cloumns; i++)//获取所有字段信息
			 {
				 index.iVal=i;		
				 db_field = m_pRecordset->Fields->GetItem(index)->GetName();
				 db_recordset.field_list.push_back(db_field);
			 }

			 _variant_t result = NULL;
			 result.vt =VT_BSTR;
			 long lo[2] = {0,0};
			 _variant_t rowsvalue = m_pRecordset->GetRows(rows);

			 int db_data_type_temp = 0;
			 _variant_t index_temp = NULL;
			 index_temp.vt = VT_I2;


			 db_record_t db_record;//每条记录的信息，包括每个字段的值，是db_data_t的向量
			 db_data_t db_data;//记录字段值信息

			 for (long i = 0; i < rows; i++)
			 {
				 //db_record_t db_record;//每条记录的信息，包括每个字段的值，是db_data_t的向量
				 db_record.clear();
				 for (int j = 0; j < cloumns; j++)
				 {		

					 lo[0] = j;  //指定第几列
					 lo[1] = i;	//指定第几行	
					 SafeArrayGetElement(rowsvalue.parray,lo,&result);//取值,从rowsvalue中取

					// db_data_t db_data;//记录字段值信息

					 index_temp.iVal = j;
					 db_data_type_temp = m_pRecordset->Fields->GetItem(index_temp)->Type;//临时存储字段类型

					 if ( 200 == db_data_type_temp)//如果是varchar类型,则如果varchar类型字段的值是""，做转换，把""转换为" "
					 {
						 if(result.vt != VT_NULL)	//必须做判断，表中字段值如果为NULL，是无法用stringstream转换的			
						 {
							 stringstream datastream;
							 datastream << (_bstr_t)result;
							 db_data.data = datastream.str();

							 if ( "" == db_data.data)//判断""是否要转换为" "
								 db_data.data = " ";
						 }

						 else
							 db_data.data = "";//"NULL";					

					 }

					 else  if(result.vt != VT_NULL)	//必须做判断，表中字段值如果为NULL，是无法用stringstream转换的			
					 {
						 stringstream datastream;
						 datastream << (_bstr_t)result;
						 db_data.data = datastream.str();					 
					 }
					 else
						 db_data.data = "";//"NULL";


					 db_record.push_back(db_data);

				 }

				 db_recordset.record_list.push_back(db_record);//record_list是db_record_t的向量

			 }
			 m_pRecordset->Close();
			 return db_retcode_ok;
		 }
		
		 //LOG_WRITE("没有符合查询条件的记录", 1, false);
		 m_pRecordset->Close();
		 return db_retcode_fail;	
		
	 }

	 catch(_com_error e)
	 {	

		 if(NULL != m_pRecordset)
		 {
			 if(m_pRecordset->State)
				 m_pRecordset->Close();
		 }

		 string error_description =  MiscTools::parse_type_to_string<_bstr_t>(e.Description());

		 deal_net_disconnect(error_description);

		 //LOG_WRITE("<EXE SQL> " << error_description << " fail!", 1, true);
		 return db_retcode_fail;
	 }

	
 }

// 断开数据库连接
 db_retcode_t mssql_dbproxy::disconect(void)
 {
	 if (_connected)	
		 _connected = false;

	 if(NULL != m_pConnection)
	 {
		 if(m_pConnection->State)
			 m_pConnection->Close();
	 }
		 	
	 return db_retcode_ok;
 }


