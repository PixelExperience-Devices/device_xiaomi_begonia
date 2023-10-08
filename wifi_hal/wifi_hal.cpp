#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <linux/errqueue.h>
#include <errno.h>
#include <string.h>

#include <linux/pkt_sched.h>
#include <netlink/object-api.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/attr.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include <dirent.h>
#include <net/if.h>

#include "sync.h"

#define LOG_TAG  "WifiHAL"

#include <utils/Log.h>

#include "wifi_hal.h"
#include "common.h"
#include "cpp_bindings.h"
#include "cutils/properties.h"

template<typename>
struct DummyFunction;

template<typename R, typename... Args>
struct DummyFunction<R (*)(Args...)> {
    static constexpr R invoke(Args...) { return WIFI_SUCCESS; }
};

template<typename... Args>
struct DummyFunction<void (*)(Args...)> {
    static constexpr void invoke(Args...) { }
};

template<typename T>
void populateDummyFor(T& val) {
    val = &DummyFunction<T>::invoke;
}

/*
 BUGBUG: normally, libnl allocates ports for all connections it makes; but
 being a static library, it doesn't really know how many other netlink connections
 are made by the same process, if connections come from different shared libraries.
 These port assignments exist to solve that problem - temporarily. We need to fix
 libnl to try and allocate ports across the entire process.
 */

#define WIFI_HAL_CMD_SOCK_PORT       644
#define WIFI_HAL_EVENT_SOCK_PORT     645

static int internal_no_seq_check(nl_msg *msg, void *arg);
static int internal_valid_message_handler(nl_msg *msg, void *arg);
static int wifi_get_multicast_id(wifi_handle handle, const char *name, const char *group);
static int wifi_add_membership(wifi_handle handle, const char *group);
static wifi_error wifi_init_interfaces(wifi_handle handle);
static wifi_error wifi_start_rssi_monitoring(wifi_request_id id, wifi_interface_handle
                        iface, s8 max_rssi, s8 min_rssi, wifi_rssi_event_handler eh);
static wifi_error wifi_stop_rssi_monitoring(wifi_request_id id, wifi_interface_handle iface);

static wifi_error wifi_get_wake_reason_stats_dummy(wifi_interface_handle iface,
    WLAN_DRIVER_WAKE_REASON_CNT *wifi_wake_reason_cnt);
static wifi_error wifi_get_packet_filter_capabilities_dummy(
    wifi_interface_handle handle, u32 *version, u32 *max_len);
static wifi_error wifi_get_ring_buffers_status_dummy(wifi_interface_handle iface,
    u32 *num_rings, wifi_ring_buffer_status *status);
static wifi_error wifi_get_logger_supported_feature_set_dummy(wifi_interface_handle iface,
    unsigned int *support);
static wifi_error wifi_get_tx_pkt_fates_dummy(wifi_interface_handle handle,
    wifi_tx_report *tx_report_bufs, size_t n_requested_fates, size_t *n_provided_fates);
static wifi_error wifi_get_rx_pkt_fates_dummy(wifi_interface_handle handle,
    wifi_rx_report *rx_report_bufs, size_t n_requested_fates, size_t *n_provided_fates);

/* Initialize/Cleanup */

void wifi_socket_set_local_port(struct nl_sock *sock, uint32_t port)
{
    uint32_t pid = getpid() & 0x3FFFFF;
    nl_socket_set_local_port(sock, pid + (port << 22));
}

static nl_sock *wifi_create_nl_socket(int port)
{

    // ALOGD("Creating netlink socket, local port[%d]", port);
    struct nl_sock *sock = nl_socket_alloc();
    if (sock == NULL) {
        ALOGE("Could not create netlink socket: %s(%d)", strerror(errno), errno);
        return NULL;
    }

    wifi_socket_set_local_port(sock, port);

    // ALOGD("Connecting to socket");
    if (nl_connect(sock, NETLINK_GENERIC)) {
        ALOGE("Could not connect to netlink socket: %s(%d)", strerror(errno), errno);
        nl_socket_free(sock);
        return NULL;
    }

    return sock;
}
/* Initialize vendor function pointer table with MTK HAL API */
wifi_error init_wifi_vendor_hal_func_table(wifi_hal_fn *fn)
{
    if (fn == NULL) {
        return WIFI_ERROR_UNKNOWN;
    }
    fn->wifi_initialize = wifi_initialize;
    fn->wifi_cleanup = wifi_cleanup;
    fn->wifi_event_loop = wifi_event_loop;
    fn->wifi_get_supported_feature_set = wifi_get_supported_feature_set;
    fn->wifi_get_ifaces = wifi_get_ifaces;
    fn->wifi_get_iface_name = wifi_get_iface_name;
    fn->wifi_set_country_code = wifi_set_country_code;
    fn->wifi_get_firmware_version = wifi_get_firmware_version;
    fn->wifi_get_driver_version = wifi_get_driver_version;
    fn->wifi_start_rssi_monitoring = wifi_start_rssi_monitoring;
    fn->wifi_stop_rssi_monitoring = wifi_stop_rssi_monitoring;
    fn->wifi_start_sending_offloaded_packet = wifi_start_sending_offloaded_packet;
    fn->wifi_stop_sending_offloaded_packet = wifi_stop_sending_offloaded_packet;
    fn->wifi_get_roaming_capabilities = wifi_get_roaming_capabilities;
    fn->wifi_configure_roaming = wifi_configure_roaming;
    fn->wifi_enable_firmware_roaming = wifi_enable_firmware_roaming;
    fn->wifi_select_tx_power_scenario = wifi_select_tx_power_scenario;
    fn->wifi_reset_tx_power_scenario = wifi_reset_tx_power_scenario;
    fn->wifi_set_scanning_mac_oui = wifi_set_scanning_mac_oui;
    fn->wifi_get_valid_channels = wifi_get_valid_channels;

    fn->wifi_get_ring_buffers_status = wifi_get_ring_buffers_status_dummy;
    fn->wifi_get_logger_supported_feature_set = wifi_get_logger_supported_feature_set_dummy;
    fn->wifi_get_tx_pkt_fates = wifi_get_tx_pkt_fates_dummy;
    fn->wifi_get_rx_pkt_fates = wifi_get_rx_pkt_fates_dummy;
    fn->wifi_get_packet_filter_capabilities = wifi_get_packet_filter_capabilities_dummy;
    fn->wifi_get_wake_reason_stats = wifi_get_wake_reason_stats_dummy;
    fn->wifi_get_driver_memory_dump = wifi_get_driver_memory_dump;

    populateDummyFor(fn->wifi_wait_for_driver_ready);
    populateDummyFor(fn->wifi_set_nodfs_flag);
    populateDummyFor(fn->wifi_set_log_handler);
    populateDummyFor(fn->wifi_reset_log_handler);
    populateDummyFor(fn->wifi_configure_nd_offload);
    populateDummyFor(fn->wifi_start_pkt_fate_monitoring);
    populateDummyFor(fn->wifi_get_ring_data);
    populateDummyFor(fn->wifi_start_logging);
    populateDummyFor(fn->wifi_get_gscan_capabilities);

    return WIFI_SUCCESS;
}

