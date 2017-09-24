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

//���ߴ�����
void mssql_dbproxy::deal_net_disconnect(const std::string& error_description)
{
	string net_read_error = "[DBNETLIB][ConnectionRead (recv()).]һ��������������������ĵ���";
	string net_write_error = "[DBNETLIB][ConnectionWrite (send()).]һ��������������������ĵ���";
	string connect_fail = "����ʧ��";

	if((net_read_error == error_description) || (net_write_error == error_description)|| (connect_fail == error_description))//�����ж�������	
	{
		_connected = false;

		if(NULL != m_pConnection)
		{
			if(m_pConnection->State)
				m_pConnection->Close();
		}
	}


}
// �������ݿ�
db_retcode_t mssql_dbproxy::connect(void)
{
	if(FAILED(::CoInitialize(NULL)))//����ǰ�����ȳ�ʼ��COM�������Ϊ������ߺ��ٴλָ�ʱ���������������ʱ����ǰ��ʼ����COM��������Ѿ�ʧЧ���������³�ʼ��
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


// ִ�����ݿ����(�����ڴ�����ɾ�������롢���¡�ɾ����¼�Ȳ���)
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

//��ȡ��¼��ָ����ָ���ֶΣ��У�������
db_retcode_t mssql_dbproxy::get_db_data(long row_index, int column_index, db_data_t& db_data)
{


	long rows = m_pRecordset->RecordCount;//��ü�¼���м�¼����
	int columns = m_pRecordset->Fields->GetCount();//��ȡ��¼��������
	int data_type = 0;

	_variant_t result = NULL;
	result.vt =VT_BSTR;

	if ((rows > 0) && (row_index <= rows) && (column_index <= columns))
	{

		_variant_t index = NULL;
		index.vt = VT_I2;
		index.iVal = column_index;
		data_type = m_pRecordset->Fields->GetItem(index)->GetType();//��ȡ�ֶ���������

		_variant_t rowsvalue = m_pRecordset->GetRows(rows);
		long lo[2] = {0,0};
		lo[0] = column_index;
		lo[1] = row_index;
		/*_variant_t result = NULL;
		result.vt =VT_BSTR;*/
		SafeArrayGetElement(rowsvalue.parray,lo,&result);


		if (result.vt != VT_NULL)//����ֶ�ֵΪNULL����stringstreamת�������
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

	//LOG_WRITE("û�з��ϲ�ѯ�����ļ�¼����ָ��������������", 1, false);
	return db_retcode_fail;
	

}

db_retcode_t mssql_dbproxy::exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list){return db_retcode_ok;}

// ִ�����ݿ����(�����ڲ��롢���¡�ɾ�����������ݵȲ���)
 db_retcode_t mssql_dbproxy::exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data){return db_retcode_ok;}

// ִ�����ݿ��ѯ����ȡ����
 db_retcode_t mssql_dbproxy::exec_scalar(const std::string& sql, db_data_t& db_data)
 {//����Ҫע��ʹ��recordset ��open����
	 if (!_connected ) return db_retcode_connection_fail;
	 try
	 {
		 if (FAILED(m_pRecordset.CreateInstance("ADODB.Recordset")))
		 {

			 return db_retcode_fail;
		 }

		 _bstr_t bstr = sql.c_str();//Ҫ��string������ת����
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }
		 
		 db_retcode_t result = get_db_data(0,0,db_data);//��ȡ��¼�е�һ����һ���ֶε�ֵ
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

// ִ�����ݿ��ѯ����ȡ����������
 db_retcode_t mssql_dbproxy::exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data){return db_retcode_ok;}

 //ִ�����ݿ��ѯ����ֶ�����
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

		 _bstr_t bstr = sql.c_str();//Ҫ��string������ת����
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }

		 int cloumns = m_pRecordset->Fields->GetCount();
		 long rows = m_pRecordset->RecordCount;

		 if (rows >= 0)
		 {
			 _variant_t index = NULL;
			 index.vt = VT_I2; //short���� 

			 db_field_attr_t db_field_attr;
			 for (int i = 0; i < cloumns; i++)//��ȡ�����ֶ�������Ϣ
			 {
				 index.iVal = i;		
				// db_field_attr_t db_field_attr;
				 db_field_attr.name = m_pRecordset->Fields->GetItem(index)->GetName();						//��ȡ�ֶ���
				 db_field_attr.defined_size = m_pRecordset->Fields->GetItem(index)->DefinedSize;			//��ȡ�ֶζ����С
				 db_field_attr.type = m_pRecordset->Fields->GetItem(index)->Type;							//��ȡ�ֶ�����
				 db_field_attr.field_atrr = m_pRecordset->Fields->GetItem(index)->Attributes;				//��ȡ�ֶ��������ԣ����Ƿ�Ϊ�������Ƿ��Ϊ��

				 db_field_attr_list.push_back(db_field_attr);

			 }

			 m_pRecordset->Close();
			 return db_retcode_ok;
		 }

		 //LOG_WRITE("��ѯ����", 1, false);
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

