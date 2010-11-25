#
#	Makefile for dcmtk (DICOM Toolkit)
#

SHELL = /bin/sh
configdir = config

include $(configdir)/Makefile.def

all:  config-all ofstd-all oflog-all dcmdata-all dcmtls-all dcmnet-all dcmqrdb-all dcmwlm-all dcmimgle-all dcmsr-all dcmsign-all dcmpstat-all dcmimage-all dcmjpeg-all dcmjpls-all

libsrc-all:  ofstd-libsrc-all oflog-libsrc-all dcmdata-libsrc-all dcmtls-libsrc-all dcmnet-libsrc-all dcmqrdb-libsrc-all dcmwlm-libsrc-all dcmimgle-libsrc-all dcmsr-libsrc-all dcmsign-libsrc-all dcmpstat-libsrc-all dcmimage-libsrc-all dcmjpeg-libsrc-all dcmjpls-libsrc-all

install:  config-install ofstd-install oflog-install dcmdata-install dcmtls-install dcmnet-install dcmqrdb-install dcmwlm-install dcmimgle-install dcmsr-install dcmsign-install dcmpstat-install dcmimage-install dcmjpeg-install dcmjpls-install dcmtk-install-doc install-man

install-all: install install-lib install-html

install-bin:  config-install-bin ofstd-install-bin oflog-install-bin dcmdata-install-bin dcmtls-install-bin dcmnet-install-bin dcmqrdb-install-bin dcmwlm-install-bin dcmimgle-install-bin dcmsr-install-bin dcmsign-install-bin dcmpstat-install-bin dcmimage-install-bin dcmjpeg-install-bin dcmjpls-install-bin

install-doc:  config-install-doc ofstd-install-doc oflog-install-doc dcmdata-install-doc dcmtls-install-doc dcmnet-install-doc dcmqrdb-install-doc dcmwlm-install-doc dcmimgle-install-doc dcmsr-install-doc dcmsign-install-doc dcmpstat-install-doc dcmimage-install-doc dcmjpeg-install-doc dcmjpls-install-doc

install-data:  config-install-data ofstd-install-data oflog-install-data dcmdata-install-data dcmtls-install-data dcmnet-install-data dcmqrdb-install-data dcmwlm-install-data dcmimgle-install-data dcmsr-install-data dcmsign-install-data dcmpstat-install-data dcmimage-install-data dcmjpeg-install-data dcmjpls-install-data

install-etc:  config-install-etc ofstd-install-etc oflog-install-etc dcmdata-install-etc dcmtls-install-etc dcmnet-install-etc dcmqrdb-install-etc dcmwlm-install-etc dcmimgle-install-etc dcmsr-install-etc dcmsign-install-etc dcmpstat-install-etc dcmimage-install-etc dcmjpeg-install-etc dcmjpls-install-etc

install-lib:  config-install-lib ofstd-install-lib oflog-install-lib dcmdata-install-lib dcmtls-install-lib dcmnet-install-lib dcmqrdb-install-lib dcmwlm-install-lib dcmimgle-install-lib dcmsr-install-lib dcmsign-install-lib dcmpstat-install-lib dcmimage-install-lib dcmjpeg-install-lib dcmjpls-install-lib

install-include:  config-install-include ofstd-install-include oflog-install-include dcmdata-install-include dcmtls-install-include dcmnet-install-include dcmqrdb-install-include dcmwlm-install-include dcmimgle-install-include dcmsr-install-include dcmsign-install-include dcmpstat-install-include dcmimage-install-include dcmjpeg-install-include dcmjpls-install-include

install-support:  config-install-support ofstd-install-support oflog-install-support dcmdata-install-support dcmtls-install-support dcmnet-install-support dcmqrdb-install-support dcmwlm-install-support dcmimgle-install-support dcmsr-install-support dcmsign-install-support dcmpstat-install-support dcmimage-install-support dcmjpeg-install-support dcmjpls-install-support

dcmtk-install-doc:
	$(configdir)/mkinstalldirs $(DESTDIR)$(docdir)
	for file in ANNOUNCE.$(PACKAGE_VERSION_NUMBER) CHANGES.$(PACKAGE_VERSION_NUMBER) COPYRIGHT FAQ HISTORY VERSION; do \
		$(INSTALL_DATA) $$file $(DESTDIR)$(docdir) ;\
	done

install-man:
	(cd doxygen && $(MAKE) DESTDIR="$(DESTDIR)" install-man)

install-html:
	(cd doxygen && $(MAKE) DESTDIR="$(DESTDIR)" install-html)

man:
	(cd doxygen && $(MAKE) DESTDIR="$(DESTDIR)" man)

html:
	(cd doxygen && $(MAKE) DESTDIR="$(DESTDIR)" html)

config-all:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

config-libsrc-all:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

config-install:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

config-install-bin:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

config-install-doc:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

config-install-data:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

config-install-etc:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

config-install-lib:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

config-install-include:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

config-install-support:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

ofstd-all:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

ofstd-libsrc-all:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

ofstd-install:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

ofstd-install-bin:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

ofstd-install-doc:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

ofstd-install-data:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

ofstd-install-etc:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

ofstd-install-lib:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

ofstd-install-include:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

ofstd-install-support:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

oflog-all:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

oflog-libsrc-all:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

oflog-install:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

oflog-install-bin:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

oflog-install-doc:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

oflog-install-data:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

oflog-install-etc:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

oflog-install-lib:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

oflog-install-include:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

