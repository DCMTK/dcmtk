#
#	Makefile for dcmtk (DICOM Toolkit)
#

SHELL= /bin/sh
configdir= config

include $(configdir)/Makefile.def

all:  config-all ofstd-all dcmdata-all dcmtls-all dcmnet-all imagectn-all dcmwlm-all dcmimgle-all dcmsr-all dcmsign-all dcmpstat-all dcmimage-all dcmjpeg-all

libsrc-all:  ofstd-libsrc-all dcmdata-libsrc-all dcmtls-libsrc-all dcmnet-libsrc-all imagectn-libsrc-all dcmwlm-libsrc-all dcmimgle-libsrc-all dcmsr-libsrc-all dcmsign-libsrc-all dcmpstat-libsrc-all dcmimage-libsrc-all dcmjpeg-libsrc-all

install:  config-install ofstd-install dcmdata-install dcmtls-install dcmnet-install imagectn-install dcmwlm-install dcmimgle-install dcmsr-install dcmsign-install dcmpstat-install dcmimage-install dcmjpeg-install dcmtk-install-doc install-man

install-all: install install-lib install-html

install-bin:  config-install-bin ofstd-install-bin dcmdata-install-bin dcmtls-install-bin dcmnet-install-bin imagectn-install-bin dcmwlm-install-bin dcmimgle-install-bin dcmsr-install-bin dcmsign-install-bin dcmpstat-install-bin dcmimage-install-bin dcmjpeg-install-bin

install-doc:  config-install-doc ofstd-install-doc dcmdata-install-doc dcmtls-install-doc dcmnet-install-doc imagectn-install-doc dcmwlm-install-doc dcmimgle-install-doc dcmsr-install-doc dcmsign-install-doc dcmpstat-install-doc dcmimage-install-doc dcmjpeg-install-doc

install-etc:  config-install-etc ofstd-install-etc dcmdata-install-etc dcmtls-install-etc dcmnet-install-etc imagectn-install-etc dcmwlm-install-etc dcmimgle-install-etc dcmsr-install-etc dcmsign-install-etc dcmpstat-install-etc dcmimage-install-etc dcmjpeg-install-etc

install-lib:  config-install-lib ofstd-install-lib dcmdata-install-lib dcmtls-install-lib dcmnet-install-lib imagectn-install-lib dcmwlm-install-lib dcmimgle-install-lib dcmsr-install-lib dcmsign-install-lib dcmpstat-install-lib dcmimage-install-lib dcmjpeg-install-lib

install-include:  config-install-include ofstd-install-include dcmdata-install-include dcmtls-install-include dcmnet-install-include imagectn-install-include dcmwlm-install-include dcmimgle-install-include dcmsr-install-include dcmsign-install-include dcmpstat-install-include dcmimage-install-include dcmjpeg-install-include

install-support:  config-install-support ofstd-install-support dcmdata-install-support dcmtls-install-support dcmnet-install-support imagectn-install-support dcmwlm-install-support dcmimgle-install-support dcmsr-install-support dcmsign-install-support dcmpstat-install-support dcmimage-install-support dcmjpeg-install-support

dcmtk-install-doc:
	$(configdir)/mkinstalldirs $(docdir)
	for file in COPYRIGHT FAQ HISTORY; do \
		$(INSTALL_DATA) $$file $(docdir) ;\
	done

install-man:
	(cd doxygen; $(MAKE) install-man)

install-html:
	(cd doxygen; $(MAKE) install-html)

man:
	(cd doxygen; $(MAKE) man)

html:
	(cd doxygen; $(MAKE) html)

config-all:
	(cd config; $(MAKE) ARCH="$(ARCH)" all)

config-libsrc-all:
	(cd config; $(MAKE) ARCH="$(ARCH)" libsrc-all)

config-install:
	(cd config; $(MAKE) ARCH="$(ARCH)" install)

config-install-bin:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-bin)

config-install-doc:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-doc)

config-install-etc:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-etc)

config-install-lib:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-lib)

config-install-include:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-include)

config-install-support:
	(cd config; $(MAKE) ARCH="$(ARCH)" install-support)

ofstd-all:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" all)

ofstd-libsrc-all:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" libsrc-all)

ofstd-install:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install)

ofstd-install-bin:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-bin)

ofstd-install-doc:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-doc)

ofstd-install-etc:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-etc)

ofstd-install-lib:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-lib)

ofstd-install-include:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-include)

ofstd-install-support:
	(cd ofstd; $(MAKE) ARCH="$(ARCH)" install-support)

dcmdata-all:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" all)

dcmdata-libsrc-all:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmdata-install:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install)

dcmdata-install-bin:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmdata-install-doc:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmdata-install-etc:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmdata-install-lib:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmdata-install-include:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-include)

dcmdata-install-support:
	(cd dcmdata; $(MAKE) ARCH="$(ARCH)" install-support)

dcmtls-all:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" all)

dcmtls-libsrc-all:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmtls-install:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install)

dcmtls-install-bin:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmtls-install-doc:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmtls-install-etc:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmtls-install-lib:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmtls-install-include:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-include)

dcmtls-install-support:
	(cd dcmtls; $(MAKE) ARCH="$(ARCH)" install-support)

dcmnet-all:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" all)

dcmnet-libsrc-all:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmnet-install:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install)

dcmnet-install-bin:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmnet-install-doc:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmnet-install-etc:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmnet-install-lib:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmnet-install-include:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-include)

