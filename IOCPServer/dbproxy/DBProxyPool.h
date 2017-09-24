#ifndef _DBPROXYPOOL_H_        
#define _DBPROXYPOOL_H_

#include "DBProxyI.h"
#include "DBLocker.h"
#include <list>

namespace Common
{
    //
    // ���ݿ����ӳ�
    //
    class db_proxy_pool
    {
    public:
        db_proxy_pool(void);
        ~db_proxy_pool(void);
        
        // ��ʼ�����ݿ�����ӿ�
        void init(const db_connection_t& db_connection);

        db_proxy_interface* get();
        void release(db_proxy_interface* ptr);
        void clear(void);

    private:
        db_mutex _mutex;
        db_connection_t _db_connection;
       // db_logger_ptr _logger_ptr;
        std::list<db_proxy_interface*> _db_proxy_ptr_list;
    };
}

#endif  // _DBPROXYPOOL_H_     
