
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <android/log.h>

#define NODE "dev/hctintercom_node"

/*
  1> 对讲机APK启动:
        拉高GPIO 80(3.3V供电) && 拉高 GPIO 12(UART电平转换)
     此时进入待机模式
 -1> APK退出或者其他情况(上层自行决定)
        拉低GPIO 80 &&　拉低　GPIO 12

  2> 按键按下
        拉高GPIO 58(PTT键) &&　切换Mic(对讲模式)
     ==> 其中Mic分为主Mic和耳机Mic
        Update 20171127:
            约定传递arg来决定是耳机还是主Mic: 1==>耳机Mic || 其他==>主Mic
 -2> 按键抬起
        拉低GPIO 58 && 切换为非对讲模式

  3> 接受到语音,GPIO 52使能
 -3> Speak EN 使能关闭(上层自行决定)
 */

#define IOC_MAGIC           'i'
#define INTERCOM_APP_START      _IO(IOC_MAGIC, 1)     //对讲机 APP启动
#define INTERCOM_APP_EXIT       _IO(IOC_MAGIC, 2)     //对讲机 APP退出
#define INTERCOM_KEY_DOWN       _IOW(IOC_MAGIC, 3, int)     //PTT按键按下  ||   APP按钮按下
#define INTERCOM_KEY_UP         _IOW(IOC_MAGIC, 4, int)     //PTT按键抬起  ||   APP按钮抬起
#define INTERCOM_SPEAK_ENABLE   _IOR(IOC_MAGIC, 5, int)     //Speak Enable
#define INTERCOM_SPEAK_DISABLE  _IO(IOC_MAGIC, 6)     //Speak Disable

static int fd = 0;

extern int open_dev();
extern int close_dev();
extern int app_start();
extern int app_exit();
extern int ppt_down(int arg);
extern int ppt_up(int arg);
extern int speak_enable();
extern int speak_disable();