# SPDX-License-Identifier: (GPL-2.0 OR BSD-2-Clause)

TARGET = xdp-loader xdp-redirect
TARGET_CLEAN = $(addsuffix _clean,$(TARGET))

.PHONY: clean $(TARGET) $(TARGET_CLEAN)

all: $(TARGET)
clean: $(TARGET_CLEAN)

$(TARGET):
	$(MAKE) -C $@

$(TARGET_CLEAN):
	$(MAKE) -C $(subst _clean,,$@) clean
