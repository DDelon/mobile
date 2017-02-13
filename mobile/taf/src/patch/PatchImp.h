#include "Patch.h"

using namespace taf;

class PatchImp : public Patch
{
public:
    /**
     *
     */
    PatchImp();

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy() {};

    /**
    * ��ȡ·���������ļ��б���Ϣ
    * @param path, Ŀ¼·��, ���_directory��·��, ������..
    * @param vector<FileInfo>, �ļ��б���Ϣ
    * @return int
    */
    int listFileInfo(const string &path, vector<FileInfo> &vf, JceCurrentPtr current);

    /**
    * �����ļ�
    * @param file, �ļ���ȫ·��
    * @param pos, ��ʲôλ�ÿ�ʼ����
    * @return vector<byte>, �ļ�����
    */
    int download(const string &file, int pos, vector<char> &vb, JceCurrentPtr current);

    /**
    * ��ȡ·���������ļ��б���Ϣ
    * @param path, Ŀ¼·��, ���_directory��·��, ������..
    * @param vector<FileInfo>, �ļ��б���Ϣ
    * @return int
    */
    int listFileInfoSubborn(const string &path, vector<FileInfo> &vf, JceCurrentPtr current);

    /**
    * �����ļ�
    * @param file, �ļ��ľ���·�������������ļ�·��
    * @param pos, ��ʲôλ�ÿ�ʼ����
    * @param vector<byte>, �ļ�����
    * @param int, 0:��ȡ��ʾ�ɹ�, 1:��ȡ���ļ�ĩβ��, <0: ��ȡʧ��
    */
    int downloadSubborn(const string & file, int pos, vector<char> &vb, JceCurrentPtr current);

protected:
	int __listFileInfo(const string &path, vector<FileInfo> &vf);

	int __downloadFromMem (const string & file, size_t pos, vector<char> & vb);

    int __downloadFromFile(const string & file, size_t pos, vector<char> & vb);

protected:
    /**
     * Ŀ¼
     */
    string _directory;

    /**
     * ÿ��ͬ����С
     */
    size_t _iSize;
};


