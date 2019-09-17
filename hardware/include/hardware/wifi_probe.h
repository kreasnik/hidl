#ifndef WIFI_PROBE_H
#define WIFI_PROBE_H
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
#include "typedef.h"
#include <sys/cdefs.h>
#include <hardware/hardware.h>
#include <hardware/hwcomposer_defs.h>

//#define TEST_DATA
#define SUCCESS         0x55
#define FIFO_LEN        28
#define DATA_LEN		2048
#define DATA_READ		0xaa

typedef struct
{
	uint8 	result;
	uint8	id;			//命令ID
	uint16	len;		//para data len
}message_header;

typedef struct
{
	message_header	header;
  	char 		data[FIFO_LEN];	// pointer to the actual data in the buffer

}spi_data_t;

typedef struct
{
	message_header	header;
  	char 			para[DATA_LEN];	// pointer to the actual data in the buffer

}message_t;

typedef enum
{
    AT_M2S_UPGRADE = 0,         /*固件更新*/
    AT_M2S_SET_RATE,            /*调节速率*/
    AT_M2S_GET_DATA,            /*获取数据*/
    AT_M2S_GET_VER,             /*获取固件版本号*/
    AT_M2S_SET_FIRMWARE_SIZE,   /*版本大小*/
    AT_M2S_START_SEARCH,
    AT_M2S_STOP_SEARCH,
    AT_M2S_UPGRADE_DONE,
    AT_M2S_END,

    AT_M2S_UPGRADE_RSP,
    AT_M2S_SET_RATE_RSP,
    AT_M2S_GET_DATA_RSP,
    AT_M2S_GET_DATA_END_RSP,
    AT_M2S_GET_VER_RSP,
    AT_M2S_SET_FIRMWARE_SIZE_RSP,
    AT_M2S_START_SEARCH_RSP,
    AT_M2S_STOP_SEARCH_RSP,
    AT_M2S_UPGRADE_DONE_RSP,
    AT_MAX,
}AT_CMD_ID;


/**********************IO Magic**********************/
#define ESP_IOC_MAGIC	            'g'
#define ESP_IOC_OPEN_DEV			_IOR(ESP_IOC_MAGIC, 0, int)
#define ESP_IOC_CLOSE_DEV			_IOR(ESP_IOC_MAGIC, 1, int)
#define ESP_IOC_START_SEARCH		_IOR(ESP_IOC_MAGIC, 2, message_t)
#define ESP_IOC_STOP_SEARCH			_IOR(ESP_IOC_MAGIC, 3, message_t)
#define ESP_IOC_GET_VER			    _IOR(ESP_IOC_MAGIC, 4, message_t)
#define ESP_IOC_SET_FIRMWARE_SIZE   _IOWR(ESP_IOC_MAGIC, 5, message_t)
#define ESP_IOC_FIRMWARE_UPGRADE    _IOWR(ESP_IOC_MAGIC, 6, message_t)
#define ESP_IOC_SET_FIRMWARE_DONE   _IOR(ESP_IOC_MAGIC, 7, message_t)
#define ESP_IOC_UPDATE_RATE		    _IOWR(ESP_IOC_MAGIC, 8, message_t)
#define ESP_IOC_GET_DATA            _IOR(ESP_IOC_MAGIC, 9, message_t)
#define ESP_IOC_SPI_EINT            _IOR(ESP_IOC_MAGIC, 10, int)




#define WIFI_PROBE_MODULE_API_VERSION_1_1 HARDWARE_MODULE_API_VERSION(1, 1)
#define WIFI_PROBE_HARDWARE_MODULE_ID "wifi_probe"

typedef struct wifi_probe_module {
    struct hw_module_t common;
} wifi_probe_module_t;

typedef struct wifi_probe {
    struct hw_device_t common;
    int (*wifi_probe_open)(struct wifi_probe *dev);
    int (*wifi_probe_close)(struct wifi_probe *dev);
    int (*wifi_probe_start_search)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_stop_search)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_get_version)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_start_firmware_upgrade)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_set_rate)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_get_data)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_get_eint)(struct wifi_probe *dev, int eint);
    int (*wifi_probe_set_firmware_size)(struct wifi_probe *dev, message_t *msg);
    int (*wifi_probe_set_firmware_done)(struct wifi_probe *dev, message_t *msg);
    void* reserved[8 - 6];
} wifi_probe_t;
#endif