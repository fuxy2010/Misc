#include "stdafx.h"
#include "DBProxy.h"
#include "DBProxyPool.h"

using namespace Common;

db_proxy::db_proxy(void)
{
	_pool_ptr = new db_proxy_pool();
}

db_proxy::~db_proxy(void)
{
	delete _pool_ptr;
}

// �������ݿ������־�ӿ�
void db_proxy::init(const db_connection_t& db_connection)
{
	_pool_ptr->init(db_connection);
}

db_retcode_t db_proxy::connect()
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	_pool_ptr->release(db_proxy_impl_ptr);
	return result;

}

// ִ�����ݿ����(�����ڴ�����ɾ�������롢���¡�ɾ����¼�Ȳ���)
db_retcode_t db_proxy::exec_dml(const std::string& sql)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_dml(sql);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

db_retcode_t db_proxy::exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_dml_ex(sql, db_data_list);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}


// ִ�����ݿ����(�����ڲ��롢���¡�ɾ�����������ݵȲ���)
db_retcode_t db_proxy::exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_dml_blob(sql, db_blob_data);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// ִ�����ݿ��ѯ����ȡ����
db_retcode_t db_proxy::exec_scalar(const std::string& sql, db_data_t& db_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_scalar(sql, db_data);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// ִ�����ݿ��ѯ����ȡ����������
db_retcode_t db_proxy::exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_scalar_blob(sql, db_blob_data);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

//===========================================================================================================================by sunyc
//ִ�����ݿ��ѯ����ֶ�����
db_retcode_t db_proxy::exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_get_field_attr(sql, db_field_attr_list);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}
//====================================================================================================================================================

// ִ�����ݿ��ѯ����ȡ���ݼ�
db_retcode_t db_proxy::exec_select(const std::string& sql, db_recordset_t& db_recordset)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_select(sql, db_recordset);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// ִ�����ݿ��ѯ����ȡ���ݼ�
db_retcode_t db_proxy::exec_select_ex(const std::string& sql, const db_data_list_t& db_data_list, db_recordset_t& db_recordset)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// �����ݿ����ػ�ȡ���ݿ����
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// �������ݿ�
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// ����ʧ�ܴ���
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// ִ�����ݿ����
	result = db_proxy_impl_ptr->exec_select_ex(sql, db_data_list, db_recordset);

	// ���ݿ����ػ������ݿ����
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}
