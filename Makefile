#
#	Makefile for dcmtk (DICOM Toolkit)
#

SHELL = /bin/sh
configdir = config

include $(configdir)/Makefile.def

all:  config-all ofstd-all oflog-all dcmdata-all dcmimgle-all dcmimage-all dcmjpeg-all dcmjpls-all dcmtls-all dcmnet-all dcmsr-all dcmsign-all dcmwlm-all dcmqrdb-all dcmpstat-all dcmrt-all

libsrc-all:  ofstd-libsrc-all oflog-libsrc-all dcmdata-libsrc-all dcmimgle-libsrc-all dcmimage-libsrc-all dcmjpeg-libsrc-all dcmjpls-libsrc-all dcmtls-libsrc-all dcmnet-libsrc-all dcmsr-libsrc-all dcmsign-libsrc-all dcmwlm-libsrc-all dcmqrdb-libsrc-all dcmpstat-libsrc-all dcmrt-libsrc-all

tests-all:  config-tests-all ofstd-tests-all oflog-tests-all dcmdata-tests-all dcmimgle-tests-all dcmimage-tests-all dcmjpeg-tests-all dcmjpls-tests-all dcmtls-tests-all dcmnet-tests-all dcmsr-tests-all dcmsign-tests-all dcmwlm-tests-all dcmqrdb-tests-all dcmpstat-tests-all dcmrt-tests-all

install:  config-install ofstd-install oflog-install dcmdata-install dcmimgle-install dcmimage-install dcmjpeg-install dcmjpls-install dcmtls-install dcmnet-install dcmsr-install dcmsign-install dcmwlm-install dcmqrdb-install dcmpstat-install dcmrt-install dcmtk-install-doc install-man

install-all: install install-lib install-html

install-bin:  config-install-bin ofstd-install-bin oflog-install-bin dcmdata-install-bin dcmimgle-install-bin dcmimage-install-bin dcmjpeg-install-bin dcmjpls-install-bin dcmtls-install-bin dcmnet-install-bin dcmsr-install-bin dcmsign-install-bin dcmwlm-install-bin dcmqrdb-install-bin dcmpstat-install-bin dcmrt-install-bin

install-doc:  config-install-doc ofstd-install-doc oflog-install-doc dcmdata-install-doc dcmimgle-install-doc dcmimage-install-doc dcmjpeg-install-doc dcmjpls-install-doc dcmtls-install-doc dcmnet-install-doc dcmsr-install-doc dcmsign-install-doc dcmwlm-install-doc dcmqrdb-install-doc dcmpstat-install-doc dcmrt-install-doc

install-data:  config-install-data ofstd-install-data oflog-install-data dcmdata-install-data dcmimgle-install-data dcmimage-install-data dcmjpeg-install-data dcmjpls-install-data dcmtls-install-data dcmnet-install-data dcmsr-install-data dcmsign-install-data dcmwlm-install-data dcmqrdb-install-data dcmpstat-install-data dcmrt-install-data

install-etc:  config-install-etc ofstd-install-etc oflog-install-etc dcmdata-install-etc dcmimgle-install-etc dcmimage-install-etc dcmjpeg-install-etc dcmjpls-install-etc dcmtls-install-etc dcmnet-install-etc dcmsr-install-etc dcmsign-install-etc dcmwlm-install-etc dcmqrdb-install-etc dcmpstat-install-etc dcmrt-install-etc

install-lib:  config-install-lib ofstd-install-lib oflog-install-lib dcmdata-install-lib dcmimgle-install-lib dcmimage-install-lib dcmjpeg-install-lib dcmjpls-install-lib dcmtls-install-lib dcmnet-install-lib dcmsr-install-lib dcmsign-install-lib dcmwlm-install-lib dcmqrdb-install-lib dcmpstat-install-lib dcmrt-install-lib

