# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is mozilla.org code.
#
# The Initial Developer of the Original Code is
# Netscape Communications.
# Portions created by the Initial Developer are Copyright (C) 2001
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#  Brian Ryner <bryner@brianryner.com>
#  Jonathan Wilson <jonwil@tpgi.com.au>
#  Dan Mosedale <dmose@mozilla.org>
#  Benjamin Smedberg <benjamin@smedbergs.us>
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

DEPTH		= ../..
topsrcdir	= ../..
srcdir		= .
VPATH		= .

include $(DEPTH)/config/autoconf.mk

DIRS		= profile/extensions

PREF_JS_EXPORTS = $(srcdir)/profile/firefox.js \
		  $(srcdir)/profile/channel-prefs.js \
		  $(NULL)

ifndef MOZ_BRANDING_DIRECTORY
PREF_JS_EXPORTS += $(srcdir)/firefox-branding.js
endif

# hardcode en-US for the moment
AB_CD = en-US

DEFINES += -DAB_CD=$(AB_CD)

APP_VERSION = $(shell cat $(srcdir)/../config/version.txt)
DEFINES += -DAPP_VERSION="$(APP_VERSION)"
APP_UA_NAME = $(shell echo $(MOZ_APP_DISPLAYNAME) | sed -e's/[^A-Za-z]//g')
DEFINES += -DAPP_UA_NAME="$(APP_UA_NAME)"

DIST_FILES = application.ini

GRE_MILESTONE = $(shell $(PYTHON) $(topsrcdir)/config/printconfigsetting.py $(LIBXUL_DIST)/bin/platform.ini Build Milestone)
GRE_BUILDID = $(shell $(PYTHON) $(topsrcdir)/config/printconfigsetting.py $(LIBXUL_DIST)/bin/platform.ini Build BuildID)

DEFINES += -DGRE_MILESTONE=$(GRE_MILESTONE) -DGRE_BUILDID=$(GRE_BUILDID)

ifdef MOZ_CO_DATE
   DEFINES += -DMOZ_SOURCE_STAMP="$(MOZ_CO_DATE)"
endif

ifdef MOZ_MEMORY
ifneq ($(OS_ARCH),WINNT)
LIBS += -ljemalloc
endif
endif

ifdef LIBXUL_SDK
include $(topsrcdir)/config/rules.mk
else
# Build a binary bootstrapping with XRE_main

ifeq ($(USE_SHORT_LIBNAME), 1)
PROGRAM = $(MOZ_APP_NAME)$(BIN_SUFFIX)
else
PROGRAM = $(MOZ_APP_NAME)-bin$(BIN_SUFFIX)
endif

REQUIRES = \
	xpcom \
	string \
	$(NULL)

CPPSRCS = nsBrowserApp.cpp

LOCAL_INCLUDES += -I$(topsrcdir)/toolkit/xre

ifdef BUILD_STATIC_LIBS
ifdef _MSC_VER
STATIC_COMPONENTS_LINKER_PATH = -LIBPATH:$(DEPTH)/staticlib
else
STATIC_COMPONENTS_LINKER_PATH = -L$(DEPTH)/staticlib
endif
LIBS += $(DEPTH)/toolkit/xre/$(LIB_PREFIX)xulapp_s.$(LIB_SUFFIX)
else
ifneq (,$(filter mac cocoa,$(MOZ_WIDGET_TOOLKIT)))
LIBS += $(DIST)/bin/XUL
else
EXTRA_DSO_LIBS += xul
endif
endif

ifneq (,$(filter mac cocoa,$(MOZ_WIDGET_TOOLKIT)))
TK_LIBS := $(TK_LIBS)
endif

ifdef MOZ_ENABLE_LIBXUL
APP_XPCOM_LIBS = $(XPCOM_GLUE_LDOPTS)
else
MOZILLA_INTERNAL_API = 1
APP_XPCOM_LIBS = $(XPCOM_LIBS)
endif

LIBS += \
	$(STATIC_COMPONENTS_LINKER_PATH) \
	$(EXTRA_DSO_LIBS) \
	$(MOZ_JS_LIBS) \
	$(APP_XPCOM_LIBS) \
	$(NSPR_LIBS) \
	$(TK_LIBS) \
	$(NULL)

# Add explicit X11 dependency when building against X11 toolkits
ifneq (,$(filter gtk2,$(MOZ_WIDGET_TOOLKIT)))
LIBS += $(XLDFLAGS) $(XLIBS) $(ZLIB_LIBS)
endif

ifdef MOZ_JPROF
LIBS += -ljprof
endif

ifndef MOZ_WINCONSOLE
ifdef MOZ_DEBUG
MOZ_WINCONSOLE = 1
else
MOZ_WINCONSOLE = 0
endif
endif

