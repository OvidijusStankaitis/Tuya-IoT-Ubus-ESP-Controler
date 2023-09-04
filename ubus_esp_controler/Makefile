include $(TOPDIR)/rules.mk

PKG_NAME:=ubus_esp_controler
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/ubus_esp_controler
	CATEGORY:=Base system
	TITLE:=ubus_esp_controler
	DEPENDS:=+libubus +libubox +libblobmsg-json +libserialport
endef

define Package/ubus_esp_controler/description
	This is the Ubus Esp Controler package
endef

define Package/ubus_esp_controler/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/rc.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ubus_esp_controler $(1)/usr/bin
	$(INSTALL_BIN) ./scripts/startup.init $(1)/etc/init.d/ubus_esp_controler
	$(LN) $(1)/etc/init.d/ubus_esp_controler $(1)/etc/rc.d/S99ubus_esp_controler
endef


$(eval $(call BuildPackage,ubus_esp_controler))