wifi_error wifi_initialize(wifi_handle *handle)
{
    ALOGI("Wifi HAL initializing");

    hal_info *info = (hal_info *)malloc(sizeof(hal_info));
    if (info == NULL) {
        ALOGE("Could not allocate hal_info");
        return WIFI_ERROR_OUT_OF_MEMORY;
    }

    memset(info, 0, sizeof(*info));

    ALOGI("Creating socket");
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, info->cleanup_socks) < 0) {
        ALOGE("Could not create cleanup sockets");
        free(info);
        return WIFI_ERROR_UNKNOWN;
    }

    struct nl_sock *cmd_sock = wifi_create_nl_socket(WIFI_HAL_CMD_SOCK_PORT);
    if (cmd_sock == NULL) {
        ALOGE("Could not create handle");
        free(info);
        return WIFI_ERROR_UNKNOWN;
    }

    struct nl_sock *event_sock = wifi_create_nl_socket(WIFI_HAL_EVENT_SOCK_PORT);
    if (event_sock == NULL) {
        ALOGE("Could not create handle");
        nl_socket_free(cmd_sock);
        free(info);
        return WIFI_ERROR_UNKNOWN;
    }

    struct nl_cb *cb = nl_socket_get_cb(event_sock);
    if (cb == NULL) {
        ALOGE("Could not create handle");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info);
        return WIFI_ERROR_UNKNOWN;
    }

    // ALOGI("cb->refcnt = %d", cb->cb_refcnt);
    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, internal_no_seq_check, info);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, internal_valid_message_handler, info);
    nl_cb_put(cb);

    info->cmd_sock = cmd_sock;
    info->event_sock = event_sock;
    info->clean_up = false;
    info->in_event_loop = false;

    info->event_cb = (cb_info *)malloc(sizeof(cb_info) * DEFAULT_EVENT_CB_SIZE);
    info->alloc_event_cb = DEFAULT_EVENT_CB_SIZE;
    info->num_event_cb = 0;
    if (info->event_cb == NULL) {
        ALOGE("Could not allocate cb_info array");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info);
        return WIFI_ERROR_OUT_OF_MEMORY;
    }

    info->cmd = (cmd_info *)malloc(sizeof(cmd_info) * DEFAULT_CMD_SIZE);
    info->alloc_cmd = DEFAULT_CMD_SIZE;
    info->num_cmd = 0;
    if (info->cmd == NULL) {
        ALOGE("Could not allocate cmd_info array");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info->event_cb);
        free(info);
        return WIFI_ERROR_OUT_OF_MEMORY;
    }

    ALOGI("Get nl80211_family_id");
    info->nl80211_family_id = genl_ctrl_resolve(cmd_sock, "nl80211");
    if (info->nl80211_family_id < 0) {
        ALOGE("Could not resolve nl80211 familty id");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info->event_cb);
        free(info->cmd);
        free(info);
        return WIFI_ERROR_UNKNOWN;
    }

    pthread_mutex_init(&info->cb_lock, NULL);

    if (wifi_init_interfaces((wifi_handle) info) != WIFI_SUCCESS) {
        ALOGE("No wifi interface found");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info->event_cb);
        free(info->cmd);
        pthread_mutex_destroy(&info->cb_lock);
        free(info);
        return WIFI_ERROR_NOT_AVAILABLE;
    }

    if ((wifi_add_membership((wifi_handle) info, "scan") < 0) ||
        (wifi_add_membership((wifi_handle) info, "mlme")  < 0) ||
        (wifi_add_membership((wifi_handle) info, "regulatory") < 0) ||
        (wifi_add_membership((wifi_handle) info, "vendor") < 0)) {
        ALOGE("Add membership failed");
        nl_socket_free(cmd_sock);
        nl_socket_free(event_sock);
        free(info->event_cb);
        free(info->cmd);
        pthread_mutex_destroy(&info->cb_lock);
        free(info);
        return WIFI_ERROR_NOT_AVAILABLE;
    }

    *handle = (wifi_handle) info;

    ALOGD("Wifi HAL initialized successfully: nl80211_family_id=%d, found %d interfaces",
        info->nl80211_family_id, info->num_interfaces);
    return WIFI_SUCCESS;
}

static int wifi_add_membership(wifi_handle handle, const char *group)
{
    hal_info *info = getHalInfo(handle);

    int id = wifi_get_multicast_id(handle, "nl80211", group);
    if (id < 0) {
        ALOGE("Could not find group %s", group);
        return id;
    }

    int ret = nl_socket_add_membership(info->event_sock, id);
    if (ret < 0) {
        ALOGE("Could not add membership to group %s", group);
    }

    ALOGD("Add membership for group %s successfully", group);
    return ret;
}

static void internal_cleaned_up_handler(wifi_handle handle)
{
    hal_info *info = getHalInfo(handle);
    wifi_cleaned_up_handler cleaned_up_handler = info->cleaned_up_handler;

    if (info->cmd_sock != 0) {
        close(info->cleanup_socks[0]);
        close(info->cleanup_socks[1]);
        nl_socket_free(info->cmd_sock);
        nl_socket_free(info->event_sock);
        info->cmd_sock = NULL;
        info->event_sock = NULL;
    }

    if (info->interfaces) {
        int i = 0;
        for (; i < info->num_interfaces; i++)
            free(info->interfaces[i]);
        free(info->interfaces);
    }

    (*cleaned_up_handler)(handle);
    pthread_mutex_destroy(&info->cb_lock);
    free(info);

    ALOGI("Internal cleanup completed");
}

