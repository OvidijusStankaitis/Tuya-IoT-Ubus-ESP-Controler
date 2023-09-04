map = Map("tuya_cloud_daemon", "Tuya IoT")

enableSection = map:section(NamedSection, "tuya_cloud_daemon_sct", "tuya_cloud_daemon", "Enable program")
flag = enableSection:option(Flag, "enable", "Enable", "Enable Tuya Cloud Daemon")
flag.default = 0
flag.optional = false

dataSection = map:section(NamedSection, "tuya_cloud_daemon_sct", " tuya_cloud_daemon", "Device info")
product = dataSection:option(Value, "product_id", "Product ID")
device = dataSection:option(Value, "device_id", "Device ID")
secret = dataSection:option(Value, "device_secret", "Device Secret")

product.size = 30
product.maxlength = 30
product.optional = false

device.size = 30
device.maxlength = 30
device.optional = false

secret.size = 30
secret.maxlength = 30
secret.optional = false

return map