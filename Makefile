# Makefile to compile JX source distribution

# If ACE_ROOT is defined, we toss the copy included in the distribution and
# create a symbolic link to the existing directory.

ifndef ACE_ROOT
  DEFAULT_ACE_ROOT := ${shell pwd}/ACE/ACE_wrappers
endif

MAKE_CMD := ${notdir ${MAKE}}

#
# tell user what options are available
#

.PHONY : default
default:
	@echo

  ifndef ACE_ROOT
	@echo "If you already have a compatible version of ACE on your computer, then"
	@echo "set ACE_ROOT to point to the top level ACE directory.  (This is part of"
	@echo "the standard ACE configuration.)  If you don't have ACE installed, add"
	@echo "the following to ~/.cshrc, if you use csh or tcsh:"
	@echo
	@echo "  setenv ACE_ROOT ${DEFAULT_ACE_ROOT}"
	@echo
	@echo "If you use bash, add the following to ~/.bashrc instead:"
	@echo
	@echo "  export ACE_ROOT=${DEFAULT_ACE_ROOT}"
	@echo
  endif

 ifeq ($(shell whoami),root)

  ifndef JX_INSTALL_ROOT
	@echo "To install the binaries in a directory other than /usr/bin, set the"
	@echo "environment variable JX_INSTALL_ROOT to the desired directory."
	@echo "If you do this, JX_INSTALL_ROOT must be on your execution path."
  endif
	@echo
  ifndef JX_LIB_ROOT
	@echo "To install the libraries in a directory other than /usr/lib, set the"
	@echo "environment variable JX_LIB_ROOT to the desired directory."
  endif

 else

  ifndef JX_INSTALL_ROOT
	@echo "To install the binaries in a directory other than ~/bin, (or ./bin, if"
	@echo "~/bin doesn't exist), set the environment variable JX_INSTALL_ROOT to"
	@echo "the desired directory.  (Use setenv if you use csh or export if you use bash.)"
	@echo
	@echo "This path (either ~/bin or JX_INSTALL_ROOT), must be on your execution path."
	@echo "(Use the \"set path\" command.)"
  endif

 endif

	@echo
	@echo "Then run one of the following or check the README file:"
	@echo
	@echo "  ${MAKE_CMD} cygwin32      for Cygwin on Windows"
	@echo "  ${MAKE_CMD} darwin_5      for Macintosh OS X 10.5"
	@echo "  ${MAKE_CMD} darwin_6      for Macintosh OS X 10.6"
	@echo "  ${MAKE_CMD} darwin_7      for Macintosh OS X 10.7"
	@echo "  ${MAKE_CMD} darwin_9      for Macintosh OS X 10.9"
	@echo "  ${MAKE_CMD} darwin_10     for Macintosh OS X 10.10"
	@echo "  ${MAKE_CMD} linux_x86_64  for 32-bit Linux systems"
	@echo "  ${MAKE_CMD} linux_x86_32  for 64-bit Linux systems"
	@echo
	@echo "If you are not sure which system you have, run \"uname -a\"."
	@echo

	@echo
	@echo "To clean up afterwards, you can use:"
	@echo
	@echo "  ${MAKE_CMD} tidy           removes .o files"
	@echo "  ${MAKE_CMD} clean          removes .o files and libraries"

  ifeq ($(shell whoami),root)
	@echo "  ${MAKE_CMD} uninstall      removes installed binaries and libraries"
  else
	@echo "  ${MAKE_CMD} uninstall      removes installed binaries"
	@echo
	@echo "The libraries are not installed in any system directories, so only use"
	@echo "\"clean\" if you link everything statically."
  endif

	@echo

#
# Useful variables
#

# ${JMAKE} insures that ACE_ROOT is passed to all other Makefiles
# This can't be done earlier, because jx_constants redefines JX_INSTALL_ROOT,
# thereby cancelling the message about it.

JX_ROOT := .
include ${JX_ROOT}/include/make/jx_constants

JMAKE = ${MAKE} PATH=${PATH}:${JX_INSTALL_ROOT}

INSTALL_CMD := cd lib; ${JMAKE} install