void wifi_cleanup(wifi_handle handle, wifi_cleaned_up_handler handler)
{
    hal_info *info = getHalInfo(handle);
    char buf[64];

    info->cleaned_up_handler = handler;
    if (TEMP_FAILURE_RETRY(write(info->cleanup_socks[0], "Exit", 4)) < 1) {
        // As a fallback set the cleanup flag to TRUE
        ALOGE("could not write to the cleanup socket");
    } else {
        // Listen to the response
        // Hopefully we dont get errors or get hung up
        // Not much can be done in that case, but assume that
        // it has rx'ed the Exit message to exit the thread.
        // As a fallback set the cleanup flag to TRUE
        memset(buf, 0, sizeof(buf));
        ssize_t result = TEMP_FAILURE_RETRY(read(info->cleanup_socks[0], buf, sizeof(buf)));
        ALOGE("%s: Read after POLL returned %zd, error no = %d (%s)", __FUNCTION__,
               result, errno, strerror(errno));
        if (strncmp(buf, "Done", 4) == 0) {
            ALOGE("Event processing terminated");
        } else {
            ALOGD("Rx'ed %s", buf);
        }
    }
    info->clean_up = true;
    pthread_mutex_lock(&info->cb_lock);

    int bad_commands = 0;

    for (int i = 0; i < info->num_event_cb; i++) {
        cb_info *cbi = &(info->event_cb[i]);
        WifiCommand *cmd = (WifiCommand *)cbi->cb_arg;
        ALOGI("Command left in event_cb %p:%s", cmd, (cmd ? cmd->getType(): ""));
    }

    while (info->num_cmd > bad_commands) {
        int num_cmd = info->num_cmd;
        cmd_info *cmdi = &(info->cmd[bad_commands]);
        WifiCommand *cmd = cmdi->cmd;
        if (cmd != NULL) {
            ALOGI("Cancelling command %p:%s", cmd, cmd->getType());
            pthread_mutex_unlock(&info->cb_lock);
            cmd->cancel();
            pthread_mutex_lock(&info->cb_lock);
            if (num_cmd == info->num_cmd) {
                ALOGI("Cancelling command %p:%s did not work", cmd, (cmd ? cmd->getType(): ""));
                bad_commands++;
            }
            /* release reference added when command is saved */
            cmd->releaseRef();
        }
    }

    for (int i = 0; i < info->num_event_cb; i++) {
        cb_info *cbi = &(info->event_cb[i]);
        WifiCommand *cmd = (WifiCommand *)cbi->cb_arg;
        ALOGE("Leaked command %p", cmd);
    }
    pthread_mutex_unlock(&info->cb_lock);
    internal_cleaned_up_handler(handle);
}

static int internal_pollin_handler(wifi_handle handle)
{
    hal_info *info = getHalInfo(handle);
    struct nl_cb *cb = nl_socket_get_cb(info->event_sock);
    int res = nl_recvmsgs(info->event_sock, cb);
    // ALOGD("nl_recvmsgs returned %d", res);
    nl_cb_put(cb);
    return res;
}

