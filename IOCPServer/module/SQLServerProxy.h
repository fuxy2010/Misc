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
        //�������ݿ�
        virtual SS_Error start();

        //�ر����ݿ�
        virtual SS_Error shutdown();
        
        //���ݿ�����
    	virtual SS_Error set_connection_info(const DB_SERVER_INFO& info);

		//ִ��SQL���
		virtual SS_Error execute_sql(const std::string sql_str, SS_CTX& res_ctx);
        	
        //��ѯ��¼
        virtual SS_Error query_record(const std::string sql_str, SS_CTX& res_ctx);

		//����ӿ�
		//�������ݱ�����ȡ����Ϣ�����ɴ����ñ��sql���
		/*==================================================================================================================================
		[source_table_name]:��Ҫ���Ƶı���
		[target_table_name]:���ɵ��±����
		�������óɹ������ش��������䣬����ʧ�ܣ����ؿ�ֵ
		ע�⣬�˺�����֧�ֵı����������:
		1.�ֶ��������ͽ�֧�֣�int, datetime, varchar(size), float
		2.���ֶ��Ƿ�Ϊ������Ч
		4.���ֶ��Ƿ��Ϊ����Ч
		5.���ֶ��Ƿ�Ϊ������Ϊ1��������Ч
		==================================================================================================================================*/
		virtual std::string get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name);

		//���ݱ�������ֶ������б�
		virtual SS_Error get_field_name(const std::string& table_name, std::vector<std::string>& field_name);

		//���ݱ�����������ֶ������б�
		virtual SS_Error get_key_name(const std::string& table_name, std::vector<std::string>& key_name);

	private:
		db_connection_t _db_connection_info;

		db_proxy _db_proxy;

    };
	
}

#endif  // _SQLSERVER_PROXY_H_       