install-include:  config-install-include ofstd-install-include oflog-install-include dcmdata-install-include dcmimgle-install-include dcmimage-install-include dcmjpeg-install-include dcmjpls-install-include dcmtls-install-include dcmnet-install-include dcmsr-install-include dcmsign-install-include dcmwlm-install-include dcmqrdb-install-include dcmpstat-install-include dcmrt-install-include

install-support:  config-install-support ofstd-install-support oflog-install-support dcmdata-install-support dcmimgle-install-support dcmimage-install-support dcmjpeg-install-support dcmjpls-install-support dcmtls-install-support dcmnet-install-support dcmsr-install-support dcmsign-install-support dcmwlm-install-support dcmqrdb-install-support dcmpstat-install-support dcmrt-install-support

check: tests-all
	$(MAKE) -s check-nosilent

check-nosilent:  config-check ofstd-check oflog-check dcmdata-check dcmimgle-check dcmimage-check dcmjpeg-check dcmjpls-check dcmtls-check dcmnet-check dcmsr-check dcmsign-check dcmwlm-check dcmqrdb-check dcmpstat-check dcmrt-check

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

help:
	@echo "Some available make targets:"
	@echo ""
	@echo "all:                 build all libraries, tests and applications"
	@echo "libsrc-all:          build all libraries"
	@echo "install-all:         install the toolkit with its libraries and documentation"
	@echo "install:             install all applications"
	@echo "install-lib:         install the libraries"
	@echo "install-html:        install the HTML documentation"
	@echo "html:                build the HTML documentation with doxygen"
	@echo "man:                 build the man pages with doxygen"
	@echo "check:               build and run all tests"
	@echo "dependencies:        update all Makefile.dep dependency files"
	@echo "clean:               remove compiler output"
	@echo "distclean:           remove compiler output and generated build structure"
	@echo "                     (This removes all files generated by configure)"
	@echo ""
	@echo "Additionally, there are a number of module-specific targets available."
	@echo "For example:"
	@echo ""
	@echo "<module>-all         build the complete module"
	@echo "<module>-libsrc-all  build the module's libraries"
	@echo "<module>-install     install the module"
	@echo "<module>-install-lib install the module's libraries"
	@echo "<module>-check       build and run the module's tests"
	@echo ""
	@echo "The following modules are available:"
	@echo ""
	@echo "ofstd oflog dcmdata dcmimgle dcmimage dcmjpeg dcmjpls dcmtls dcmnet dcmsr dcmsign dcmwlm dcmqrdb dcmpstat dcmrt"

config-all:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

config-libsrc-all:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

config-tests-all:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