/* Run event handler */
void wifi_event_loop(wifi_handle handle)
{
    hal_info *info = getHalInfo(handle);
    if (info->in_event_loop) {
        return;
    } else {
        info->in_event_loop = true;
    }

    pollfd pfd[2];
    memset(&pfd[0], 0, sizeof(pollfd) * 2);

    pfd[0].fd = nl_socket_get_fd(info->event_sock);
    pfd[0].events = POLLIN;
    pfd[1].fd = info->cleanup_socks[1];
    pfd[1].events = POLLIN;

    char buf[2048];
    /* TODO: Add support for timeouts */

    do {
        int timeout = -1;                   /* Infinite timeout */
        pfd[0].revents = 0;
        pfd[1].revents = 0;
        // ALOGI("Polling socket");
        int result = TEMP_FAILURE_RETRY(poll(pfd, 2, timeout));
        if (result < 0) {
            // ALOGE("Error polling socket");
        } else if (pfd[0].revents & POLLERR) {
            ALOGE("POLL Error; error no = %d (%s)", errno, strerror(errno));
            ssize_t result2 = TEMP_FAILURE_RETRY(read(pfd[0].fd, buf, sizeof(buf)));
            ALOGE("Read after POLL returned %zd, error no = %d (%s)", result2,
                  errno, strerror(errno));
        } else if (pfd[0].revents & POLLHUP) {
            ALOGE("Remote side hung up");
            break;
        } else if (pfd[0].revents & POLLIN) {
            // ALOGI("Found some events!!!");
            internal_pollin_handler(handle);
        } else if (pfd[1].revents & POLLIN) {
            memset(buf, 0, sizeof(buf));
            ssize_t result2 = TEMP_FAILURE_RETRY(read(pfd[1].fd, buf, sizeof(buf)));
            ALOGE("%s: Read after POLL returned %zd, error no = %d (%s)", __FUNCTION__,
                   result2, errno, strerror(errno));
            if (strncmp(buf, "Exit", 4) == 0) {
                ALOGD("Got a signal to exit!!!");
                if (TEMP_FAILURE_RETRY(write(pfd[1].fd, "Done", 4)) < 1) {
                    ALOGE("could not write to the cleanup socket");
                }
                break;
            } else {
                ALOGD("Rx'ed %s on the cleanup socket\n", buf);
            }
        } else {
            ALOGE("Unknown event - %0x, %0x", pfd[0].revents, pfd[1].revents);
        }
    } while (!info->clean_up);
    ALOGI("Exit %s", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////////////
static int internal_no_seq_check(struct nl_msg *msg, void *arg)
{
    return NL_OK;
}

static int internal_valid_message_handler(nl_msg *msg, void *arg)
{
    wifi_handle handle = (wifi_handle)arg;
    hal_info *info = getHalInfo(handle);

    WifiEvent event(msg);
    int res = event.parse();
    if (res < 0) {
        ALOGE("Failed to parse event: %d", res);
        return NL_SKIP;
    }

    int cmd = event.get_cmd();
    uint32_t vendor_id = 0;
    int subcmd = 0;

    if (cmd == NL80211_CMD_VENDOR) {
        vendor_id = event.get_u32(NL80211_ATTR_VENDOR_ID);
        subcmd = event.get_u32(NL80211_ATTR_VENDOR_SUBCMD);
        ALOGV("event received %s, vendor_id = 0x%0x, subcmd = 0x%0x",
                event.get_cmdString(), vendor_id, subcmd);
    } else {
        ALOGV("event received %s", event.get_cmdString());
    }

    pthread_mutex_lock(&info->cb_lock);

    for (int i = 0; i < info->num_event_cb; i++) {
        if (cmd == info->event_cb[i].nl_cmd) {
            if (cmd == NL80211_CMD_VENDOR
                && ((vendor_id != info->event_cb[i].vendor_id)
                || (subcmd != info->event_cb[i].vendor_subcmd)))
            {
                /* event for a different vendor, ignore it */
                continue;
            }

            cb_info *cbi = &(info->event_cb[i]);
            nl_recvmsg_msg_cb_t cb_func = cbi->cb_func;
            void *cb_arg = cbi->cb_arg;
            WifiCommand *cmd = (WifiCommand *)cbi->cb_arg;
            if (cmd != NULL) {
                cmd->addRef();
            }

            pthread_mutex_unlock(&info->cb_lock);
            if (cb_func)
                (*cb_func)(msg, cb_arg);
            if (cmd != NULL) {
                cmd->releaseRef();
            }

            return NL_OK;
        }
    }

    pthread_mutex_unlock(&info->cb_lock);
    return NL_OK;
}

///////////////////////////////////////////////////////////////////////////////////

class GetMulticastIdCommand : public WifiCommand
{
private:
    const char *mName;
    const char *mGroup;
    int   mId;

public:
    GetMulticastIdCommand(wifi_handle handle, const char *name, const char *group)
        : WifiCommand("GetMulticastIdCommand", handle, 0)
    {
        mName = name;
        mGroup = group;
        mId = -1;
    }

    int getId() {
        return mId;
    }

    virtual int create() {
        int nlctrlFamily = genl_ctrl_resolve(mInfo->cmd_sock, "nlctrl");
        // ALOGD("ctrl family = %d", nlctrlFamily);
        int ret = mMsg.create(nlctrlFamily, CTRL_CMD_GETFAMILY, 0, 0);
        if (ret < 0) {
            return ret;
        }
        ret = mMsg.put_string(CTRL_ATTR_FAMILY_NAME, mName);
        return ret;
    }

    virtual int handleResponse(WifiEvent& reply) {

        // ALOGD("handling reponse in %s", __func__);

        struct nlattr **tb = reply.attributes();
        struct nlattr *mcgrp = NULL;
        int i;

        if (!tb[CTRL_ATTR_MCAST_GROUPS]) {
            ALOGD("No multicast groups found");
            return NL_SKIP;
        } else {
            // ALOGD("Multicast groups attr size = %d", nla_len(tb[CTRL_ATTR_MCAST_GROUPS]));
        }

        for_each_attr(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], i) {

            // ALOGD("Processing group");
            struct nlattr *tb2[CTRL_ATTR_MCAST_GRP_MAX + 1];
            nla_parse(tb2, CTRL_ATTR_MCAST_GRP_MAX, (nlattr *)nla_data(mcgrp),
                nla_len(mcgrp), NULL);
            if (!tb2[CTRL_ATTR_MCAST_GRP_NAME] || !tb2[CTRL_ATTR_MCAST_GRP_ID]) {
                continue;
            }

            char *grpName = (char *)nla_data(tb2[CTRL_ATTR_MCAST_GRP_NAME]);
            int grpNameLen = nla_len(tb2[CTRL_ATTR_MCAST_GRP_NAME]);

            if (!grpName || grpNameLen == 0) {
                continue;
            }

            if (strncmp(grpName, mGroup, grpNameLen) != 0)
                continue;

            mId = nla_get_u32(tb2[CTRL_ATTR_MCAST_GRP_ID]);
            break;
        }

        return NL_SKIP;
    }

};

class SetCountryCodeCommand : public WifiCommand {
private:
    const char *mCountryCode;
public:
    SetCountryCodeCommand(wifi_interface_handle handle, const char *country_code)
        : WifiCommand("SetCountryCodeCommand", handle, 0) {
        mCountryCode = country_code;
        }
    virtual int create() {
        int ret;

        ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_SET_COUNTRY_CODE);
        if (ret < 0) {
             ALOGE("Can't create message to send to driver - %d", ret);
             return ret;
        }

        nlattr *data = mMsg.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        ret = mMsg.put_string(WIFI_ATTRIBUTE_COUNTRY_CODE, mCountryCode);
        if (ret < 0) {
            return ret;
        }

        mMsg.attr_end(data);
        return WIFI_SUCCESS;
    }
};

class SetRSSIMonitorCommand : public WifiCommand
{
private:
    s8 mMax_rssi;
    s8 mMin_rssi;
    wifi_rssi_event_handler mHandler;

public:
    SetRSSIMonitorCommand(wifi_request_id id, wifi_interface_handle handle,
                s8 max_rssi, s8 min_rssi, wifi_rssi_event_handler eh)
        : WifiCommand("SetRSSIMonitorCommand", handle, id), mMax_rssi(max_rssi), mMin_rssi
        (min_rssi), mHandler(eh)
    {
    }

    int createRequest(WifiRequest& request, int enable) {
        int result = request.create(GOOGLE_OUI, WIFI_SUBCMD_SET_RSSI_MONITOR);
        if (result < 0) {
            return result;
        }

        ALOGI("set RSSI Monitor, mMax_rssi=%d, mMin_rssi=%d, enable=%d", mMax_rssi, mMin_rssi, enable);

        nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }
        result = request.put_u32(WIFI_ATTRIBUTE_MAX_RSSI, (enable? mMax_rssi: 0));
        if (result < 0) {
            return result;
        }
        ALOGD("create request");
        result = request.put_u32(WIFI_ATTRIBUTE_MIN_RSSI, (enable? mMin_rssi: 0));
        if (result < 0) {
            return result;
        }
        result = request.put_u32(WIFI_ATTRIBUTE_RSSI_MONITOR_START, enable);
        if (result < 0) {
            return result;
        }

