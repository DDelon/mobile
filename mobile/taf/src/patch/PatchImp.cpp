#include "util/tc_file.h"
#include "util/tc_config.h"
#include "util/tc_md5.h"
#include "log/taf_logger.h"
#include "PatchImp.h"
#include "PatchCache.h"

extern TC_Config *g_conf;

extern PatchCache g_PatchCache;

struct LIST
{
    LIST(const string& dir, vector<FileInfo> &vf) : _dir(dir), _vf(vf)
    {
    }

    void operator()(const string &file)
    {
        //��ͨ�ļ���ͬ��, �����ļ���Ч
        if(taf::TC_File::isFileExistEx(file, S_IFREG))
        {
            FileInfo fi;
            fi.path     = file.substr(_dir.length());
            fi.size     = taf::TC_File::getFileSize(file);
            fi.canExec  = taf::TC_File::canExecutable(file);
            fi.md5      = taf::TC_MD5::md5file(file);

            _vf.push_back(fi);

            LOG->debug() << "LIST:" << fi.path << "|FILE:" << file << "|SIZE:" << fi.size << "|EXEC:" << (fi.canExec?"true":"false") << endl;
        }
    }

    string              _dir;
    vector<FileInfo>    &_vf;
};

PatchImp::PatchImp():_iSize(1024*1024)
{
}

void PatchImp::initialize()
{
    try
    {
        _directory  = (*g_conf)["/taf<directory>"];
        _iSize      = TC_Common::toSize(g_conf->get("/taf<size>", "1M"), 1024*1024);
    }
    catch(exception &ex)
    {
        LOG->debug() << "directory must not be empty." << endl;
        exit(0);
    }

    if(!taf::TC_File::isAbsolute(_directory))
    {
        LOG->debug() << "directory must be absolute directory path." << endl;
        exit(0);
    }
    LOG->debug() << "patch dirtectory:" << _directory << endl;
    LOG->debug() << "patch size:" << _iSize << endl;
}


/**************************************************************************************************
 ** ����ӿڣ���ͨ���ؽӿ�
 **
 **/
int PatchImp::listFileInfo(const string &path, vector<FileInfo> & vf, JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << path << endl;

    string dir = taf::TC_File::simplifyDirectory(_directory + "/" + path);
    LOG->debug() << __FUNCTION__ << "|" << path << "|" << dir << endl;

    return __listFileInfo(dir, vf);
}

int PatchImp::download(const string & file, int pos, vector<char> & vb, JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << file << "|" << pos << endl;

    string path = taf::TC_File::simplifyDirectory(_directory + "/" + file);

    int iRet = -1;
    if (iRet < 0) iRet = __downloadFromMem (path, pos, vb);
    if (iRet < 0) iRet = __downloadFromFile(path, pos, vb);

    return iRet;
}

/**************************************************************************************************
 ** ����ӿڣ�����ָ���ļ��ӿ�
 **
 **/
int PatchImp::listFileInfoSubborn(const string & path, vector<FileInfo> &vf, JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << path << endl;

    return __listFileInfo(path, vf);
}

int PatchImp::downloadSubborn(const string &file, int pos, vector<char> &vb, JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << file << "|" << pos << endl;

    return __downloadFromFile(file, pos, vb);
}


/**************************************************************************************************
 *  ���������ڲ�ʹ�ú���
 */
int PatchImp::__listFileInfo(const string &path, vector<FileInfo> &vf)
{
    LOG->debug() << __FUNCTION__ << "|" << path << endl;
    if (path.find("..") != string::npos)
    {
        return -1;
    }

    if(!taf::TC_File::isFileExistEx(path, S_IFDIR))
    {
        //���ļ�
        FileInfo fi;
        fi.path     = taf::TC_File::extractFileName(path);
        fi.size     = taf::TC_File::getFileSize(path);
        fi.canExec  = taf::TC_File::canExecutable(path);
        fi.md5      = taf::TC_MD5::md5file(path);

        vf.push_back(fi);
        return 1;
    }
    else
    {
        //Ŀ¼
        vector<string> files;
        taf::TC_File::listDirectory(path, files, true);
        for_each(files.begin(), files.end(), LIST(path, vf));
        return 0;
    }

    return 0;
}


int PatchImp::__downloadFromMem (const string & file, size_t pos, vector<char> & vb)
{
    LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|S:" << _iSize << endl;

    pair<char *, size_t> mem;
    if (g_PatchCache.load(file, mem) != 0)
    {
        LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|LoadFile error" << endl;
        return -1;
    }

    if (pos >= mem.second)
    {
        LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|to tail ok" << endl;
        g_PatchCache.release(file);
        return 1;
    }

    const size_t sizeBuf = mem.second - pos >= _iSize?_iSize:mem.second - pos;
    LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|L:" << sizeBuf << endl;

    vb.resize(sizeBuf);
    memcpy((char *)&vb[0], mem.first + pos, sizeBuf);

    g_PatchCache.release(file);
    return 0;
}


int PatchImp::__downloadFromFile(const string & file, size_t pos, vector<char> & vb)
{
    LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|S:" << _iSize << endl;

    FILE * fp = fopen(file.c_str(), "rb");
    if (fp == NULL)
    {
        LOG->error() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|open file error:" << strerror(errno) << endl;
        return -1;
    }

    //��ָ��λ�ÿ�ʼ������
    if (fseek(fp, pos, SEEK_SET) == -1)
    {
        fclose(fp);

        LOG->error() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|fseek error:" << strerror(errno) << endl;
        return -2;
    }

    //��ʼ��ȡ�ļ�
    vb.resize(_iSize);
    size_t r = fread((void*)(&vb[0]), 1, _iSize, fp);
    if (r > 0)
    {
        //�ɹ���ȡr�ֽ�����
        vb.resize(r);
        fclose(fp);
        LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|R:" << r << endl;
        return 0;
    }
    else
    {
        //���ļ�ĩβ��
        if (feof(fp))
        {
            fclose(fp);
            LOG->debug() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|to tail ok" << endl;
            return 1;
        }

        //��ȡ�ļ�������
        LOG->error() << __FUNCTION__ << "|F:" << file << "|P:" << pos << "|R:" << r << "|" << strerror(errno) << endl;
        fclose(fp);
        return -3;
    }

}

