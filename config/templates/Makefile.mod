#
#	Makefile for modules
#

@SET_MAKE@

SHELL = /bin/sh
VPATH = @srcdir@:@top_srcdir@/include:@configdir@/include
srcdir = @srcdir@
top_srcdir = @top_srcdir@
configdir = @configdir@

include $(configdir)/@common_makefile@


all: include-all libsrc-all apps-all docs-all tests-all


install: install-bin install-doc install-support

install-bin: apps-install

install-doc: docs-install

install-etc: etc-install

install-include: include-install

install-lib: libsrc-install include-install

install-support: etc-install


include-all:
	(cd include; $(MAKE) ARCH="$(ARCH)" all)

libsrc-all: include-all
	(cd libsrc; $(MAKE) ARCH="$(ARCH)" all)

apps-all: libsrc-all
	(cd apps; $(MAKE) ARCH="$(ARCH)" all)

tests-all: libsrc-all
	(cd tests; $(MAKE) ARCH="$(ARCH)" all)

docs-all:
	(cd docs; $(MAKE) all)


include-install:
	(cd include; $(MAKE) ARCH="$(ARCH)" install)

libsrc-install: libsrc-all
	(cd libsrc; $(MAKE) ARCH="$(ARCH)" install)

apps-install: apps-all
	(cd apps; $(MAKE) ARCH="$(ARCH)" install)

docs-install:
	(cd docs; $(MAKE) install)

etc-install:
	(cd etc; $(MAKE) install)

clean:
	(cd include; $(MAKE) clean)
	(cd libsrc; $(MAKE) clean)
	(cd apps; $(MAKE) clean)
	(cd tests; $(MAKE) clean)
	(cd docs; $(MAKE) clean)
	(cd etc; $(MAKE) clean)
	rm -f $(TRASH)

distclean:
	(cd include; $(MAKE) distclean)
	(cd libsrc; $(MAKE) distclean)
	(cd apps; $(MAKE) distclean)
	(cd tests; $(MAKE) distclean)
	(cd docs; $(MAKE) distclean)
	(cd etc; $(MAKE) distclean)
	rm -f $(DISTTRASH)

distconfigclean: distclean
	-(cd $(configdir); $(MAKE) distclean)


dependencies:
	(cd libsrc; touch $(DEP); $(MAKE) dependencies)
	(cd apps; touch $(DEP); $(MAKE) dependencies)
	(cd tests; touch $(DEP); $(MAKE) dependencies)