#
# Useful macros
#

TEST_ACE_CONFIG = \
    if { test ! -f ACE/ACE_wrappers/ace/config.h; } then \
        { \
        ln -sf

CREATE_ACE_CONFIG = \
        ACE/ACE_wrappers/ace/config.h; \
        } \
    fi

TEST_ACE_MACROS = \
    if { test ! -f ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; } then \
        { \
        ln -sf

CREATE_ACE_MACROS = \
        ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; \
        } \
    fi

#
# glibc 2.8
#

.PHONY : linux_x86_32
linux_x86_32: linux_common
	@ln -sf sys/Linux-Intel-32  include/make/jx_config
	@${INSTALL_CMD}

#
# 64-bit RedHat 5.x
#

.PHONY : linux_x86_64
linux_x86_64: linux_common
	@ln -sf sys/Linux-Intel-64  include/make/jx_config
	@${INSTALL_CMD}

#
# Linux shared code
#

.PHONY : linux_common
linux_common: prep
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-linux.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_linux.GNU ${CREATE_ACE_MACROS}

#
# Cygwin
#

.PHONY : cygwin32
cygwin32: prep
	@ln -sf sys/Linux-Intel-cygwin32 \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_CygWin.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-cygwin32.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_cygwin32.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# OS X
#

.PHONY: darwin_5 
darwin_5: prep
	@ln -sf sys/OSX_5_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-leopard.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_leopard.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_6
darwin_6: prep
	@ln -sf sys/OSX_6_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-snowleopard.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_snowleopard.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_7
darwin_7: prep
	@ln -sf sys/OSX_6_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-lion.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_lion.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_9
darwin_9: prep
	@ln -sf sys/OSX_9_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-mountainlion.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_mountainlion.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

.PHONY: darwin_10
darwin_10: prep
	@ln -sf sys/OSX_10_g++ \
            include/make/jx_config
	@ln -sf ../../include/missing_proto/jMissingProto_empty.h \
            include/jcore/jMissingProto.h
	@${TEST_ACE_CONFIG} config-macosx-yosemite.h ${CREATE_ACE_CONFIG}
	@${TEST_ACE_MACROS} platform_macosx_yosemite.GNU ${CREATE_ACE_MACROS}
	@${INSTALL_CMD}

#
# shared code
#

.PHONY : prep
prep: check_install_dir get_ace get_aspell clean_links
	@if { test ! -e libjcore/code/jStringData.h; } then \
         { \
         cp -f libjcore/jStringData_init.h libjcore/code/jStringData.h; \
         chmod u+w libjcore/code/jStringData.h; \
         } \
     fi

# Solaris test doesn't support ! operator

.PHONY : check_install_dir
check_install_dir:
  ifdef JX_INSTALL_ROOT
	@if { test -d ${JX_INSTALL_ROOT} -a -w ${JX_INSTALL_ROOT}; } then \
         { \
         true; \
         } \
     else \
         { \
         echo; \
         echo "Please set JX_INSTALL_ROOT to a valid directory and make sure that"; \
         echo "it is on your execution path."; \
         echo; \
         false; \
         } \
     fi
  endif

#	@if { test -d ${ACE_ROOT} -a ! ACE/ACE_wrappers -ef ${ACE_ROOT}; } then

CHECK_ACE := lib/util/j_has_ace

CC := ${shell if { which gcc > /dev/null; } \
              then { echo gcc; } \
              else { echo ${CC}; } fi }

