include $(TOPDIR)/rules.mk

PKG_NAME:=mgl03-boot-ctl
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mgl03-boot-ctl
SECTION:=utils
CATEGORY:=Utilities
TITLE:=MGL03 dual-boot configuration utility
DEPENDS:=+jansson
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/mgl03-mux-ctl/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mgl03_boot_ctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,mgl03-boot-ctl))
