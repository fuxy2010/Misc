// **********************************************************************
// ����: ������
// �汾: 1.0
// ����: 2011-01 ~ 2011-11
// �޸���ʷ��¼: 
// ����, ����, �������
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
		//�������ݿ�
		virtual SS_Error start() = 0;
		
		//�ر����ݿ�
		virtual SS_Error shutdown() = 0;

		//���ݿ�����
		virtual SS_Error set_connection_info(const DB_SERVER_INFO& info) = 0;

        //ִ��SQL���
        virtual SS_Error execute_sql(const std::string sql_str, SS_CTX& res_ctx) = 0;
        	
        //��ѯ��¼
        virtual SS_Error query_record(const std::string sql_str, SS_CTX& res_ctx) = 0;

		//����ӿ�
		//�������ݱ�����ȡ����Ϣ�����ɴ����ñ��sql���
		virtual std::string get_create_table_sql(const std::string& source_table_name, const std::string& target_table_name) = 0;

		//���ݱ�������ֶ������б�
		virtual SS_Error get_field_name(const std::string& table_name, std::vector<std::string>& field_name) = 0;

		//���ݱ�����������ֶ������б�
		virtual SS_Error get_key_name(const std::string& table_name, std::vector<std::string>& key_name) = 0;
    };
}

#endif // _I_DB_PROXY_H_