        request.attr_end(data);
        return result;
    }

    int start() {
        WifiRequest request(familyId(), ifaceId());
        int result = createRequest(request, 1);
        if (result < 0) {
            return result;
        }
        result = requestResponse(request);
        if (result < 0) {
            ALOGE("Failed to set RSSI Monitor, result=%d", result);
            return result;
        }
        ALOGD("Successfully set RSSI monitoring");
        registerVendorHandler(GOOGLE_OUI, WIFI_EVENT_RSSI_MONITOR);


        if (result < 0) {
            unregisterVendorHandler(GOOGLE_OUI, WIFI_EVENT_RSSI_MONITOR);
            return result;
        }
        ALOGI("Done!");
        return result;
    }

    virtual int cancel() {
        WifiRequest request(familyId(), ifaceId());
        int result = createRequest(request, 0);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to create request, result=%d", result);
        } else {
            result = requestResponse(request);
            if (result != WIFI_SUCCESS) {
                ALOGE("Failed to stop RSSI monitoring, result=%d", result);
            }
        }
        unregisterVendorHandler(GOOGLE_OUI, WIFI_EVENT_RSSI_MONITOR);
        return WIFI_SUCCESS;
    }

    virtual int handleResponse(WifiEvent& reply) {
        /* Nothing to do on response! */
        return NL_SKIP;
    }

    virtual int handleEvent(WifiEvent& event) {
        ALOGD("Got a RSSI monitor event");

        struct nlattr *vendor_data = (struct nlattr *)event.get_vendor_data();
        int len = event.get_vendor_data_len();

        if (vendor_data == NULL || len == 0) {
            ALOGE("RSSI monitor: no vendor data");
            return NL_SKIP;
        }
        /* driver<->HAL event structure */
        #define RSSI_MONITOR_EVT_VERSION   1
        typedef struct {
            u8 version;
            s8 cur_rssi;
            mac_addr BSSID;
        } rssi_monitor_evt;

        rssi_monitor_evt *data = NULL;
        if (vendor_data->nla_type == WIFI_EVENT_RSSI_MONITOR)
            data = (rssi_monitor_evt *)nla_data(vendor_data);
        else
            return NL_SKIP;

        if (!data) {
            ALOGE("RSSI monitor: no data");
            return NL_SKIP;
        }

        ALOGI("data: version=%d, cur_rssi=%d BSSID=" MACSTR "\r\n",
            data->version, data->cur_rssi, MAC2STR(data->BSSID));

        if (data->version != RSSI_MONITOR_EVT_VERSION) {
            ALOGE("Event version mismatch %d, expected %d", data->version, RSSI_MONITOR_EVT_VERSION);
            return NL_SKIP;
        }

        if (*mHandler.on_rssi_threshold_breached) {
            (*mHandler.on_rssi_threshold_breached)(id(), data->BSSID, data->cur_rssi);
        } else {
            ALOGW("No RSSI monitor handler registered");
        }

        return NL_SKIP;
    }
};

class ConfigRoamingCommand : public WifiCommand
{
private:
    wifi_roaming_config *mConfig;

public:
    ConfigRoamingCommand(wifi_interface_handle handle, wifi_roaming_config *config)
        : WifiCommand("ConfigRoamingCommand", handle, 0)
    {
        mConfig = config;
    }

    int createRequest(WifiRequest& request, int subcmd, wifi_roaming_config *config) {
        int result = request.create(GOOGLE_OUI, subcmd);
        if (result < 0) {
            return result;
        }

        nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        result = request.put_u32(WIFI_ATTRIBUTE_ROAMING_BLACKLIST_NUM, config->num_blacklist_bssid);
        if (result < 0) {
            return result;
        }

        mac_addr *bssid_list = config->blacklist_bssid;
        for (u32 i = 0; i < config->num_blacklist_bssid; i++) {
            result = request.put_addr(WIFI_ATTRIBUTE_ROAMING_BLACKLIST_BSSID, bssid_list[i]);
            ALOGI("Blacklist BSSID[%d] " MACSTR, i, MAC2STR(bssid_list[i]));
            if (result < 0) {
                return result;
            }
        }

        result = request.put_u32(WIFI_ATTRIBUTE_ROAMING_WHITELIST_NUM, config->num_whitelist_ssid);
        if (result < 0) {
            return result;
        }

        char ssid[MAX_SSID_LENGTH + 1];
        ssid_t *ssid_list = config->whitelist_ssid;
        for (u32 i = 0; i < config->num_whitelist_ssid; i++) {
            memcpy(ssid, ssid_list[i].ssid_str, ssid_list[i].length);
            ssid[ssid_list[i].length] = '\0';
            result = request.put(
                WIFI_ATTRIBUTE_ROAMING_WHITELIST_SSID, ssid, ssid_list[i].length + 1);
            ALOGI("Whitelist ssid[%d] : %s", i, ssid);
            if (result < 0) {
                return result;
            }
        }

        request.attr_end(data);
        return WIFI_SUCCESS;
    }

    int start() {
        ALOGD("[WIFI HAL] Configure roaming");
        WifiRequest request(familyId(), ifaceId());
        int result = createRequest(request, WIFI_SUBCMD_CONFIG_ROAMING, mConfig);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to create request, result=%d", result);
            return result;
        }

        result = requestResponse(request);
        if (result != WIFI_SUCCESS) {
            ALOGE("[WIFI HAL]Failed to configure roaming, result=%d", result);
        }

        return result;
    }

protected:
    virtual int handleResponse(WifiEvent& reply) {
         ALOGD("ConfigRoamingCommand complete!");
        /* Nothing to do on response! */
        return NL_SKIP;
    }
};

class EnableRoamingCommand : public WifiCommand {
private:
    fw_roaming_state_t mState;
public:
    EnableRoamingCommand(wifi_interface_handle handle, fw_roaming_state_t state)
        : WifiCommand("EnableRoamingCommand", handle, 0) {
            mState = state;
        }
    virtual int create() {
        int ret;

        ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_ENABLE_ROAMING);
        if (ret < 0) {
             ALOGE("Can't create message to send to driver - %d", ret);
             return ret;
        }

        nlattr *data = mMsg.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        ret = mMsg.put_u32(WIFI_ATTRIBUTE_ROAMING_STATE, mState);
        if (ret < 0) {
            return ret;
        }

        mMsg.attr_end(data);
        return WIFI_SUCCESS;
    }
};

class GetFeatureSetCommand : public WifiCommand
{
private:
    int feature_type;
    feature_set *fset;
    feature_set *feature_matrix;
    int *fm_size;
    int set_size_max;

public:
    GetFeatureSetCommand(wifi_interface_handle handle, int feature, feature_set *set,
         feature_set set_matrix[], int *size, int max_size)
        : WifiCommand("GetFeatureSetCommand", handle, 0)
    {
        feature_type = feature;
        fset = set;
        feature_matrix = set_matrix;
        fm_size = size;
        set_size_max = max_size;
    }

    virtual int create() {
        int ret;

        if(feature_type == WIFI_ATTRIBUTE_FEATURE_SET) {
            ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_GET_FEATURE_SET);
        } else if (feature_type == WIFI_ATTRIBUTE_ROAMING_CAPABILITIES) {
            ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_GET_ROAMING_CAPABILITIES);
        } else {
            ALOGE("Unknown feature type %d", feature_type);
            return -1;
        }

        if (ret < 0) {
            ALOGE("Can't create subcmd message to driver, ret=%d", ret);
        }

        return ret;
    }

