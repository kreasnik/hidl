
#include "ioctl_wifi_probe.h"

int open_dev()
{
    int arg = 0;
    ALOGD("open_dev\n");
    fd = open(NODE,O_RDWR);
    if (fd < 0){
        ALOGD("Open Dev %s Error", NODE);
        return -1;
    }
    if (ioctl(fd, ESP_IOC_OPEN_DEV, &arg) < 0){
        ALOGD("Call cmd ESP_IOC_OPEN_DEV fail\n");
        return -1;
    }
    return 0;
}

int close_dev()
{
    int arg = 0;
    if (ioctl(fd, ESP_IOC_CLOSE_DEV, &arg) < 0){
        ALOGD("Call cmd ESP_IOC_CLOSE_DEV fail\n");
    }
    if(fd < 0){
        return -1;
    }else{
        close(fd);
    }
    return 0;
}

int start_search(message_t *msg)
{
    if (ioctl(fd, ESP_IOC_START_SEARCH, msg) < 0)
    {
        ALOGD("Call cmd ESP_IOC_START_SEARCH fail\n");
        return -1;
    }
    return 0;
}

int stop_search(message_t *msg)
{
    if (ioctl(fd, ESP_IOC_STOP_SEARCH, msg) < 0)
    {
        ALOGD("Call cmd ESP_IOC_STOP_SEARCH fail\n");
        return -1;
    }
    return 0;
}

#ifdef TEST_DATA
int get_version_test(message_t *msg){
    char* version = "esp8266_v1.0_0411";
    msg->header.id = AT_M2S_GET_VER;
    msg->header.len = strlen(version);
    memcpy(msg->para, version, msg->header.len);
    return 0;
}
#endif

int get_version(message_t *msg)
{
#ifdef TEST_DATA
    return get_version_test(msg);
#else
    if (ioctl(fd, ESP_IOC_GET_VER, msg) < 0) 
    {
        ALOGD("Call cmd ESP_IOC_GET_VER fail\n");
        return -1;
    }
    return 0;
#endif
}
#ifdef TEST_DATA
int start_firmware_upgrade_test(message_t *msg){
    char *tmp = NULL;
    tmp = malloc(msg->header.len);
    memcpy(tmp, msg->para, msg->header.len);
    ALOGD("start_firmware_upgrade_test len:%d, upgrade:%s\n", msg->header.len, tmp);
    if(tmp != NULL){
        free(tmp);
        tmp = NULL;
    }
    return 0;
}
#endif
int start_firmware_upgrade(message_t *msg)
{
#ifdef TEST_DATA
    return start_firmware_upgrade_test(msg);
#else
    if (ioctl(fd, ESP_IOC_FIRMWARE_UPGRADE, msg) < 0) 
    {
        ALOGD("Call cmd ESP_IOC_FIRMWARE_UPGRADE fail\n");
        return -1;
    }
    return 0;
#endif
}
#ifdef TEST_DATA
int set_rate_test(message_t *msg){
    ALOGD("set_rate_test len:%d, para:%s\n", msg->header.len, msg->para);
    return 0;
}
#endif
int set_rate(message_t *msg)
{
#ifdef TEST_DATA
    return set_rate_test(msg);
#else
    if (ioctl(fd, ESP_IOC_UPDATE_RATE, msg) < 0) 
    {
        ALOGD("Call cmd ESP_IOC_UPDATE_RATE fail\n");
        return -1;
    }
    return 0;
#endif
}

#ifdef TEST_DATA
int get_data_test(message_t *msg){
    char* data = "3C:46:D8:2E:EC:7B|80:81:00:61:70:88|02|00|12|-51|TP-LINK TECHNOLOGIES CO.,LTD.";
    msg->header.id = AT_M2S_GET_DATA;
    msg->header.len = strlen(data);
    memcpy(msg->para, data, msg->header.len);
    return 0;
}
#endif
int get_data(message_t *msg)
{
#ifdef TEST_DATA
    return get_data_test(msg);
#else
    if (ioctl(fd, ESP_IOC_GET_DATA, msg) < 0) 
    {
        ALOGD("Call cmd ESP_IOC_GET_DATA fail\n");
        return -1;
    }
    return 0;
#endif
}

int get_eint(int eint)
{
    int arg = 0;
    if (ioctl(fd, ESP_IOC_SPI_EINT, &arg) < 0)
    {
        ALOGD("Call cmd ESP_IOC_SPI_EINT fail\n");
        return -1;
    }
    return arg;
}

#ifdef TEST_DATA
int set_firmware_size_test(message_t *msg){
    ALOGD("set_firmware_size_test len:%d, para:%s\n", msg->header.len, msg->para);
    return 0;
}
#endif
int set_firmware_size(message_t *msg)
{
#ifdef TEST_DATA
    return set_firmware_size_test(msg);
#else
    if (ioctl(fd, ESP_IOC_SET_FIRMWARE_SIZE, msg) < 0) 
    {
        ALOGD("Call cmd ESP_IOC_SET_FIRMWARE_SIZE fail\n");
        return -1;
    }
    return 0;
#endif
}

#ifdef TEST_DATA
int set_firmware_done_test(message_t *msg){
    ALOGD("set_firmware_size_test len:%d\n", msg->header.len);
    return 0;
}
#endif
int set_firmware_done(message_t *msg)
{
#ifdef TEST_DATA
    return set_firmware_done_test(msg);
#else
    if (ioctl(fd, ESP_IOC_SET_FIRMWARE_DONE, msg) < 0)
    {
        ALOGD("Call cmd ESP_IOC_SET_FIRMWARE_DONE fail\n");
        return -1;
    }
    return 0;
#endif
}