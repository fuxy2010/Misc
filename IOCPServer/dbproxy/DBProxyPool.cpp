#include "stdafx.h"
#include "DBProxyPool.h"
//#ifdef SUPPORT_DB_MYSQL
//#include "MysqlDBProxy.h"
//#endif
//
//#ifdef SUPPORT_DB_ORACLE
//#include "OracleDBProxy.h"
//#endif
//
//#ifdef SUPPORT_DB_SQLITE3
//#include "Sqlite3DBProxy.h"
//#endif

//#ifdef SUPPORT_DB_MSSQL
#include "MSsqlDBProxy.h"
//#endif

using namespace std;
using namespace Common;

db_proxy_pool::db_proxy_pool(void)
{
}

db_proxy_pool::~db_proxy_pool(void)
{
    clear();
}

// 初始化数据库操作接口
void db_proxy_pool::init(const db_connection_t& db_connection)
{
    _db_connection = db_connection;
   // _logger_ptr = logger_ptr;
}

db_proxy_interface* db_proxy_pool::get()
{
    db_locker lock(&_mutex);

    db_proxy_interface * db_proxy_impl_ptr = NULL;
    if (_db_proxy_ptr_list.size() > 0)
    {
        db_proxy_impl_ptr = _db_proxy_ptr_list.front();
        _db_proxy_ptr_list.pop_front();
    }
    else
    {
        switch(_db_connection.db_type)
        {
		case db_type_mssql:
//#ifdef SUPPORT_DB_MSSQL
			db_proxy_impl_ptr = new mssql_dbproxy();
//#endif
//        case db_type_mysql:
//#ifdef SUPPORT_DB_MYSQL
//            db_proxy_impl_ptr = new mysql_dbproxy();
//#endif
//            break;
//
//        case db_type_oracle:
//#ifdef SUPPORT_DB_ORACLE
//            db_proxy_impl_ptr = new oracle_dbproxy();
//#endif
//            break;
//
//        case db_type_sqlite3:
//#ifdef SUPPORT_DB_SQLITE3
//            db_proxy_impl_ptr = new sqlite3_dbproxy();
//
//#endif
           // break;

        default:
            // Do nothing.
            break;
        }

        if (db_proxy_impl_ptr)
        {
            db_proxy_impl_ptr->init(_db_connection);
        }
    }

    return db_proxy_impl_ptr;
}

void db_proxy_pool::release(db_proxy_interface* ptr)
{
    db_locker lock(&_mutex);

    if (ptr->is_connected())
    {
        _db_proxy_ptr_list.push_back(ptr);
    }
    else
    {
        ptr->disconect();
        delete ptr;
    }
}

void db_proxy_pool::clear(void)
{
    db_locker lock(&_mutex);

    for (std::list<db_proxy_interface*>::iterator iter = _db_proxy_ptr_list.begin();
         iter != _db_proxy_ptr_list.end(); iter++)
    {
        (*iter)->disconect();
        delete *iter;
    }

    _db_proxy_ptr_list.clear();
}