NSDISTMODE = copy

include $(topsrcdir)/config/config.mk

ifdef _MSC_VER
# Always enter a Windows program through wmain, whether or not we're
# a console application.
WIN32_EXE_LDFLAGS += -ENTRY:wmainCRTStartup
endif

ifndef BUILD_STATIC_LIBS

ifdef NS_TRACE_MALLOC
# when libxul is enabled, trace-malloc is part of it
ifndef MOZ_ENABLE_LIBXUL
EXTRA_DSO_LIBS += tracemalloc
endif
endif

else
include $(topsrcdir)/config/static-config.mk

EXTRA_DEPS	+= \
	$(STATIC_EXTRA_DEPS) \
	$(NULL)
DEFINES		+= $(STATIC_DEFINES)
CPPSRCS		+= $(STATIC_CPPSRCS)
EXTRA_DSO_LIBS	+= $(STATIC_EXTRA_DSO_LIBS)
REQUIRES	+= $(STATIC_REQUIRES)
EXTRA_LIBS	+= $(STATIC_EXTRA_LIBS)
endif

ifeq ($(OS_ARCH),WINNT)
OS_LIBS += $(call EXPAND_LIBNAME,comctl32 comdlg32 uuid shell32 ole32 oleaut32 version winspool)
OS_LIBS += $(call EXPAND_LIBNAME,usp10 msimg32)
ifdef MOZ_ENABLE_GLITZ
OS_LIBS += $(call EXPAND_LIBNAME,opengl32)
endif

RCINCLUDE = splash.rc
ifndef GNU_CC
RCFLAGS += -DMOZ_PHOENIX -I$(srcdir)
else
RCFLAGS += -DMOZ_PHOENIX --include-dir $(srcdir)
endif
ifdef BUILD_STATIC_LIBS
RCFLAGS += -DMOZ_STATIC_BUILD
endif
ifdef DEBUG
RCFLAGS += -DDEBUG
endif
endif

ifeq ($(OS_ARCH),BeOS)
BEOS_PROGRAM_RESOURCE = $(srcdir)/apprunner-beos.rsrc
ifdef BUILD_STATIC_LIBS
OS_LIBS += -ltracker -lgame
endif
endif

ifeq ($(OS_ARCH),OS2)
RESFILE=splashos2.res
RCFLAGS += -DMOZ_PHOENIX
ifdef BUILD_STATIC_LIBS
EXE_DEF_FILE = browser.def
RCFLAGS += -DMOZ_STATIC_BUILD -i $(DIST)/include/widget
endif
ifdef DEBUG
RCFLAGS += -DDEBUG
endif
RCFLAGS += -DFIREFOX_ICO=\"$(DIST)/branding/firefox.ico\" -DDOCUMENT_ICO=\"$(DIST)/branding/document.ico\"
endif

include $(topsrcdir)/config/rules.mk

DEFINES += -DFIREFOX_ICO=\"$(DIST)/branding/firefox.ico\" -DDOCUMENT_ICO=\"$(DIST)/branding/document.ico\"

ifdef MOZILLA_OFFICIAL
DEFINES += -DMOZILLA_OFFICIAL
endif

ifdef BUILD_STATIC_LIBS
include $(topsrcdir)/config/static-rules.mk

DEFINES += -DIMPL_XREAPI
endif

ifeq ($(MOZ_WIDGET_TOOLKIT),cocoa)
ifdef BUILD_STATIC_LIBS
ifdef USE_PREBINDING
BIN_FLAGS += -Wl,-headerpad -Wl,5a0c
endif
endif
endif

ifeq ($(MOZ_WIDGET_TOOLKIT),photon)
LIBS += -lphexlib
endif

ifeq ($(OS_ARCH),WINNT)
#
# Control the default heap size.
# This is the heap returned by GetProcessHeap().
# As we use the CRT heap, the default size is too large and wastes VM.
#
# The default heap size is 1MB on Win32.
# The heap will grow if need be.
#
# Set it to 256k.  See bug 127069.
#
ifndef GNU_CC
LDFLAGS += /HEAP:0x40000
endif
endif

$(PROGRAM): $(DEPTH)/toolkit/xre/$(LIB_PREFIX)xulapp_s.$(LIB_SUFFIX)

ifeq ($(OS_ARCH),OS2)
ifdef BUILD_STATIC_LIBS
$(EXE_DEF_FILE):
	rm -f $@
	@echo NAME mozilla >$(EXE_DEF_FILE)
	@echo IMPORTS >>$(EXE_DEF_FILE)
	@echo 	WinQueryProperty		= PMMERGE.5450 >>$(EXE_DEF_FILE)
	@echo 	WinRemoveProperty		= PMMERGE.5451 >>$(EXE_DEF_FILE)
	@echo 	WinSetProperty			= PMMERGE.5452 >>$(EXE_DEF_FILE)