config-check:
	(cd config && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

ofstd-all:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

ofstd-libsrc-all:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

ofstd-tests-all:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

ofstd-check:
	(cd ofstd && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

oflog-all:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

oflog-libsrc-all:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

oflog-tests-all:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

oflog-check:
	(cd oflog && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmdata-all:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmdata-libsrc-all:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmdata-tests-all:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmdata-check:
	(cd dcmdata && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmimgle-all:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmimgle-libsrc-all:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmimgle-tests-all:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmimgle-check:
	(cd dcmimgle && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmimage-all:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmimage-libsrc-all:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmimage-tests-all:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmimage-check:
	(cd dcmimage && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmjpeg-all:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmjpeg-libsrc-all:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmjpeg-tests-all:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmjpeg-check:
	(cd dcmjpeg && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmjpls-all:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmjpls-libsrc-all:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmjpls-tests-all:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmjpls-check:
	(cd dcmjpls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmtls-all:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmtls-libsrc-all:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmtls-tests-all:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmtls-check:
	(cd dcmtls && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmnet-all:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmnet-libsrc-all:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmnet-tests-all:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmnet-check:
	(cd dcmnet && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmsr-all:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmsr-libsrc-all:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmsr-tests-all:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmsr-check:
	(cd dcmsr && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmsign-all:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmsign-libsrc-all:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmsign-tests-all:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmsign-check:
	(cd dcmsign && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmwlm-all:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmwlm-libsrc-all:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmwlm-tests-all:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmwlm-check:
	(cd dcmwlm && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmqrdb-all:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmqrdb-libsrc-all:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmqrdb-tests-all:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmqrdb-check:
	(cd dcmqrdb && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmpstat-all:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmpstat-libsrc-all:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmpstat-tests-all:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

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

dcmpstat-check:
	(cd dcmpstat && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dcmrt-all:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" all)

dcmrt-libsrc-all:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" libsrc-all)

dcmrt-tests-all:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" tests-all)

dcmrt-install:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install)

dcmrt-install-bin:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-bin)

dcmrt-install-doc:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-doc)

dcmrt-install-data:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-data)

dcmrt-install-etc:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-etc)

dcmrt-install-lib:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-lib)

dcmrt-install-include:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-include)

dcmrt-install-support:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" install-support)

dcmrt-check:
	(cd dcmrt && $(MAKE) ARCH="$(ARCH)" DESTDIR="$(DESTDIR)" check)

dependencies:
	(cd ofstd && $(MAKE) dependencies)
	(cd oflog && $(MAKE) dependencies)
	(cd dcmdata && $(MAKE) dependencies)
	(cd dcmimgle && $(MAKE) dependencies)
	(cd dcmimage && $(MAKE) dependencies)
	(cd dcmjpeg && $(MAKE) dependencies)
	(cd dcmjpls && $(MAKE) dependencies)
	(cd dcmtls && $(MAKE) dependencies)
	(cd dcmnet && $(MAKE) dependencies)
	(cd dcmsr && $(MAKE) dependencies)
	(cd dcmsign && $(MAKE) dependencies)
	(cd dcmwlm && $(MAKE) dependencies)
	(cd dcmqrdb && $(MAKE) dependencies)
	(cd dcmpstat && $(MAKE) dependencies)
	(cd dcmrt && $(MAKE) dependencies)
	-(cd config && $(MAKE) dependencies)

clean:
	(cd ofstd && $(MAKE) clean)
	(cd oflog && $(MAKE) clean)
	(cd dcmdata && $(MAKE) clean)
	(cd dcmimgle && $(MAKE) clean)
	(cd dcmimage && $(MAKE) clean)
	(cd dcmjpeg && $(MAKE) clean)
	(cd dcmjpls && $(MAKE) clean)
	(cd dcmtls && $(MAKE) clean)
	(cd dcmnet && $(MAKE) clean)
	(cd dcmsr && $(MAKE) clean)
	(cd dcmsign && $(MAKE) clean)
	(cd dcmwlm && $(MAKE) clean)
	(cd dcmqrdb && $(MAKE) clean)
	(cd dcmpstat && $(MAKE) clean)
	(cd dcmrt && $(MAKE) clean)
	(cd doxygen && $(MAKE) clean)
	-(cd config && $(MAKE) clean)
	rm -f $(TRASH)

distclean:
	(cd ofstd && $(MAKE) distclean)
	(cd oflog && $(MAKE) distclean)
	(cd dcmdata && $(MAKE) distclean)
	(cd dcmimgle && $(MAKE) distclean)
	(cd dcmimage && $(MAKE) distclean)
	(cd dcmjpeg && $(MAKE) distclean)
	(cd dcmjpls && $(MAKE) distclean)
	(cd dcmtls && $(MAKE) distclean)
	(cd dcmnet && $(MAKE) distclean)
	(cd dcmsr && $(MAKE) distclean)
	(cd dcmsign && $(MAKE) distclean)
	(cd dcmwlm && $(MAKE) distclean)
	(cd dcmqrdb && $(MAKE) distclean)
	(cd dcmpstat && $(MAKE) distclean)
	(cd dcmrt && $(MAKE) distclean)
	(cd doxygen && $(MAKE) distclean)
	-(cd config && $(MAKE) distclean)
	rm -f $(TRASH)