protected:
    virtual int handleResponse(WifiEvent& reply) {

        ALOGD("In GetFeatureSetCommand::handleResponse for %d", feature_type);

        if (reply.get_cmd() != NL80211_CMD_VENDOR) {
            ALOGD("Ignore reply with cmd 0x%x", reply.get_cmd());
            return NL_SKIP;
        }

        int vendor_id = reply.get_vendor_id();
        int subcmd = reply.get_vendor_subcmd();
        ALOGD("vendor_id = 0x%x, subcmd = 0x%x", vendor_id, subcmd);

        nlattr *vendor_data = reply.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = reply.get_vendor_data_len();
        if (vendor_data == NULL || len == 0) {
            ALOGE("No vendor data in GetFeatureSetCommand response, ignore it");
            return NL_SKIP;
        }

        if (feature_type == WIFI_ATTRIBUTE_FEATURE_SET) {
            void *data = reply.get_vendor_data();
            if (!data) {
                ALOGE("Failed to get VENDOR_DATA");
                return NL_SKIP;
            }
            if (!fset) {
                ALOGE("feature_set pointer is not set");
                return NL_SKIP;
            }
            memcpy(fset, data, min(len, (int) sizeof(*fset)));
            ALOGI("feature_set=0x%x", *fset);
        } else if (feature_type == WIFI_ATTRIBUTE_ROAMING_CAPABILITIES){
            if(!feature_matrix || !fm_size) {
                ALOGE("feature_set pointer is not set");
                return NL_SKIP;
            }

            *fm_size = set_size_max; // black and white
            wifi_roaming_capabilities cap;
            memset(&cap, 0, sizeof(cap));
            for (nl_iterator it(vendor_data); it.has_next(); it.next()) {
                if (it.get_type() == WIFI_ATTRIBUTE_ROAMING_BLACKLIST_NUM) {
                    cap.max_blacklist_size =
                            it.get_u32() > MAX_BLACKLIST_BSSID ? MAX_BLACKLIST_BSSID : it.get_u32();
                } else if (it.get_type() == WIFI_ATTRIBUTE_ROAMING_WHITELIST_NUM ) {
                    cap.max_whitelist_size =
                            it.get_u32() > MAX_WHITELIST_SSID ? MAX_WHITELIST_SSID : it.get_u32();
                } else {
                    ALOGW("Ignore invalid attribute type = %d", it.get_type());
                }
            }
            ALOGI("blacklist/whitelist: %d: %d", cap.max_blacklist_size, cap.max_whitelist_size);
            memcpy(feature_matrix, &cap, sizeof(wifi_roaming_capabilities));
        }

        return NL_OK;
    }

};

class SelectTxPowerCommand : public WifiCommand {
private:
    int mScenario;
public:
    SelectTxPowerCommand(wifi_interface_handle handle, int scenario)
        : WifiCommand("SelectTxPowerCommand", handle, 0) {
            mScenario = scenario;
        }
    virtual int create() {
        int ret;

        ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_SELECT_TX_POWER_SCENARIO);
        if (ret < 0) {
             ALOGE("Can't create message to send to driver - %d", ret);
             return ret;
        }

        nlattr *data = mMsg.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        ret = mMsg.put_u32(WIFI_ATTRIBUTE_TX_POWER_SCENARIO, mScenario);
        if (ret < 0) {
            ALOGE("Failed to put TX_POWER_SCENARIO");
            return ret;
        }

        mMsg.attr_end(data);
        return WIFI_SUCCESS;
    }
};

class SetScanMacOuiCommand : public WifiCommand {
private:
    const byte *mScanMacOui;
public:
    SetScanMacOuiCommand(wifi_interface_handle handle, const oui scan_oui)
        : WifiCommand("SetScanMacOuiCommand", handle, 0), mScanMacOui(scan_oui) {
    }
    virtual int create() {
        ALOGI("Set scan mac oui");
        int ret;

        ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_SET_PNO_RANDOM_MAC_OUI);
        if (ret < 0) {
             ALOGE("Can't create message to send to driver - %d", ret);
             return ret;
        }

        nlattr *data = mMsg.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        ret = mMsg.put(WIFI_ATTRIBUTE_PNO_RANDOM_MAC_OUI, (void *)mScanMacOui, sizeof(oui));
        if (ret < 0) {
            return ret;
        }

        mMsg.attr_end(data);
        return WIFI_SUCCESS;
    }
};

class GetChannelListCommand : public WifiCommand
{
private:
    wifi_channel *mChannels;
    int mMaxChannels;
    int *mNumOfChannel;
    int mBand;

public:
    GetChannelListCommand(wifi_interface_handle handle, int band, int max_channels,
        wifi_channel *channels, int *num_channels)
        : WifiCommand("GetChannelListCommand", handle, 0)
    {
        mBand = band;
        mMaxChannels = max_channels;
        mChannels = channels;
        mNumOfChannel = num_channels;
        memset(mChannels, 0, sizeof(wifi_channel) * mMaxChannels);
    }

    virtual int create() {
        ALOGV("Creating message to get channel list; iface = %d", mIfaceInfo->id);

        int ret = mMsg.create(GOOGLE_OUI, WIFI_SUBCMD_GET_CHANNEL_LIST);
        if (ret < 0) {
            return ret;
        }

        ALOGI("In GetChannelList::mBand=%d", mBand);

        nlattr *data = mMsg.attr_start(NL80211_ATTR_VENDOR_DATA);
        if (!data) {
            ALOGE("Failed attr_start for VENDOR_DATA");
            return WIFI_ERROR_UNKNOWN;
        }

        ret = mMsg.put_u32(WIFI_ATTRIBUTE_BAND, mBand);
        if (ret < 0) {
            return ret;
        }

        mMsg.attr_end(data);
        return ret;
    }

protected:
    virtual int handleResponse(WifiEvent& reply) {

        ALOGV("In GetChannelList::handleResponse");

        if (reply.get_cmd() != NL80211_CMD_VENDOR) {
            ALOGE("Ignore reply with cmd 0x%x", reply.get_cmd());
            return NL_SKIP;
        }

        int vendor_id = reply.get_vendor_id();
        int subcmd = reply.get_vendor_subcmd();
        ALOGV("vendor_id = 0x%x, subcmd = 0x%x", vendor_id, subcmd);

        nlattr *vendor = reply.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = reply.get_vendor_data_len();
        if (vendor == NULL || len == 0) {
            ALOGE("No vendor data in GetChannelList response, ignore it");
            return NL_SKIP;
        }

        int num_channels = 0;
        for (nl_iterator it(vendor); it.has_next(); it.next()) {
            if (it.get_type() == WIFI_ATTRIBUTE_NUM_CHANNELS) {
                num_channels = it.get_u32();
                ALOGI("Get channel list with %d channels", num_channels);
                if (num_channels > mMaxChannels)
                    num_channels = mMaxChannels;
                *mNumOfChannel = num_channels;
            } else if (it.get_type() == WIFI_ATTRIBUTE_CHANNEL_LIST && num_channels) {
                memcpy(mChannels, it.get_data(), sizeof(wifi_channel) * num_channels);
            } else {
                ALOGW("Ignore invalid attribute type = %d, size = %d",
                        it.get_type(), it.get_len());
            }
        }

        ALOGD("mChannels[0]=%d mChannels[1]=%d", *mChannels, *(mChannels + 1));

        return NL_OK;
    }
};

