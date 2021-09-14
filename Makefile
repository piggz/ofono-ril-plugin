# -*- Mode: makefile-gmake -*-

.PHONY: clean all debug release test
.PHONY: debug_lib release_lib coverage_lib
.PHONY: print_debug_lib print_release_lib print_coverage_lib

#
# Required packages
#
# ofono.pc adds -export-symbols-regex linker option which doesn't  work
# on all platforms.
#

LDPKGS = libgrilio libmce-glib libglibutil gobject-2.0 glib-2.0
PKGS = ofono $(LDPKGS)

#
# Default target
#

all: debug release

#
# Library name
#

NAME = rilplugin
LIB_NAME = $(NAME)
LIB_SONAME = $(LIB_NAME).so
LIB = $(LIB_SONAME)
STATIC_LIB = $(NAME).a

#
# Sources
#

SRC = \
  ril_call_barring.c \
  ril_call_forward.c \
  ril_call_settings.c \
  ril_call_volume.c \
  ril_cbs.c \
  ril_cell_info.c \
  ril_config.c \
  ril_connman.c \
  ril_data.c \
  ril_devinfo.c \
  ril_devmon_auto.c \
  ril_devmon.c \
  ril_devmon_combine.c \
  ril_devmon_ds.c \
  ril_devmon_ss.c \
  ril_devmon_ur.c \
  ril_ecclist.c \
  ril_gprs.c \
  ril_gprs_context.c \
  ril_modem.c \
  ril_netmon.c \
  ril_netreg.c \
  ril_network.c \
  ril_oem_raw.c \
  ril_phonebook.c \
  ril_plugin.c \
  ril_radio.c \
  ril_radio_caps.c \
  ril_radio_settings.c \
  ril_sim.c \
  ril_sim_card.c \
  ril_sim_settings.c \
  ril_sms.c \
  ril_stk.c \
  ril_ussd.c \
  ril_util.c \
  ril_vendor.c \
  ril_vendor_mtk.c \
  ril_voicecall.c

#
# Directories
#

SRC_DIR = src
BUILD_DIR = build
DEBUG_BUILD_DIR = $(BUILD_DIR)/debug
RELEASE_BUILD_DIR = $(BUILD_DIR)/release
COVERAGE_BUILD_DIR = $(BUILD_DIR)/coverage

#
# Tools and flags
#

CC = $(CROSS_COMPILE)gcc
LD = $(CC)
WARNINGS = -Wall
BASE_FLAGS = -fPIC -fvisibility=hidden
FULL_CFLAGS = $(BASE_FLAGS) $(CFLAGS) $(DEFINES) $(WARNINGS) -MMD -MP \
  $(shell pkg-config --cflags $(PKGS))
FULL_LDFLAGS = $(BASE_FLAGS) $(LDFLAGS) -shared \
  $(shell pkg-config --libs $(LDPKGS))
DEBUG_FLAGS = -g
RELEASE_FLAGS =
COVERAGE_FLAGS = -g

KEEP_SYMBOLS ?= 0
ifneq ($(KEEP_SYMBOLS),0)
RELEASE_FLAGS += -g
endif

DEBUG_LDFLAGS = $(FULL_LDFLAGS) $(DEBUG_FLAGS)
RELEASE_LDFLAGS = $(FULL_LDFLAGS) $(RELEASE_FLAGS)
DEBUG_CFLAGS = $(FULL_CFLAGS) $(DEBUG_FLAGS) -DDEBUG
RELEASE_CFLAGS = $(FULL_CFLAGS) $(RELEASE_FLAGS) -O2
COVERAGE_CFLAGS = $(FULL_CFLAGS) $(COVERAGE_FLAGS) --coverage

#
# Files
#

DEBUG_OBJS = $(SRC:%.c=$(DEBUG_BUILD_DIR)/%.o)
RELEASE_OBJS = $(SRC:%.c=$(RELEASE_BUILD_DIR)/%.o)
COVERAGE_OBJS = $(SRC:%.c=$(COVERAGE_BUILD_DIR)/%.o)

DEBUG_SO = $(DEBUG_BUILD_DIR)/$(LIB)
RELEASE_SO = $(RELEASE_BUILD_DIR)/$(LIB)

DEBUG_LIB = $(DEBUG_BUILD_DIR)/$(STATIC_LIB)
RELEASE_LIB = $(RELEASE_BUILD_DIR)/$(STATIC_LIB)
COVERAGE_LIB = $(COVERAGE_BUILD_DIR)/$(STATIC_LIB)

#
# Dependencies
#

DEPS = \
  $(DEBUG_OBJS:%.o=%.d) \
  $(RELEASE_OBJS:%.o=%.d)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEPS)),)
-include $(DEPS)
endif
endif

$(DEBUG_OBJS) $(DEBUG_SO): | $(DEBUG_BUILD_DIR)
$(RELEASE_OBJS) $(RELEASE_SO): | $(RELEASE_BUILD_DIR)
$(COVERAGE_OBJS) $(COVERAGE_LIB): | $(COVERAGE_BUILD_DIR)

#
# Rules
#

debug: $(DEBUG_SO)

release: $(RELEASE_SO)

debug_lib: $(DEBUG_LIB)

release_lib: $(RELEASE_LIB)

coverage_lib: $(COVERAGE_LIB)

print_debug_lib:
	@echo $(DEBUG_LIB)

print_release_lib:
	@echo $(RELEASE_LIB)

print_coverage_lib:
	@echo $(COVERAGE_LIB)

clean:
	make -C unit clean
	rm -f *~ $(SRC_DIR)/*~ rpm/*~
	rm -fr $(BUILD_DIR)

test:
	make -C unit test

$(DEBUG_BUILD_DIR):
	mkdir -p $@

$(RELEASE_BUILD_DIR):
	mkdir -p $@

$(COVERAGE_BUILD_DIR):
	mkdir -p $@

$(DEBUG_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(DEBUG_CFLAGS) -MT"$@" -MF"$(@:%.o=%.d)" $< -o $@

$(RELEASE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(RELEASE_CFLAGS) -MT"$@" -MF"$(@:%.o=%.d)" $< -o $@

$(COVERAGE_BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c $(COVERAGE_CFLAGS) -MT"$@" -MF"$(@:%.o=%.d)" $< -o $@

$(DEBUG_SO): $(DEBUG_OBJS)
	$(LD) $(DEBUG_OBJS) $(DEBUG_LDFLAGS) -o $@

$(RELEASE_SO): $(RELEASE_OBJS)
	$(LD) $(RELEASE_OBJS) $(RELEASE_LDFLAGS) -o $@
ifeq ($(KEEP_SYMBOLS),0)
	$(STRIP) $@
endif

$(DEBUG_LIB): $(DEBUG_OBJS)
	$(AR) rc $@ $?
	ranlib $@

$(RELEASE_LIB): $(RELEASE_OBJS)
	$(AR) rc $@ $?
	ranlib $@

$(COVERAGE_LIB): $(COVERAGE_OBJS)
	$(AR) rc $@ $?
	ranlib $@

#
# Install
#

PLUGINDIR ?= usr/lib/ofono/plugins
ABS_PLUGINDIR := $(shell echo /$(PLUGINDIR) | sed -r 's|/+|/|g')

INSTALL = install
INSTALL_PLUGIN_DIR = $(DESTDIR)$(ABS_PLUGINDIR)

install: $(INSTALL_PLUGIN_DIR)
	$(INSTALL) -m 755 $(RELEASE_SO) $(INSTALL_PLUGIN_DIR)

$(INSTALL_PLUGIN_DIR):
	$(INSTALL) -d $@
