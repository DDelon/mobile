
TOPDIR	:= ../..
include ${TOPDIR}/make.rules

#------------------------------------------------

all : $(JCE_H) $(JCE_CPP)

ifneq ($(JCE_SRC),)
$(JCE_H) $(JCE_CPP) : $(JCE_SRC)
	rm -vf $(JCE_H) $(JCE_CPP)
	$(JCE2CPP) $(JCEFLAGS)  --with-taf $(JCE_SRC)
	cp -rf ${JCE_H} ${NODEF_H}
	cp -rf ${JCE_CPP} ${NODEF_C}
endif

#------------------------------------------------

clean:
	rm -vf $(JCE_H) $(JCE_CPP)

cleanall:
	rm -vf $(JCE_H) $(JCE_CPP)

install:
	echo "do nothing"
