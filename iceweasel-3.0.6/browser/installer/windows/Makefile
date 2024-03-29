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
# The Original Code is the Mozilla Browser code.
#
# The Initial Developer of the Original Code is
# IBM Corporation.
# Portions created by the Initial Developer are Copyright (C) 2004
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#  Brian Ryner <bryner@brianryner.com>
#  Chase Phillips <cmp@mozilla.org>
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

DEPTH		= ../../..
topsrcdir	= ../../..
srcdir		= .
VPATH		= .

include $(DEPTH)/config/autoconf.mk

include $(topsrcdir)/toolkit/mozapps/installer/package-name.mk

CONFIG_DIR = instgen
SFX_MODULE = $(topsrcdir)/other-licenses/7zstub/firefox/7zSD.sfx

PP_LOCALIZED_FILES = \
	packages-static \
	$(NULL)

INSTALLER_FILES = \
	app.tag \
	nsis/installer.nsi \
	nsis/uninstaller.nsi \
	nsis/shared.nsh \
	$(NULL)

BRANDING_FILES = \
	branding.nsi \
	wizHeader.bmp \
	wizHeaderRTL.bmp \
	wizWatermark.bmp \
	$(NULL)

DEFINES += \
	-DAB_CD=$(AB_CD) \
	-DPKG_BASENAME=$(PKG_BASENAME) \
	-DMOZ_APP_VERSION=$(MOZ_APP_VERSION) \
	-DMOZ_APP_NAME=$(MOZ_APP_NAME) \
	-DMOZ_APP_DISPLAYNAME=${MOZ_APP_DISPLAYNAME} \
	-DMOZILLA_VERSION=${MOZILLA_VERSION} \
	$(NULL)

include $(topsrcdir)/config/config.mk
include $(call EXPAND_LOCALE_SRCDIR,toolkit/locales)/installer/windows/charset.mk

installer::
	$(MAKE) -C .. installer-stage
	$(MAKE) $(CONFIG_DIR)/setup.exe

# For building the uninstaller during the application build so it can be
# included for mar file generation.
uninstaller::
	$(RM) -rf $(CONFIG_DIR) && mkdir $(CONFIG_DIR)
	$(INSTALL) $(addprefix $(srcdir)/,$(INSTALLER_FILES)) $(CONFIG_DIR)
	$(INSTALL) $(addprefix $(DIST)/branding/,$(BRANDING_FILES)) $(CONFIG_DIR)
	$(EXIT_ON_ERROR) \
	for i in $(PP_LOCALIZED_FILES); do \
	  $(PERL) $(topsrcdir)/config/preprocessor.pl $(DEFINES) $(ACDEFINES) $(srcdir)/$$i > $(CONFIG_DIR)/$$i; \
	done
	$(PERL) $(topsrcdir)/config/preprocessor.pl -Fsubstitution $(DEFINES) $(ACDEFINES) \
	  $(srcdir)/nsis/defines.nsi.in > $(CONFIG_DIR)/defines.nsi
	$(PERL) $(topsrcdir)/toolkit/mozapps/installer/windows/nsis/preprocess-locale.pl \
	  $(topsrcdir) $(call EXPAND_LOCALE_SRCDIR,browser/locales)/installer $(AB_CD) \
	  $(WIN_INSTALLER_CHARSET) $(CONFIG_DIR)

$(CONFIG_DIR)/setup.exe::
	$(RM) -rf $(CONFIG_DIR) && mkdir $(CONFIG_DIR)
	$(INSTALL) $(addprefix $(srcdir)/,$(INSTALLER_FILES)) $(CONFIG_DIR)
	$(INSTALL) $(addprefix $(DIST)/branding/,$(BRANDING_FILES)) $(CONFIG_DIR)
	$(EXIT_ON_ERROR) \
	for i in $(PP_LOCALIZED_FILES); do \
	  $(PERL) $(topsrcdir)/config/preprocessor.pl $(DEFINES) $(ACDEFINES) $(srcdir)/$$i > $(CONFIG_DIR)/$$i; \
	done
	$(PERL) $(topsrcdir)/toolkit/mozapps/installer/windows/nsis/make-installremoves.pl \
	  ../removed-files > $(CONFIG_DIR)/removed-files.log
	$(PERL) $(topsrcdir)/config/preprocessor.pl -Fsubstitution $(DEFINES) $(ACDEFINES) \
	  $(srcdir)/nsis/defines.nsi.in > $(CONFIG_DIR)/defines.nsi
	$(PERL) $(topsrcdir)/toolkit/mozapps/installer/windows/nsis/preprocess-locale.pl \
	  $(topsrcdir) $(call EXPAND_LOCALE_SRCDIR,browser/locales)/installer $(AB_CD) \
	  $(WIN_INSTALLER_CHARSET) $(CONFIG_DIR)

include $(topsrcdir)/config/rules.mk
include $(topsrcdir)/toolkit/mozapps/installer/windows/nsis/makensis.mk

export::
ifndef MOZ_BRANDING_DIRECTORY
	$(NSINSTALL) -D $(DIST)/branding
	cp $(srcdir)/nsis/branding.nsi $(DIST)/branding/branding.nsi
	cp $(srcdir)/wizHeader.bmp     $(DIST)/branding/wizHeader.bmp
	cp $(srcdir)/wizHeaderRTL.bmp  $(DIST)/branding/wizHeaderRTL.bmp
	cp $(srcdir)/wizWatermark.bmp  $(DIST)/branding/wizWatermark.bmp
endif
