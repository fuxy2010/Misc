// **********************************************************************
// Copyright (C) 2008-2009, ���Ҷ�ý��������̼����о����ģ��人��ѧ��
// �ļ���: DBLocker.h
// ����: chenqg    ����: chenqg@multimedia.whu.edu.cn
// �汾: 1.0
// ����: 2009-10-30
// ����: ���ݿ����������
// �޸���ʷ��¼: 
//   ����, ����, �������
// **********************************************************************

#ifndef _DBLOCKER_H_
#define _DBLOCKER_H_

#ifdef _WIN32 // Windows version
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // Linux version
#include <pthread.h>
#endif

namespace Common
{
    class db_mutex
    {
    public:
        db_mutex()
        {
#ifdef _WIN32 // Windows version
            InitializeCriticalSection(&_mutex);
#else // Linux version
            pthread_mutex_init(&_mutex, NULL); 
#endif
        };

        ~db_mutex()
        {
#ifdef _WIN32 // Windows version
            DeleteCriticalSection(&_mutex);
#else // Linux version
            pthread_mutex_destroy(&_mutex); 
#endif
        };

        void lock() const
        {
#ifdef _WIN32 // Windows version
            EnterCriticalSection(&_mutex);
#else // Linux version
            pthread_mutex_lock(&_mutex); 
#endif
        };

        void unlock() const
        {
#ifdef _WIN32 // Windows version
            LeaveCriticalSection(&_mutex);
#else // Linux version
            pthread_mutex_unlock(&_mutex); 
#endif
        };

    private:
#ifdef _WIN32 // Windows version
        mutable CRITICAL_SECTION _mutex;
#else // Linux version
        pthread_mutex_t _mutex;
#endif
    };

    class db_locker
    {
    public:
        db_locker(db_mutex * mutex_ptr)
        {
            _mutex_ptr = mutex_ptr;
            if (_mutex_ptr)
            {
                _mutex_ptr->lock();
            }
        };

        ~db_locker(void)
        {
            if (_mutex_ptr)
            {
                _mutex_ptr->unlock();
            }
        };

    private:
        db_locker(const db_locker&);
        db_locker& operator=(const db_locker&);

    private:
        db_mutex * _mutex_ptr;
    };
}

#endif // _DBLOCKER_H_
