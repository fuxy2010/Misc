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

// 设置数据库操作日志接口
void db_proxy::init(const db_connection_t& db_connection)
{
	_pool_ptr->init(db_connection);
}

db_retcode_t db_proxy::connect()
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	_pool_ptr->release(db_proxy_impl_ptr);
	return result;

}

// 执行数据库操作(适用于创建、删除表或插入、更新、删除记录等操作)
db_retcode_t db_proxy::exec_dml(const std::string& sql)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_dml(sql);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

db_retcode_t db_proxy::exec_dml_ex(const std::string& sql, const db_data_list_t& db_data_list)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_dml_ex(sql, db_data_list);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}


// 执行数据库操作(适用于插入、更新、删除二进制数据等操作)
db_retcode_t db_proxy::exec_dml_blob(const std::string& sql, const db_blob_data_t& db_blob_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_dml_blob(sql, db_blob_data);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// 执行数据库查询并获取数据
db_retcode_t db_proxy::exec_scalar(const std::string& sql, db_data_t& db_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_scalar(sql, db_data);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// 执行数据库查询并获取二进制数据
db_retcode_t db_proxy::exec_scalar_blob(const std::string& sql, db_blob_data_t& db_blob_data)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_scalar_blob(sql, db_blob_data);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

//===========================================================================================================================by sunyc
//执行数据库查询获得字段属性
db_retcode_t db_proxy::exec_get_field_attr(const std::string& sql, db_field_attr_list_t& db_field_attr_list)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_get_field_attr(sql, db_field_attr_list);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}
//====================================================================================================================================================

// 执行数据库查询并获取数据集
db_retcode_t db_proxy::exec_select(const std::string& sql, db_recordset_t& db_recordset)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_select(sql, db_recordset);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}

// 执行数据库查询并获取数据集
db_retcode_t db_proxy::exec_select_ex(const std::string& sql, const db_data_list_t& db_data_list, db_recordset_t& db_recordset)
{
	db_locker lock(&_mutex);
	db_retcode_t result = db_retcode_fail;

	// 从数据库代理池获取数据库代理
	db_proxy_interface* db_proxy_impl_ptr = _pool_ptr->get();
	if (!db_proxy_impl_ptr)
	{
		return result;
	}

	// 连接数据库
	if (!db_proxy_impl_ptr->is_connected())
	{
		result = db_proxy_impl_ptr->connect();
		if (db_retcode_ok != result)
		{
			// 连接失败处理
			_pool_ptr->release(db_proxy_impl_ptr);
			return result;
		}
	}

	// 执行数据库操作
	result = db_proxy_impl_ptr->exec_select_ex(sql, db_data_list, db_recordset);

	// 数据库代理池回收数据库代理
	_pool_ptr->release(db_proxy_impl_ptr);
	return result;
}
