--------
��Ŀ¼���������һ�����ļ�:libxmltrans.a�����ڽ�ʹ��XML��ʽ������Э�飬ת����WUPЭ����������Ŀǰ����Iphoneƽ̨��ʹ�á�

����ֱ����TAF�����ʹ�ã�Ҳ�����ṩ�����ն�ʹ�á�
km���и�����������ע����¡�

--------
�����ṩ���������ʱ����Ҫע�⣬����ҪTAF���һЩͷ�ļ�����������������ļ�Ŀ¼���£�
jce/JceDisplayer.h
jce/Jce.h
jce/JceType.h
jce/RequestF.h
Markup.h
Markup.cpp
xmltrans.h
xmltrans.cpp
Makefile (���ļ�����һ����ʾ���ļ����ݣ����ǵ�ǰĿ¼���ȴ��ڵ�Makefile)

---------
�ṩ��ʹ���ߵ�Makefile�ļ����ݣ�ʹ�ø�Makefile�������һ��.a�ļ���

INCLUDE := -I./

all:libxmltrans.a

libtrans.a:xmltrans.o Markup.o
        ar r libtrans.a xmltrans.o Markup.o

Markup.o:Markup.cpp
        g++ -Wall -c -o Markup.o Markup.cpp -I./ $(INCLUDE)

xmltrans.o:xmltrans.cpp
        g++ -Wall -c -o xmltrans.o xmltrans.cpp -I./ $(INCLUDE)

clean:
        rm -rf *.o libxmltrans.a