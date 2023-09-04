include $(TOPDIR)/rules.mk

PKG_NAME:=tuya_cloud_daemon
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/tuya_cloud_daemon
	CATEGORY:=Base system
	TITLE:=tuya_cloud_daemon
	DEPENDS:=+USE_UCLIBC +argp-standalone +tuya-sdk +libubus +libubox +libblobmsg-json
endef

define Package/tuya_cloud_daemon/description
	This is the Tuya Cloud Daemon package
endef

define Package/tuya_cloud_daemon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuya_cloud_daemon $(1)/usr/bin
	$(INSTALL_BIN) ./files/tuya_cloud_daemon.init $(1)/etc/init.d/tuya_cloud_daemon
	$(INSTALL_CONF) ./files/tuya_cloud_daemon.config $(1)/etc/config/tuya_cloud_daemon
endef

$(eval $(call BuildPackage,tuya_cloud_daemon))