#ifndef __ESP_COMMON_H__
#define __ESP_COMMON_H__

#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/cdev.h>
#include <linux/input.h>

//use 6757 path for compile begin
#include "../../../spi/mediatek/mt6757/mtk_spi.h" 
//use 6757 path for compile end
 
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#else
#include <linux/notifier.h>
#endif
#include "esp_typedef.h"

/**************************debug******************************/
#define ERR_LOG  (0)
#define INFO_LOG (1)
#define DEBUG_LOG (2)


#define ic_debug(level, fmt, args...) do { \
			if (g_debug_level >= level) {\
				pr_warn("[esp] " fmt, ##args); \
			} \
		} while (0)

#define FUNC_ENTRY()  ic_debug(DEBUG_LOG, "%s, %d, enter\n", __func__, __LINE__)
#define FUNC_EXIT()  ic_debug(DEBUG_LOG, "%s, %d, exit\n", __func__, __LINE__)

#define TEST_LEN        32
//#define TEST_DATA
#define RH_LEN          4
#define SUCCESS         0x55
#define FIFO_LEN        28
#define DATA_LEN		2048
#define DATA_READ		0xaa

typedef struct
{
    uint8    cmd;            ///< Command value
    uint8    addr;           ///< Point to address value
}spi_header;

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


struct esp_device {
	dev_t devno;
	struct cdev cdev;
	struct device *device;
	struct class *class;
	struct spi_device *spi;
	int device_count;
	struct mt_chip_conf spi_mcc;

	spinlock_t spi_lock;
	struct list_head device_entry;

	struct input_dev *input;

	u8 *spi_buffer;  /* only used for SPI transfer internal */
	struct mutex buf_lock;
	struct mutex release_lock;
	u8 buf_status;

	/* for netlink use */
	struct sock *nl_sk;

#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#else
	struct notifier_block notifier;
#endif

	u8 probe_finish;
	u8 irq_count;

	/* bit24-bit32 of signal count */
	/* bit16-bit23 of event type, 1: key down; 2: key up; 3: fp data ready; 4: home key */
	/* bit0-bit15 of event type, buffer status register */
	u32 event_type;
	u8 sig_count;
	u8 is_sleep_mode;
	u8 system_status;

	u32 cs_gpio;
	u32 reset_gpio;
	u32 irq_gpio;
	u32 irq_num;
	u8  need_update;
	u32 irq;

#ifdef CONFIG_OF
	struct pinctrl *pinctrl_gpios;
	struct pinctrl_state *pins_irq;
	struct pinctrl_state *pins_miso_spi, *pins_miso_pullhigh, *pins_miso_pulllow;
	struct pinctrl_state *pins_reset_high, *pins_reset_low;
#endif
};

#endif	/* __ESP_COMMON_H__ */