oflog-install-support:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmdata-all:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmdata-libsrc-all:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmdata-install:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmdata-install-bin:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmdata-install-doc:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmdata-install-data:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmdata-install-etc:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmdata-install-lib:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmdata-install-include:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmdata-install-support:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmtls-all:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmtls-libsrc-all:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmtls-install:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmtls-install-bin:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmtls-install-doc:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmtls-install-data:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmtls-install-etc:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmtls-install-lib:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmtls-install-include:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmtls-install-support:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmnet-all:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmnet-libsrc-all:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmnet-install:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmnet-install-bin:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmnet-install-doc:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmnet-install-data:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmnet-install-etc:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmnet-install-lib:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmnet-install-include:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmnet-install-support:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmqrdb-all:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmqrdb-libsrc-all:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmqrdb-install:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmqrdb-install-bin:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmqrdb-install-doc:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmqrdb-install-data:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmqrdb-install-etc:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmqrdb-install-lib:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmqrdb-install-include:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmqrdb-install-support:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmwlm-all:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmwlm-libsrc-all:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmwlm-install:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmwlm-install-bin:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmwlm-install-doc:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmwlm-install-data:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmwlm-install-etc:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmwlm-install-lib:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmwlm-install-include:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmwlm-install-support:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmimgle-all:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmimgle-libsrc-all:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmimgle-install:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmimgle-install-bin:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmimgle-install-doc:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmimgle-install-data:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmimgle-install-etc:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmimgle-install-lib:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmimgle-install-include:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmimgle-install-support:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmsr-all:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmsr-libsrc-all:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmsr-install:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmsr-install-bin:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmsr-install-doc:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmsr-install-data:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmsr-install-etc:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmsr-install-lib:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmsr-install-include:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmsr-install-support:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmsign-all:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmsign-libsrc-all:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmsign-install:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmsign-install-bin:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmsign-install-doc:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmsign-install-data:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmsign-install-etc:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmsign-install-lib:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmsign-install-include:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmsign-install-support:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmpstat-all:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmpstat-libsrc-all:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmpstat-install:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmpstat-install-bin:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmpstat-install-doc:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmpstat-install-data:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmpstat-install-etc:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmpstat-install-lib:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmpstat-install-include:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmpstat-install-support:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmimage-all:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmimage-libsrc-all:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmimage-install:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmimage-install-bin:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmimage-install-doc:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmimage-install-data:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmimage-install-etc:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmimage-install-lib:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmimage-install-include:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmimage-install-support:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmjpeg-all:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmjpeg-libsrc-all:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmjpeg-install:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmjpeg-install-bin:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmjpeg-install-doc:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmjpeg-install-data:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmjpeg-install-etc:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmjpeg-install-lib:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmjpeg-install-include:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmjpeg-install-support:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmjpls-all:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmjpls-libsrc-all:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmjpls-install:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmjpls-install-bin:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmjpls-install-doc:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmjpls-install-data:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmjpls-install-etc:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmjpls-install-lib:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmjpls-install-include:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmjpls-install-support:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dependencies:
	(cd ofstd && $(MAKE) dependencies)
	(cd oflog && $(MAKE) dependencies)
	(cd dcmdata && $(MAKE) dependencies)
	(cd dcmtls && $(MAKE) dependencies)
	(cd dcmnet && $(MAKE) dependencies)
	(cd dcmqrdb && $(MAKE) dependencies)
	(cd dcmwlm && $(MAKE) dependencies)
	(cd dcmimgle && $(MAKE) dependencies)
	(cd dcmsr && $(MAKE) dependencies)
	(cd dcmsign && $(MAKE) dependencies)
	(cd dcmpstat && $(MAKE) dependencies)
	(cd dcmimage && $(MAKE) dependencies)
	(cd dcmjpeg && $(MAKE) dependencies)
	(cd dcmjpls && $(MAKE) dependencies)
	-(cd config && $(MAKE) dependencies)

clean:
	(cd ofstd && $(MAKE) clean)
	(cd oflog && $(MAKE) clean)
	(cd dcmdata && $(MAKE) clean)
	(cd dcmtls && $(MAKE) clean)
	(cd dcmnet && $(MAKE) clean)
	(cd dcmqrdb && $(MAKE) clean)
	(cd dcmwlm && $(MAKE) clean)
	(cd dcmimgle && $(MAKE) clean)
	(cd dcmsr && $(MAKE) clean)
	(cd dcmsign && $(MAKE) clean)
	(cd dcmpstat && $(MAKE) clean)
	(cd dcmimage && $(MAKE) clean)
	(cd dcmjpeg && $(MAKE) clean)
	(cd dcmjpls && $(MAKE) clean)
	(cd doxygen && $(MAKE) clean)
	-(cd config && $(MAKE) clean)
	rm -f $(TRASH)

distclean:
	(cd ofstd && $(MAKE) distclean)
	(cd oflog && $(MAKE) distclean)
	(cd dcmdata && $(MAKE) distclean)
	(cd dcmtls && $(MAKE) distclean)
	(cd dcmnet && $(MAKE) distclean)
	(cd dcmqrdb && $(MAKE) distclean)
	(cd dcmwlm && $(MAKE) distclean)
	(cd dcmimgle && $(MAKE) distclean)
	(cd dcmsr && $(MAKE) distclean)
	(cd dcmsign && $(MAKE) distclean)
	(cd dcmpstat && $(MAKE) distclean)
	(cd dcmimage && $(MAKE) distclean)
	(cd dcmjpeg && $(MAKE) distclean)
	(cd dcmjpls && $(MAKE) distclean)
	(cd doxygen && $(MAKE) distclean)
	-(cd config && $(MAKE) distclean)
	rm -f $(TRASH)