/////////////////////////////////////////////////////////////////////////

static int wifi_get_multicast_id(wifi_handle handle, const char *name, const char *group)
{
    GetMulticastIdCommand cmd(handle, name, group);
    int res = cmd.requestResponse();
    if (res < 0)
        return res;
    else
        return cmd.getId();
}

static bool is_wifi_interface(const char *name)
{
    if (strncmp(name, "wlan", 4) != 0 && strncmp(name, "p2p", 3) != 0 && strncmp(name, "ap", 2) != 0) {
        /* Not a wifi interface; ignore it */
        return false;
    } else {
        return true;
    }
}

int get_interface(const char *name, interface_info *info)
{
    strncpy(info->name, name, sizeof(info->name));
    info->name[sizeof(info->name) - 1] = '\0';
    info->id = if_nametoindex(name);
    ALOGD("found an interface : %s, id = %d", name, info->id);
    return WIFI_SUCCESS;
}

wifi_error wifi_init_interfaces(wifi_handle handle)
{
    ALOGI("init wifi interfaces");
    hal_info *info = (hal_info *)handle;
    struct dirent *de;

    DIR *d = opendir("/sys/class/net");
    if (d == 0)
        return WIFI_ERROR_UNKNOWN;

    int n = 0;
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.')
            continue;
        if (is_wifi_interface(de->d_name) ) {
            n++;
        }
    }

    closedir(d);

    if (n == 0)
        return WIFI_ERROR_NOT_AVAILABLE;

    d = opendir("/sys/class/net");
    if (d == 0)
        return WIFI_ERROR_UNKNOWN;

    info->interfaces = (interface_info **)malloc(sizeof(interface_info *) * n);
    if (!info->interfaces) {
        closedir(d);
        return WIFI_ERROR_OUT_OF_MEMORY;
    }
    memset(info->interfaces, 0, sizeof(interface_info *) * n);

    int i = 0;
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.')
            continue;
        if (is_wifi_interface(de->d_name)) {
            interface_info *ifinfo = (interface_info *)malloc(sizeof(interface_info));
            if (ifinfo == NULL) {
                ALOGE("%s: Error ifinfo NULL", __FUNCTION__);
                while (i > 0) {
                    free(info->interfaces[i - 1]);
                    i--;
                }
                free(info->interfaces);
                closedir(d);
                return WIFI_ERROR_OUT_OF_MEMORY;
            }
            if (get_interface(de->d_name, ifinfo) != WIFI_SUCCESS) {
                free(ifinfo);
                continue;
            }
            ifinfo->handle = handle;
            info->interfaces[i] = ifinfo;
            i++;
        }
    }

    closedir(d);

    info->num_interfaces = n;
    return WIFI_SUCCESS;
}

wifi_error wifi_get_ifaces(wifi_handle handle, int *num, wifi_interface_handle **interfaces)
{
    hal_info *info = (hal_info *)handle;

    *interfaces = (wifi_interface_handle *)info->interfaces;
    *num = info->num_interfaces;

    return WIFI_SUCCESS;
}

wifi_error wifi_get_iface_name(wifi_interface_handle handle, char *name, size_t size)
{
    interface_info *info = (interface_info *)handle;
    strncpy(name, info->name, size);
    return WIFI_SUCCESS;
}

wifi_error wifi_get_supported_feature_set(wifi_interface_handle handle, feature_set *pset)
{
    GetFeatureSetCommand command(handle, WIFI_ATTRIBUTE_FEATURE_SET, pset, NULL, NULL, 1);
    return (wifi_error)command.requestResponse();
}

wifi_error wifi_set_country_code(wifi_interface_handle handle, const char *country_code)
{
    SetCountryCodeCommand command(handle, country_code);
    return (wifi_error) command.requestResponse();
}

static wifi_error wifi_start_rssi_monitoring(wifi_request_id id, wifi_interface_handle
                        iface, s8 max_rssi, s8 min_rssi, wifi_rssi_event_handler eh)
{
    ALOGI("Start RSSI monitoring %d", id);
    wifi_handle handle = getWifiHandle(iface);
    SetRSSIMonitorCommand *cmd = new SetRSSIMonitorCommand(id, iface, max_rssi, min_rssi, eh);
    NULL_CHECK_RETURN(cmd, "memory allocation failure", WIFI_ERROR_OUT_OF_MEMORY);
    wifi_error result = wifi_register_cmd(handle, id, cmd);
    if (result != WIFI_SUCCESS) {
        cmd->releaseRef();
        return result;
    }
    result = (wifi_error)cmd->start();
    if (result != WIFI_SUCCESS) {
        wifi_unregister_cmd(handle, id);
        cmd->releaseRef();
        return result;
    }
    return result;
}

static wifi_error wifi_stop_rssi_monitoring(wifi_request_id id, wifi_interface_handle iface)
{
    ALOGI("Stopping RSSI monitoring %d", id);

    if (id == -1) {
        wifi_rssi_event_handler handler;
        s8 max_rssi = 0, min_rssi = 0;
        memset(&handler, 0, sizeof(handler));
        SetRSSIMonitorCommand *cmd = new SetRSSIMonitorCommand(id, iface, max_rssi, min_rssi, handler);
        NULL_CHECK_RETURN(cmd, "memory allocation failure", WIFI_ERROR_OUT_OF_MEMORY);
        cmd->cancel();
        cmd->releaseRef();
        return WIFI_SUCCESS;
    }
    return wifi_cancel_cmd(id, iface);
}