.PHONY : get_ace
get_ace:
  ifdef ACE_ROOT
	@if { test ! -d ACE/ACE_wrappers || \
          { ${CC} ${CHECK_ACE}.c -o ${CHECK_ACE}; \
            ${CHECK_ACE} ${ACE_ROOT} ACE/ACE_wrappers; } ; } then \
         ${RM} -r ACE/ACE_wrappers; \
         ln -sf ${ACE_ROOT} ACE/ACE_wrappers; \
     fi;
	@if { test ! '(' -f ${ACE_ROOT}/ace/Makefile -o -f ${ACE_ROOT}/ace/GNUmakefile ')'; } then \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.a; \
         ln -sf /usr/lib/libACE.a lib/libACE-${ACE_LIB_VERSION}.a; \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.so; \
         ln -sf /usr/lib/libACE.so lib/libACE-${ACE_LIB_VERSION}.so; \
         ${RM} lib/libACE.so.${ACE_VERSION}; \
     fi
  else
	@if { test ! -e ${DEFAULT_ACE_ROOT}; } then \
         cd ACE; \
         if { ! test -f ACE.tgz ; } then \
           if { which wget; } then \
               wget -O ACE.tgz http://libjx.sourceforge.net/data/ACE-${ACE_VERSION}.tar.gz; \
           elif { which curl; } then \
               curl -o ACE.tgz http://libjx.sourceforge.net/data/ACE-${ACE_VERSION}.tar.gz; \
           else \
               echo "Please install either curl or wget"; \
               exit 1; \
           fi; \
         fi; \
         tar -xzf ACE.tgz; touch ACE_wrappers/${ACE_VERSION}; ./patch_ace; \
     fi
	@if { test ! '(' -f ${DEFAULT_ACE_ROOT}/ace/Makefile -o -f ${DEFAULT_ACE_ROOT}/ace/GNUmakefile ')'; } then \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.a; \
         ln -sf /usr/lib/libACE.a lib/libACE-${ACE_LIB_VERSION}.a; \
         ${RM} lib/libACE-${ACE_LIB_VERSION}.so; \
         ln -sf /usr/lib/libACE.so lib/libACE-${ACE_LIB_VERSION}.so; \
         ${RM} lib/libACE.so.${ACE_VERSION}; \
     fi
  endif

.PHONY : get_aspell
get_aspell:
	@if { ! which aspell 2> /dev/null; } then \
         cd misc; \
         if { ! test -f aspell.tgz ; } then \
             if { which wget; } then \
                 wget -O aspell.tgz http://libjx.sourceforge.net/data/aspell-0.60.6.1.tar.gz; \
             elif { which curl; } then \
                 curl -o aspell.tgz http://libjx.sourceforge.net/data/aspell-0.60.6.1.tar.gz; \
             else \
                 echo "Please install either curl or wget"; \
                 exit 1; \
             fi; \
         fi; \
         if { ! test -d aspell-* ; } then \
             tar -xzf aspell.tgz; \
             ./patch/aspell/apply; \
         fi; \
         cd aspell-*; \
         if { ! test -f Makefile ; } then \
             ./configure; \
         fi; \
         make; \
         echo please enter sudo password to install aspell; \
         sudo make install; \
     fi
	@if { ! test -d misc/aspell6-* ; } then \
         cd misc; \
         if { ! test -f aspell_en.tgz ; } then \
             if { which wget; } then \
                 wget -O aspell_en.tgz http://libjx.sourceforge.net/data/aspell6-en-7.1-0.tar.gz; \
             elif { which curl; } then \
                 curl -o aspell_en.tgz http://libjx.sourceforge.net/data/aspell6-en-7.1-0.tar.gz; \
             else \
                 echo "Please install either curl or wget"; \
                 exit 1; \
             fi; \
         fi; \
         if { ! test -d aspell6-* ; } then \
             tar -xzf aspell_en.tgz; \
         fi; \
         cd aspell6-*; \
         if { ! test -f Makefile ; } then \
             ./configure; \
         fi; \
         make; \
         echo please enter sudo password to install aspell-en; \
         sudo make install; \
     fi

.PHONY : clean_links
clean_links:
	@${RM} include/make/jx_config
	@${RM} include/jcore/jMissingProto.h
	@if { test ! -L ACE/ACE_wrappers; } then \
         ${RM} ACE/ACE_wrappers/ace/config.h; \
         ${RM} ACE/ACE_wrappers/include/makeinclude/platform_macros.GNU; \
     fi

#
# clean up
#

.PHONY : tidy
tidy:
	@cd lib; ${JMAKE} tidy

.PHONY : clean
clean:
	@cd lib; ${JMAKE} clean

.PHONY : uninstall
uninstall:
	@cd lib; ${JMAKE} uninstall
