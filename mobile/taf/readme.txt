1 Ŀ¼�ṹ˵��:
- bin: ����֮��Ķ������ļ�, ���������Լ���صķ���
- doc: �ĵ�
- include: ��Ҫ�����ṩ���ʵ�includeĿ¼
  - config: �������ķ����ͷ�ļ� 
  - hessian: hessian��ͷ�ļ�
  - jce: jce�����ͷ�ļ�
  - jmem: ����jceЭ��Ĺ����ڴ�, �ļ���ѭ�����к�hashmap
  - log: ��־�����ͷ�ļ�   
  - nodeF: ��node�첽�ϱ������Ŀ�ͷ�ļ�
  - parse: ����jce�ļ��Ŀ���ļ�
  - patch: ���������ͷ�ļ�
  - proxy: �м��proxy����ͷ�ļ�
  - servant: �м�����񲿷�ͷ�ļ�
  - util: ������
- java: java����ع��ߺʹ�����
  - hessian2jce: ����hessian�Ľӿ��ļ���, �����Զ����ɾ��󲿷ֵ�hce�ļ�, ����hce�ļ������Զ�����hessian��c++�ͻ��˵��ô���, ����μ�src/hessian, src/hce2cpp
  - jce: java��jce�����Դ���Լ�proxy���ֵĴ���, ������Ҫ�õ�jceЭ��Ĵ����������ô���.
- javalib: java��Ҫ�Ĺ���lib, �Ż���ͳһ���
- jce: jce�ļ�Ŀ¼
  - config: config�����jce�ļ�
  - log: log�����jce�ļ�
  - node: node�����jce�ļ�
  - nodeF: ��node�ϱ�������jce�ļ�
  - patch: patch�����jce�ļ�
  - proxy: proxy��Ҫ��jce�ļ�
  - registry: registry�����jce�ļ�
  - servant: servant��Ҫ��jce�ļ�
  - stat: stat�����jce�ļ�
- lib: lib��
- script: �����Զ����ɽű��Լ�demo
  - create_taf_server.sh: �Զ����ɿշ���Ľű�
  - demo: demo����
- src: Դ��Ŀ¼
  - adminclient: registry�Ŀͻ��˹�����, ��ִ�г�����뵽binĿ¼��
  - config: �������ķ��� , ��ִ�г�����뵽binĿ¼��
  - hce2cpp: hce����cpp��Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��  
  - jce2cpp: jce����cpp��Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��
  - libconfig: �������Ŀͻ���lib��, lib����뵽libĿ¼��
  - libhessian: hessian���ÿͻ��˿�, lib����뵽libĿ¼��
  - liblog: ��־���Ŀͻ���lib��, lib����뵽libĿ¼��
  - libparse: ����jce�ļ��Ŀ�, lib����뵽libĿ¼��
  - libpatch: ��������ͻ���lib��, lib����뵽libĿ¼��
  - libproxy: proxy���ֵ�lib��
  - libservant: jce���񲿷ֵ�lib��, �����֧��ͬ�����첽����
  - libstat: ģ�����÷���Ŀͻ���lib��, lib����뵽libĿ¼��
  - libutil: �������, lib����뵽libĿ¼��
  - log: ��־���ķ���, ��ִ�г�����뵽binĿ¼��
  - node: node��Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��
  - patch: patch��Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��
  - patchclient: patch�ͻ��˿�ִ���ļ�, ��ִ�г�����뵽binĿ¼��
  - registry: registry��Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��
  - stat: ģ�����÷����Դ��Ŀ¼, ��ִ�г�����뵽binĿ¼��
- test: ���Գ���
  - client: ����server�ͻ��˵Ĵ���
  - config: �����������ķ���  
  - echoserver: ����libserver��echoserver
  - hce2cpp: ����hce2cpp���ߵ�Դ��
  - hessian: ����hessian������Դ��
  - jce2cpp: ����jce2cpp���ߵ�Դ��
  - jce2java: ����jce2java��Դ��
  - jmem: ����jmem���
  - log: ����log����
  - node: ����nodeԴ��
  - registry: ����registry����
  - server: ���Է�����
  - util: ���Թ�����

����˵��:
	1 �����������ͨ����������, ����:
	./NodeServer --config=node.config.conf
	2 �ͻ��˲��Գ�����Է������÷���, ������Բμ�test/node����Ĵ����д;
	
��ͨ�����Makefile˵��:
	#--------------------------------------------------------
	#��ǰĿ¼��tafĿ¼�����·��
	TOPDIR  := ../..
	#Ӧ��ͨ�ù���
	include ${TOPDIR}/make.rules
	#-------------------------------------------------------------------------------
	#��Ҫ�����includeĿ¼, taf/include·���Ѿ���make.rules������, ��Ҫ���õ�ͷ�ļ�, ��Ҫ�����·��, ����"servant/Application.h"
	INCLUDE   += 
	#��Ҫ�����lib: taf/lib·���Ѿ���make.rules������, ������Ҫָ����Ҫ�����Ŀ�Ϳ�����, ����Ҫ��·��
	LIB       += -ltaf -lutil 
	#-------------------------------------------------------------------------------
	#�����Ŀ��
	TARGET  := ${NODEBIN}

	#-------------------------------------------------------------------------------
	#��Ҫ����makefile��֧��
	include ${TOPDIR}/node/Node.mk
	include ${TOPDIR}/inc.mk