wifi_error wifi_get_roaming_capabilities(wifi_interface_handle handle,
                                            wifi_roaming_capabilities *caps) {
    ALOGI("Get roaming capabilities");

    wifi_handle wifiHandle = getWifiHandle(handle);
    hal_info *info = getHalInfo(wifiHandle);

    if (!caps) {
        ALOGE("%s: Invalid Buffer provided. Exit", __FUNCTION__);
        return WIFI_ERROR_INVALID_ARGS;
    }

    if (!info) {
        ALOGE("%s: hal_info is NULL", __FUNCTION__);
        return WIFI_ERROR_INVALID_ARGS;
    }

    // first time to get roaming cap
    if (info->roaming_capa.max_blacklist_size == 0 && info->roaming_capa.max_whitelist_size == 0) {
        int size = 0;
        GetFeatureSetCommand command(handle, WIFI_ATTRIBUTE_ROAMING_CAPABILITIES, NULL,
            (feature_set*) caps, &size, 2);
        wifi_error ret = (wifi_error)command.requestResponse();
        if (ret == WIFI_SUCCESS) {
            info->roaming_capa.max_blacklist_size = caps->max_blacklist_size;
            info->roaming_capa.max_whitelist_size = caps->max_whitelist_size;
        }
        return ret;
    } else {
        memcpy(caps, &info->roaming_capa, sizeof(wifi_roaming_capabilities));
    }

    return WIFI_SUCCESS;
}

wifi_error wifi_configure_roaming(wifi_interface_handle handle,
                                     wifi_roaming_config *roaming_config) {
    ALOGI("Configure roaming");
    wifi_handle wifiHandle = getWifiHandle(handle);
    hal_info *info = getHalInfo(wifiHandle);
    if (!roaming_config) {
        ALOGE("%s: Invalid Buffer provided. Exit", __FUNCTION__);
        return WIFI_ERROR_INVALID_ARGS;
    }

    /* Set bssid blacklist */
    if (roaming_config->num_blacklist_bssid > info->roaming_capa.max_blacklist_size) {
        ALOGE("%s: Number of blacklist bssids(%d) provided is more than maximum blacklist bssids(%d)"
              " supported", __FUNCTION__, roaming_config->num_blacklist_bssid,
              info->roaming_capa.max_blacklist_size);
        return WIFI_ERROR_NOT_SUPPORTED;
    }

    /* Set ssid whitelist */
    if (roaming_config->num_whitelist_ssid > info->roaming_capa.max_whitelist_size) {
        ALOGE("%s: Number of whitelist ssid(%d) provided is more than maximum whitelist ssids(%d) "
              "supported", __FUNCTION__, roaming_config->num_whitelist_ssid,
              info->roaming_capa.max_whitelist_size);
        return WIFI_ERROR_NOT_SUPPORTED;
    }

    ConfigRoamingCommand command(handle, roaming_config);
    return (wifi_error)command.start();
}

wifi_error wifi_enable_firmware_roaming(wifi_interface_handle handle,
                              fw_roaming_state_t state) {
    ALOGI("Enable roaming %d", state);
    EnableRoamingCommand command(handle, state);
    return (wifi_error) command.requestResponse();
}

wifi_error wifi_select_tx_power_scenario(wifi_interface_handle handle,
        wifi_power_scenario scenario)
{
    ALOGI("Select tx power scenario %d", scenario);
    SelectTxPowerCommand command(handle, scenario);
    return (wifi_error) command.requestResponse();
}

wifi_error wifi_reset_tx_power_scenario(wifi_interface_handle handle)
{
    ALOGI("Reset tx power scenario");
    SelectTxPowerCommand command(handle, -1);
    return (wifi_error) command.requestResponse();
}

wifi_error wifi_set_scanning_mac_oui(wifi_interface_handle handle, oui scan_oui)
{
    SetScanMacOuiCommand command(handle, scan_oui);
    return (wifi_error) command.requestResponse();
}

wifi_error wifi_get_valid_channels(wifi_interface_handle handle,
        int band, int max_channels, wifi_channel *channels, int *num_channels)
{
    GetChannelListCommand command(handle, band, max_channels, channels, num_channels);
    return (wifi_error) command.requestResponse();
}

/////////////////////////////////////////////////////////////////////////////

static wifi_error wifi_get_wake_reason_stats_dummy(wifi_interface_handle iface,
    WLAN_DRIVER_WAKE_REASON_CNT *wifi_wake_reason_cnt)
{
    if (wifi_wake_reason_cnt) {
        memset(wifi_wake_reason_cnt, 0, sizeof(*wifi_wake_reason_cnt));
        return WIFI_SUCCESS;
    }
    return  WIFI_ERROR_INVALID_ARGS;
}

static wifi_error wifi_get_packet_filter_capabilities_dummy(wifi_interface_handle handle,
    u32 *version, u32 *max_len)
{
    if (version && max_len) {
        *version = 0;
        *max_len = 0;
        return WIFI_SUCCESS;
    }
    return  WIFI_ERROR_INVALID_ARGS;
}

static wifi_error wifi_get_tx_pkt_fates_dummy(wifi_interface_handle handle,
    wifi_tx_report *tx_report_bufs, size_t n_requested_fates, size_t *n_provided_fates)
{
    if (n_provided_fates) {
        *n_provided_fates = 0;
        return WIFI_SUCCESS;
    }
    return WIFI_ERROR_INVALID_ARGS;
}

static wifi_error wifi_get_rx_pkt_fates_dummy(wifi_interface_handle handle,
        wifi_rx_report *rx_report_bufs, size_t n_requested_fates, size_t *n_provided_fates)
{
    if (n_provided_fates) {
        *n_provided_fates = 0;
        return WIFI_SUCCESS;
    }
    return WIFI_ERROR_INVALID_ARGS;
}


static wifi_error wifi_get_ring_buffers_status_dummy(wifi_interface_handle iface,
        u32 *num_rings, wifi_ring_buffer_status *status)
{
    if (num_rings && status) {
        *num_rings = 1;
        memset(status, 0, sizeof(*status));
        strncpy((char*)status->name, "dummy", sizeof(status->name));
        return WIFI_SUCCESS;
    }
    return WIFI_ERROR_INVALID_ARGS;
}

static wifi_error wifi_get_logger_supported_feature_set_dummy(wifi_interface_handle iface,
        unsigned int *support)
{
    if (support) {
        *support = 0;
        return WIFI_SUCCESS;
    }
    return WIFI_ERROR_INVALID_ARGS;
}