LDFLAGS += -Zlinker /NOE
endif
endif

ifneq (,$(filter-out OS2 WINNT,$(OS_ARCH)))

$(MOZ_APP_NAME):: mozilla.in Makefile.in Makefile $(DEPTH)/config/autoconf.mk
	cat $< | sed -e "s|%MOZAPPDIR%|$(installdir)|" > $@
	chmod +x $@

libs:: $(MOZ_APP_NAME)
	$(INSTALL) $< $(DIST)/bin

install:: $(MOZ_APP_NAME)
	$(SYSINSTALL) $< $(DESTDIR)$(bindir)

GARBAGE += $(MOZ_APP_NAME)
GARBAGE += $(addprefix $(DIST)/bin/defaults/pref/, firefox.js)

endif

endif # LIBXUL_SDK

ifneq (,$(filter gtk2,$(MOZ_WIDGET_TOOLKIT)))

ICON_FILES	= \
		$(DIST)/branding/mozicon128.png \
		$(DIST)/branding/mozicon50.xpm \
		$(DIST)/branding/mozicon16.xpm \
		$(DIST)/branding/document.png \
		$(NULL)

libs:: $(ICON_FILES)
	$(INSTALL) $^ $(DIST)/bin/icons

install::
	$(SYSINSTALL) $(IFLAGS1) $(ICON_FILES) $(DESTDIR)$(mozappdir)/icons
endif

ifeq ($(MOZ_WIDGET_TOOLKIT),gtk2)
libs::
	$(INSTALL) $(DIST)/branding/default16.png $(DIST)/bin/chrome/icons/default
	$(INSTALL) $(DIST)/branding/default32.png $(DIST)/bin/chrome/icons/default
	$(INSTALL) $(DIST)/branding/default48.png $(DIST)/bin/chrome/icons/default
endif

export::
ifndef MOZ_BRANDING_DIRECTORY
	$(NSINSTALL) -D $(DIST)/branding
ifeq ($(OS_ARCH),WINNT)
	cp $(srcdir)/firefox.ico   $(DIST)/branding/firefox.ico
	cp $(srcdir)/firefox.ico   $(DIST)/branding/app.ico
	cp $(srcdir)/document.ico  $(DIST)/branding/document.ico
endif
ifneq (,$(filter mac cocoa,$(MOZ_WIDGET_TOOLKIT)))
	cp $(srcdir)/macbuild/firefox.icns  $(DIST)/branding/firefox.icns
	cp $(srcdir)/macbuild/document.icns  $(DIST)/branding/document.icns
	cp $(srcdir)/macbuild/dsstore $(DIST)/branding/dsstore
	cp $(srcdir)/macbuild/background.png $(DIST)/branding/background.png
	cp $(srcdir)/macbuild/disk.icns $(DIST)/branding/disk.icns
endif
ifneq (,$(filter gtk2,$(MOZ_WIDGET_TOOLKIT)))
	cp $(srcdir)/mozicon128.png $(DIST)/branding/mozicon128.png
	cp $(srcdir)/mozicon16.xpm $(DIST)/branding/mozicon16.xpm
	cp $(srcdir)/mozicon50.xpm $(DIST)/branding/mozicon50.xpm
	cp $(srcdir)/document.png $(DIST)/branding/document.png
endif
ifeq ($(MOZ_WIDGET_TOOLKIT),gtk2)
	cp $(srcdir)/default16.png   $(DIST)/branding/default16.png
	cp $(srcdir)/default32.png   $(DIST)/branding/default32.png
	cp $(srcdir)/default48.png   $(DIST)/branding/default48.png
endif
ifeq ($(OS_ARCH),OS2)
	cp $(srcdir)/firefox-os2.ico   $(DIST)/branding/firefox.ico
	cp $(srcdir)/firefox-os2.ico   $(DIST)/branding/app.ico
	cp $(srcdir)/document-os2.ico  $(DIST)/branding/document.ico
endif
endif

libs::
	touch $(DIST)/bin/.autoreg

libs:: $(srcdir)/profile/prefs.js
	$(INSTALL) $^ $(DIST)/bin/defaults/profile

libs:: $(srcdir)/blocklist.xml
	$(INSTALL) $^ $(DIST)/bin

ifneq (,$(filter mac cocoa,$(MOZ_WIDGET_TOOLKIT)))

APP_NAME = $(MOZ_APP_DISPLAYNAME)

ifdef MOZ_DEBUG
APP_NAME := $(APP_NAME)Debug
endif

AB_CD = $(MOZ_UI_LOCALE)