dcmnet-install-support:
	(cd dcmnet; $(MAKE) ARCH="$(ARCH)" install-support)

imagectn-all:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" all)

imagectn-libsrc-all:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" libsrc-all)

imagectn-install:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install)

imagectn-install-bin:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-bin)

imagectn-install-doc:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-doc)

imagectn-install-etc:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-etc)

imagectn-install-lib:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-lib)

imagectn-install-include:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-include)

imagectn-install-support:
	(cd imagectn; $(MAKE) ARCH="$(ARCH)" install-support)

dcmwlm-all:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" all)

dcmwlm-libsrc-all:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmwlm-install:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install)

dcmwlm-install-bin:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmwlm-install-doc:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmwlm-install-etc:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmwlm-install-lib:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmwlm-install-include:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-include)

dcmwlm-install-support:
	(cd dcmwlm; $(MAKE) ARCH="$(ARCH)" install-support)

dcmimgle-all:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" all)

dcmimgle-libsrc-all:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmimgle-install:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install)

dcmimgle-install-bin:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmimgle-install-doc:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmimgle-install-etc:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmimgle-install-lib:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmimgle-install-include:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-include)

dcmimgle-install-support:
	(cd dcmimgle; $(MAKE) ARCH="$(ARCH)" install-support)

dcmsr-all:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" all)

dcmsr-libsrc-all:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmsr-install:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install)

dcmsr-install-bin:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmsr-install-doc:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmsr-install-etc:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmsr-install-lib:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmsr-install-include:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-include)

dcmsr-install-support:
	(cd dcmsr; $(MAKE) ARCH="$(ARCH)" install-support)

dcmsign-all:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" all)

dcmsign-libsrc-all:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmsign-install:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install)

dcmsign-install-bin:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmsign-install-doc:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmsign-install-etc:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmsign-install-lib:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmsign-install-include:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-include)

dcmsign-install-support:
	(cd dcmsign; $(MAKE) ARCH="$(ARCH)" install-support)

dcmpstat-all:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" all)

dcmpstat-libsrc-all:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmpstat-install:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install)

dcmpstat-install-bin:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmpstat-install-doc:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmpstat-install-etc:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmpstat-install-lib:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmpstat-install-include:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-include)

dcmpstat-install-support:
	(cd dcmpstat; $(MAKE) ARCH="$(ARCH)" install-support)

dcmimage-all:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" all)

dcmimage-libsrc-all:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmimage-install:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install)

dcmimage-install-bin:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmimage-install-doc:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmimage-install-etc:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmimage-install-lib:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmimage-install-include:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-include)

dcmimage-install-support:
	(cd dcmimage; $(MAKE) ARCH="$(ARCH)" install-support)

dcmjpeg-all:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" all)

dcmjpeg-libsrc-all:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" libsrc-all)

dcmjpeg-install:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install)

dcmjpeg-install-bin:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-bin)

dcmjpeg-install-doc:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-doc)

dcmjpeg-install-etc:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-etc)

dcmjpeg-install-lib:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-lib)

dcmjpeg-install-include:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-include)

dcmjpeg-install-support:
	(cd dcmjpeg; $(MAKE) ARCH="$(ARCH)" install-support)

dependencies:
	(cd ofstd; $(MAKE) dependencies)
	(cd dcmdata; $(MAKE) dependencies)
	(cd dcmtls; $(MAKE) dependencies)
	(cd dcmnet; $(MAKE) dependencies)
	(cd imagectn; $(MAKE) dependencies)
	(cd dcmwlm; $(MAKE) dependencies)
	(cd dcmimgle; $(MAKE) dependencies)
	(cd dcmsr; $(MAKE) dependencies)
	(cd dcmsign; $(MAKE) dependencies)
	(cd dcmpstat; $(MAKE) dependencies)
	(cd dcmimage; $(MAKE) dependencies)
	(cd dcmjpeg; $(MAKE) dependencies)
	-(cd config; $(MAKE) dependencies)

clean:
	(cd ofstd; $(MAKE) clean)
	(cd dcmdata; $(MAKE) clean)
	(cd dcmtls; $(MAKE) clean)
	(cd dcmnet; $(MAKE) clean)
	(cd imagectn; $(MAKE) clean)
	(cd dcmwlm; $(MAKE) clean)
	(cd dcmimgle; $(MAKE) clean)
	(cd dcmsr; $(MAKE) clean)
	(cd dcmsign; $(MAKE) clean)
	(cd dcmpstat; $(MAKE) clean)
	(cd dcmimage; $(MAKE) clean)
	(cd dcmjpeg; $(MAKE) clean)
	(cd doxygen; $(MAKE) clean)
	-(cd config; $(MAKE) clean)
	rm -f $(TRASH)

distclean:
	(cd ofstd; $(MAKE) distclean)
	(cd dcmdata; $(MAKE) distclean)
	(cd dcmtls; $(MAKE) distclean)
	(cd dcmnet; $(MAKE) distclean)
	(cd imagectn; $(MAKE) distclean)
	(cd dcmwlm; $(MAKE) distclean)
	(cd dcmimgle; $(MAKE) distclean)
	(cd dcmsr; $(MAKE) distclean)
	(cd dcmsign; $(MAKE) distclean)
	(cd dcmpstat; $(MAKE) distclean)
	(cd dcmimage; $(MAKE) distclean)
	(cd dcmjpeg; $(MAKE) distclean)
	(cd doxygen; $(MAKE) distclean)
	-(cd config; $(MAKE) distclean)
	rm -f $(TRASH)

