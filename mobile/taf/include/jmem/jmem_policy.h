#ifndef _JMEM_POLICY_H
#define _JMEM_POLICY_H

#include <fcntl.h>
#include <algorithm>
#include "util/tc_file.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_sem_mutex.h"
#include "util/tc_shm.h"
#include "util/tc_mmap.h"

namespace taf
{
//////////////////////////////////////////////////////////////////////
// �洢����: �ڴ�, �����ڴ�, mmap(�ļ�)

/**
 * �ڴ�洢
 */
template<typename T, typename LockPolicy>
class MemStorePolicy : public LockPolicy
{
public:
    /**
    * ��ʼ��
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    */
    void create(void *pAddr, size_t iSize)
    {
        _t.create(pAddr,iSize);
    }

    /**
    * �����϶���
    * @param pAddr: ָ����пռ��ָ��
    * @param iSize: �ռ��ָ��
    */
    void connect(void *pAddr, size_t iSize)
    {
        _t.connect(pAddr,iSize);
    }

protected:
    T   _t;
};

/**
 * �����ڴ�洢
 */
template<typename T, typename LockPolicy>
class ShmStorePolicy : public LockPolicy
{
public:
    /**
     * ��ʼ������洢
     * @param iShmKey
     * @param iSize
     */
    void initStore(key_t iShmKey, size_t iSize)
    {
        _shm.init(iSize, iShmKey);
        if(_shm.iscreate())
        {
            _t.create(_shm.getPointer(), iSize);
        }
        else
        {
            _t.connect(_shm.getPointer(), iSize);
        }
    }

    /**
     * �ͷŹ����ڴ�
     */
    void release()
    {
        _shm.del();
    }
protected:
    TC_Shm  _shm;
    T       _t;
};

template<typename T, typename LockPolicy>
class ShmFileStorePolicy : public LockPolicy
{
    class ShmStore
    {
    public:
        int shmopen( const char* file, size_t size )
        {
            int fd = -1;
            if ( ( fd = shm_open( file, O_RDWR | O_CREAT | O_EXCL, 0644 ) ) >= 0 )
            {
                ftruncate( fd, size );
                _bCreate = true;
            }
            else if ( errno == EEXIST )
            {
                if ( ( fd = shm_open( file, O_RDWR | O_CREAT, 0644 ) ) >= 0 )
                {
                    ftruncate( fd, size );
                    _bCreate = false;
                }
                else
                {
                    throw TC_Mmap_Exception( "[TC_Mmap::mmap] fopen shmfile '" + string( file ) + "' error", errno );
                }
            }
            else
            {
                throw TC_Mmap_Exception( "[TC_Mmap::mmap] fopen shmfile '" + string( file ) + "' error", errno );
            }
            return fd;
        }
        void shmfree( int fd )
        {
            close( fd );
        }
        void shmclose( const char* file )
        {
            shm_unlink( file );
        }

        /* 
         * @brief �Ƿ񴴽������ģ��ļ��Ĵ��ڿɷ�Ϊ������ʽ��
         * 
         * һ���Ǵ����ģ�һ���ǿ�ʼ�ʹ��ڵ�
         * @return ����Ǵ��������ľͷ���true�����򷵻�false
         */
        bool iscreate() const
        {
            return _bCreate;
        }

    protected:
        /*
         * �Ƿ񴴽�������
         */
        bool _bCreate;
    };

public:
    /**
     * ��ʼ������洢
     * @param iShmKey
     * @param iSize
     */
    void initStore( const char* file, size_t iSize )
    {
        _file = file;
        std::replace(_file.begin(),_file.end(),'/','_');
        int fd = -1;
        string filename = string( "/dev/shm/" ) + _file;
        if ( TC_File::isFileExist( filename ) )
        {
            iSize = TC_File::getFileSize( filename );
        }
        if ( ( fd = _ss.shmopen( _file.c_str(), iSize ) ) >= 0 )
        {
            _mmap.mmap( iSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

            if ( _ss.iscreate() )
            {
                _t.create( _mmap.getPointer(), _mmap.getSize() );
            }
            else
            {
                _t.connect( _mmap.getPointer(), _mmap.getSize() );
            }
            _ss.shmfree( fd );
        }
    }

    /**
     * ��չ�ռ�, Ŀǰֻ��hashmap��Ч
     */
    int expand( size_t iSize )
    {
        TC_LockT<typename LockPolicy::Mutex> lock( LockPolicy::mutex() );
        if ( iSize < _mmap.getSize() )
        {
            return -1;
        }
        int fd = -1;
        int ret = -1;
        if ( ( fd = _ss.shmopen( _file.c_str(), iSize ) ) >= 0 )
        {
            TC_Mmap m(false);
            m.mmap( iSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

            if((ret=_t.append( m.getPointer(), m.getSize()))==0)
            {
                _mmap.munmap();
                _mmap=m;
                _mmap.setOwner(true);
            }
            else
            {
                m.munmap();
            }
            _ss.shmfree(fd);
        }
        return ret;
    }

    /**
     * �ͷŹ����ڴ�
     */
    void release()
    {
        _ss.shmclose(_file.c_str());
    }
protected:
    string  _file;
    ShmStore _ss;
    TC_Mmap _mmap;
    T     _t;
};

/**
 * �ļ��洢
 */
template<typename T, typename LockPolicy>
class FileStorePolicy : public LockPolicy
{
public:
    /**
     * ��ʼ���ļ�
     * @param file, �ļ�·��
     * @param iSize, �ļ���С
     */
    void initStore(const char *file, size_t iSize)
    {
        _file = file;
        if ( TC_File::isFileExist( _file) )
        {
            iSize = TC_File::getFileSize( _file);
        }
        _mmap.mmap(file, iSize);
        if(_mmap.iscreate())
        {
            _t.create(_mmap.getPointer(), iSize);
        }
        else
        {
            _t.connect(_mmap.getPointer(), iSize);
        }
    }

    /**
     * ��չ�ռ�, Ŀǰֻ��hashmap��Ч
     */
    int expand(size_t iSize)
    {
        TC_LockT<typename LockPolicy::Mutex> lock(LockPolicy::mutex());

        TC_Mmap m(false);
        m.mmap(_file.c_str(), iSize);

        int ret = _t.append(m.getPointer(), iSize);

        if(ret == 0)
        {
            _mmap.munmap();
            _mmap = m;
            _mmap.setOwner(true);
        }
        else
        {
            m.munmap();
        }

        return ret;
    }

protected:
    string  _file;
    TC_Mmap _mmap;
    T       _t;
};

//////////////////////////////////////////////////////////////////////
// ������: ����, �߳���, ������

/**
 * ����
 */
class EmptyLockPolicy
{
public:
    typedef TC_EmptyMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * �߳�������
 */
class ThreadLockPolicy
{
public:
    typedef TC_ThreadMutex Mutex;
    Mutex &mutex()     { return _mutex; }

protected:
    Mutex _mutex;
};

/**
 * ����������
 */
class SemLockPolicy
{
public:
    typedef TC_SemMutex Mutex;
    void initLock(key_t iSemKey)    { return _mutex.init(iSemKey); }
    Mutex &mutex()                  { return _mutex; }

protected:
    Mutex _mutex;
};

}

#endif
