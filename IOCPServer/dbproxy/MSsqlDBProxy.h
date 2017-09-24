#ifndef _MSSQL_DBPROXY_H_        
#define _MSSQL_DBPROXY_H_

//
////���Window7 SP1 ADO�������� #import "C:\Program Files\Common Files\System\ADO\msado15.dll" named_guids rename("EOF","adoEOF"), rename("BOF","adoBOF")
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
	

		// �������ݿ�
		virtual db_retcode_t connect(void);

		// ִ�����ݿ����(�����ڴ�����ɾ�������롢���¡�ɾ����¼�Ȳ���)
		virtual db_retcode_t exec_dml(const std::string& sql);
		virtual db_retcode_t exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list);

		// ִ�����ݿ����(�����ڲ��롢���¡�ɾ�����������ݵȲ���)
		virtual db_retcode_t exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data);

		// ִ�����ݿ��ѯ����ȡ����
		virtual db_retcode_t exec_scalar(const std::string& sql, db_data_t& db_data);

		// ִ�����ݿ��ѯ����ȡ����������
		virtual db_retcode_t exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data);

		//ִ�����ݿ��ѯ����ֶ�����
		virtual db_retcode_t exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list);

		// ִ�����ݿ��ѯ����ȡ���ݼ�
		virtual db_retcode_t exec_select(const std::string& sql, db_recordset_t& db_recordset);

		// �Ͽ����ݿ�����
		virtual db_retcode_t disconect(void);

		// ��ȡ��ǰ��¼ָ����ָ���ֶΣ��У�����
	private:
		db_retcode_t get_db_data(long row_index, int column_index, db_data_t& db_data);

		void deal_net_disconnect(const std::string& error_description); //������ߴ�����

	private:
		_ConnectionPtr m_pConnection;	
		_RecordsetPtr  m_pRecordset;
		

	};
}

#endif  // _MSSQL_DBPROXY_H_     