AB := $(firstword $(subst -, ,$(AB_CD)))

clean clobber repackage::
	rm -rf $(DIST)/$(APP_NAME).app

ifdef LIBXUL_SDK
APPFILES = Resources
else
APPFILES = MacOS
endif

libs repackage:: $(PROGRAM) application.ini
	mkdir -p $(DIST)/$(APP_NAME).app/Contents/MacOS
	rsync -a --exclude CVS --exclude "*.in" $(srcdir)/macbuild/Contents $(DIST)/$(APP_NAME).app --exclude English.lproj
	mkdir -p $(DIST)/$(APP_NAME).app/Contents/Resources/$(AB).lproj
	rsync -a --exclude CVS --exclude "*.in" $(srcdir)/macbuild/Contents/Resources/English.lproj/ $(DIST)/$(APP_NAME).app/Contents/Resources/$(AB).lproj
	sed -e "s/%APP_VERSION%/$(APP_VERSION)/" -e "s/%APP_NAME%/$(APP_NAME)/" $(srcdir)/macbuild/Contents/Info.plist.in > $(DIST)/$(APP_NAME).app/Contents/Info.plist
	sed -e "s/%APP_VERSION%/$(APP_VERSION)/" -e "s/%APP_NAME%/$(APP_NAME)/" $(srcdir)/macbuild/Contents/Resources/English.lproj/InfoPlist.strings.in | iconv -f UTF-8 -t UTF-16 > $(DIST)/$(APP_NAME).app/Contents/Resources/$(AB).lproj/InfoPlist.strings
	rsync -a $(DIST)/bin/ $(DIST)/$(APP_NAME).app/Contents/$(APPFILES)
	$(RM) $(DIST)/$(APP_NAME).app/Contents/$(APPFILES)/mangle $(DIST)/$(APP_NAME).app/Contents/$(APPFILES)/shlibsign
ifdef LIBXUL_SDK
	cp $(LIBXUL_DIST)/bin/$(XR_STUB_NAME) $(DIST)/$(APP_NAME).app/Contents/MacOS/firefox-bin
else
	rm -f $(DIST)/$(APP_NAME).app/Contents/MacOS/$(PROGRAM)
	rsync -aL $(PROGRAM) $(DIST)/$(APP_NAME).app/Contents/MacOS
endif
	mkdir -p $(DIST)/$(APP_NAME).app/Contents/Plug-Ins
	rsync -a --copy-unsafe-links $(LIBXUL_DIST)/package/PrintPDE.plugin $(DIST)/$(APP_NAME).app/Contents/Plug-Ins
	-cp -L $(DIST)/bin/mangle $(DIST)/bin/shlibsign $(DIST)/$(APP_NAME).app/Contents/$(APPFILES)
	cp -RL $(DIST)/branding/firefox.icns $(DIST)/$(APP_NAME).app/Contents/Resources/firefox.icns
	cp -RL $(DIST)/branding/document.icns $(DIST)/$(APP_NAME).app/Contents/Resources/document.icns
	cp -RL $(LIBXUL_DIST)/package/mozillaSuite.rsrc $(DIST)/$(APP_NAME).app/Contents/Resources/firefox-bin.rsrc
	echo -n APPLMOZB > $(DIST)/$(APP_NAME).app/Contents/PkgInfo
#       remove CVS dirs from packaged app
	find $(DIST)/$(APP_NAME).app -type d -name "CVS" -prune -exec rm -rf {} \;

else
ifdef LIBXUL_SDK
libs::
	cp $(LIBXUL_DIST)/bin/xulrunner-stub$(BIN_SUFFIX) $(DIST)/bin/firefox$(BIN_SUFFIX)
endif
endif

libs::
ifeq ($(OS_ARCH),WINNT)
	$(PERL) -pe 's/(?<!\r)\n/\r\n/g;' < $(topsrcdir)/LICENSE > $(DIST)/bin/LICENSE
else
	$(INSTALL) $(topsrcdir)/LICENSE	$(DIST)/bin
endif

ifdef LIBXUL_SDK
ifndef SKIP_COPY_XULRUNNER
libs::
ifeq (cocoa,$(MOZ_WIDGET_TOOLKIT))
	rsync -a --copy-unsafe-links $(LIBXUL_DIST)/XUL.framework $(DIST)/$(APP_NAME).app/Contents/Frameworks
else
	$(NSINSTALL) -D $(DIST)/bin/xulrunner
	(cd $(LIBXUL_SDK)/bin && tar $(TAR_CREATE_FLAGS) - .) | (cd $(DIST)/bin/xulrunner && tar -xf -)
endif # cocoa
endif # SKIP_COPY_XULRUNNER
endif # LIBXUL_SDK