// ִ�����ݿ��ѯ����ȡ���ݼ�
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

		 _bstr_t bstr = sql.c_str();//Ҫ��string������ת����
		 if (FAILED(m_pRecordset->Open(bstr,_variant_t((IDispatch *)m_pConnection,true), adOpenStatic, adLockOptimistic, adCmdText)))
		 {
			 return db_retcode_fail;
		 }

		 int cloumns = m_pRecordset->Fields->GetCount();
		 long rows = m_pRecordset->RecordCount;

		 if( rows > 0 )
		 {
			 _variant_t index = NULL;
			 index.vt = VT_I2; //short���� 

			 db_field_t db_field = "";
			 for (int i = 0; i < cloumns; i++)//��ȡ�����ֶ���Ϣ
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


			 db_record_t db_record;//ÿ����¼����Ϣ������ÿ���ֶε�ֵ����db_data_t������
			 db_data_t db_data;//��¼�ֶ�ֵ��Ϣ

			 for (long i = 0; i < rows; i++)
			 {
				 //db_record_t db_record;//ÿ����¼����Ϣ������ÿ���ֶε�ֵ����db_data_t������
				 db_record.clear();
				 for (int j = 0; j < cloumns; j++)
				 {		

					 lo[0] = j;  //ָ���ڼ���
					 lo[1] = i;	//ָ���ڼ���	
					 SafeArrayGetElement(rowsvalue.parray,lo,&result);//ȡֵ,��rowsvalue��ȡ

					// db_data_t db_data;//��¼�ֶ�ֵ��Ϣ

					 index_temp.iVal = j;
					 db_data_type_temp = m_pRecordset->Fields->GetItem(index_temp)->Type;//��ʱ�洢�ֶ�����

					 if ( 200 == db_data_type_temp)//�����varchar����,�����varchar�����ֶε�ֵ��""����ת������""ת��Ϊ" "
					 {
						 if(result.vt != VT_NULL)	//�������жϣ������ֶ�ֵ���ΪNULL�����޷���stringstreamת����			
						 {
							 stringstream datastream;
							 datastream << (_bstr_t)result;
							 db_data.data = datastream.str();

							 if ( "" == db_data.data)//�ж�""�Ƿ�Ҫת��Ϊ" "
								 db_data.data = " ";
						 }

						 else
							 db_data.data = "";//"NULL";					

					 }

					 else  if(result.vt != VT_NULL)	//�������жϣ������ֶ�ֵ���ΪNULL�����޷���stringstreamת����			
					 {
						 stringstream datastream;
						 datastream << (_bstr_t)result;
						 db_data.data = datastream.str();					 
					 }
					 else
						 db_data.data = "";//"NULL";


					 db_record.push_back(db_data);

				 }

				 db_recordset.record_list.push_back(db_record);//record_list��db_record_t������

			 }
			 m_pRecordset->Close();
			 return db_retcode_ok;
		 }
		
		 //LOG_WRITE("û�з��ϲ�ѯ�����ļ�¼", 1, false);
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

// �Ͽ����ݿ�����
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


