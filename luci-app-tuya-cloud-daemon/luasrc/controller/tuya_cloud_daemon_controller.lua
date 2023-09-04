module("luci.controller.tuya_cloud_daemon_controller", package.seeall)

function index()
    entry({"admin", "services", "tuya_cloud_daemon"}, cbi("tuya_cloud_daemon_model"), _("Tuya IoT"), 10)
end
