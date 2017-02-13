#ifndef __PATCH_CACHE_H__
#define __PATCH_CACHE_H__
#include <string>
#include <map>
#include <vector>
#include "util/tc_monitor.h"

const size_t SIZE_BUMEM_MIN =   5 * 1024 * 1024;
const size_t SIZE_BUMEM_MAX = 100 * 1024 * 1024;

class PatchCache
{
private:
    enum FileLoad 
    {
        EM_NLOAD    = 0,
        EM_LOADING  = 1,
        EM_RELOAD   = 2,
        EM_LOADED   = 3   
    };

    struct MemState
    {
        string      FileName;       //�ļ�·��
        size_t      FileSize;       //�ļ���С
        size_t      FileInode;      //Inode������
        time_t      FileTime;       //�ļ�����ʱ��

        char *      MemBuf;         //�ÿ��ڴ����ʼ��ַ
        size_t      MemSize;        //�ÿ��ڴ�Ĵ�С
        size_t      MemCount;       //ָ��ÿ��ڴ����Ŀ
        time_t      MemTime;        //�ÿ��ڴ�������ʱ��
        FileLoad    MemLoad;        //�ÿ��ڴ��Ѿ����سɹ�
    };
public:
    void setMemOption(const size_t MemMax, const size_t MemMin, const size_t MemNum)
    {
        _MemMax     = MemMax > SIZE_BUMEM_MAX?SIZE_BUMEM_MAX:MemMax;
        _MemMin     = MemMin < SIZE_BUMEM_MIN?SIZE_BUMEM_MIN:MemMin;
        _MemNum     = MemNum;
    }

    int load(const std::string & sFile, std::pair<char *, size_t> & mem);

    int release(const std::string & sFile);

private:
    int __loadFile(const std::string & sFile, char * szBuff, size_t sizeLen);

    int __getMem(const std::string & sFile, struct MemState *& cur);

private:
    taf::TC_ThreadLock _mutex;

    std::vector<MemState *>             _vecMems;

    std::map<std::string, MemState *>   _mapFiles;

    size_t _MemMax;

    size_t _MemMin;

    size_t _MemNum;
};

#endif

