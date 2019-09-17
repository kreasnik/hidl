
#include "ioctl_talkie.h"
#define LOG_TAG "talkie-hal"
int open_dev()
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "open_dev\n");
    fd = open(NODE,O_RDWR);
    if (fd < 0)
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Open Dev %s Error", NODE);
        return -1;
    }
    return 0;
}

int close_dev()
{
    if(fd < 0)
    {
        return -1;
    }
    else
    {
        close(fd);
    }
    return 0;
}

int app_start()
{
    int arg = 0;
    if (ioctl(fd, INTERCOM_APP_START, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_APP_START fail\n");
        return -1;
    }
    return 0;
}

int app_exit()
{
    int arg = 0;
    if (ioctl(fd, INTERCOM_APP_EXIT, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_APP_EXIT fail\n");
        return -1;
    }
    return 0;
}

int ppt_down(int arg)
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "ppt_down\n");
    if (ioctl(fd, INTERCOM_KEY_DOWN, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_KEY_DOWN fail\n");
        return -1;
    }
    return 0;
}

int ppt_up(int arg)
{
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "ppt_up\n");
    if (ioctl(fd, INTERCOM_KEY_UP, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_KEY_UP fail\n");
        return -1;
    }
    return 0;
}

int speak_enable()
{
    int arg = 0;
    if (ioctl(fd, INTERCOM_SPEAK_ENABLE, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_SPEAK_ENABLE fail\n");
        return -1;
    }
    return arg;
}

int speak_disable()
{
    int arg = 0;
    if (ioctl(fd, INTERCOM_SPEAK_DISABLE, &arg) < 0) 
    {
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "Call cmd INTERCOM_SPEAK_DISABLE fail\n");
        return -1;
    }
    return 0;
}
