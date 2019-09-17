 /*This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/fb.h>
#include <linux/regulator/consumer.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#endif

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif

#ifdef CONFIG_MTK_CLKMGR
#include "mach/mt_clkmgr.h"
#else
#include <linux/clk.h>
#endif

#include <net/sock.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

/* MTK header */
// use 6757 path begin for compile 
#include "../../../spi/mediatek/mt6763/mtk_spi.h"  
#include "../../../spi/mediatek/mt6763/mtk_spi_hal.h"
#include <linux/regulator/mediatek/mtk_regulator.h>  

extern void hct_waite_for_security_dts_paser(void);
extern int  hct_security_set_reset(int cmd);
extern int  hct_security_set_down(int cmd);
extern int  hct_security_set_spi_mode(int cmd);
extern unsigned int hct_security_get_irq_gpio(void);
static int hct_security_probe_isok = 0;                   
// use 6757 path end for compile 
#include "mtk_gpio.h"
//#include "mach/gpio_const.h"
//#include "upmu_common.h"
#include "esp_common.h"
#if defined(CONFIG_ARCH_MT6797)
#include <mt_vcorefs_manager.h>
#endif

/**************************defination******************************/
#define ESP_DEV_NAME "esp_8266"  // atb modify 20161222
#define ESP_DEV_MAJOR 0	/* assigned */

#define ESP_CLASS_NAME "esp_8266"

/* debug log setting */
u8 g_debug_level = ERR_LOG;

/*************************************************************/
static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);

static unsigned int bufsiz = (20 * 1024);
module_param(bufsiz, uint, S_IRUGO);
MODULE_PARM_DESC(bufsiz, "maximum data bytes for SPI message");

#ifdef CONFIG_OF
static const struct of_device_id esp_of_match[] = {
    { .compatible = "mediatek,security-esp", },
	{},
};
MODULE_DEVICE_TABLE(of, esp_of_match);
#endif


/* for netlink use */

static u8 g_vendor_id = 0;

static ssize_t esp_debug_show(struct device *dev,
			struct device_attribute *attr, char *buf);

static ssize_t esp_debug_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count);

static DEVICE_ATTR(debug, S_IRUGO | S_IWUSR, esp_debug_show, esp_debug_store);

static struct attribute *esp_debug_attrs[] = {
	&dev_attr_debug.attr,
	NULL
};

static const struct attribute_group esp_debug_attr_group = {
	.attrs = esp_debug_attrs,
	.name = "debug"
};

const struct mt_chip_conf spi_ctrdata = {
	.cs_pol = 0,
	.sample_sel = 0,
    .setuptime = 10,
    .holdtime = 10,
    // .high_time=10, // 10--6m 15--4m 20--3m 30--2m [ 60--1m 120--0.5m  300--0.2m]80 
    // .low_time=10,
    .cs_idletime = 10,
    .ulthgh_thrsh = 0,
    .cpol = 0,
    .cpha = 0,
    .rx_mlsb = SPI_MSB,
    .tx_mlsb = SPI_MSB,
    .tx_endian = 0,
    .rx_endian = 0,
    .com_mod = FIFO_TRANSFER, //DMA_TRANSFER,
    .pause = 1,
    .finish_intr = 1,
    .deassert = 0,
    .ulthigh = 0,
    .tckdly = 0,
};


static struct work_struct esp_work;
static struct hrtimer esp_timer;
static ktime_t ktime;  
static unsigned int interval=5000; /* unit: us */  
struct esp_device *esp_dev = NULL;
static DEFINE_MUTEX(dev_lock);
static int is_eint_timeout = 0;
static wait_queue_head_t g_Waitq;
static wait_queue_head_t g_Waitq_timeout;
static DECLARE_WAIT_QUEUE_HEAD(g_Waitq);
static DECLARE_WAIT_QUEUE_HEAD(g_Waitq_timeout);
static unsigned int esp_spi_eint_irqnum;
static int spi_eint;
static int flag_irq;
static int flag_irq_timeout;
static void esp_spi_eint_handler(unsigned long data);
static DECLARE_TASKLET(esp_spi_tasklet,esp_spi_eint_handler,0);
static void esp_spi_eint_handler_timeout(unsigned long data);
static DECLARE_TASKLET(esp_spi_tasklet_timeout,esp_spi_eint_handler_timeout,0);

static void esp_spi_eint_handler(unsigned long data)
{
    flag_irq = 1;
	FUNC_ENTRY();
    wake_up_interruptible(&g_Waitq);
    enable_irq(esp_spi_eint_irqnum);
}

static void esp_spi_eint_handler_timeout(unsigned long data)
{
    flag_irq_timeout = 1;
	FUNC_ENTRY();
    wake_up_interruptible(&g_Waitq_timeout);
    enable_irq(esp_spi_eint_irqnum);
}

static irqreturn_t request_interrupt(int irqnum, void *data)
{
    FUNC_ENTRY();
    disable_irq_nosync(esp_spi_eint_irqnum);
	if(is_eint_timeout == 1){
		tasklet_schedule(&esp_spi_tasklet_timeout);
	}else{
    	tasklet_schedule(&esp_spi_tasklet);
	}
    FUNC_EXIT();
    return IRQ_HANDLED;
}

static int esp_init_interrupt(void)
{
    int ret = -1;
    esp_spi_eint_irqnum = hct_security_get_irq_gpio();
    if(esp_spi_eint_irqnum<=0){
        ret = esp_spi_eint_irqnum;
        ic_debug(INFO_LOG, "%s, hct_security_get_irq_gpio fail.\n", __func__);
    }
    ret = request_irq(esp_spi_eint_irqnum, request_interrupt, IRQF_TRIGGER_RISING, "security_eint", NULL);
    enable_irq(esp_spi_eint_irqnum);
    enable_irq_wake(esp_spi_eint_irqnum);
    return ret;
}

int esp_wait_spi_eint(void)
{
    FUNC_ENTRY();
    flag_irq = 0;
    wait_event_interruptible(g_Waitq, 0 != flag_irq);
    FUNC_EXIT();
    return spi_eint;
}

int esp_wait_spi_eint_timeout(int timeout)
{
    FUNC_ENTRY();
    flag_irq_timeout = 0;
    wait_event_interruptible_timeout(g_Waitq_timeout, 0 != flag_irq_timeout, timeout);
    FUNC_EXIT();
    return spi_eint;
}

int esp_spi_sync(struct esp_device *dev, uint8 *txbuf, uint8 *rxbuf, int len){
	struct spi_message msg;
	struct spi_transfer *xfer = NULL;
	xfer = kzalloc(280*10, GFP_KERNEL| GFP_DMA);
	if (xfer == NULL) {
		ic_debug(ERR_LOG, "%s, no memory for SPI transfer\n", __func__);
		return -ENOMEM;
	}
	dev->spi_mcc.com_mod = DMA_TRANSFER; //DMA_TRANSFER;
	spi_setup(dev->spi);
	mdelay(1);
	spi_message_init(&msg);

	xfer[0].tx_buf = txbuf;
	xfer[0].len = len;
	xfer[0].delay_usecs = 5;
	
	xfer[0].rx_buf = rxbuf;
	xfer[0].cs_change = 1;
	spi_message_add_tail(&xfer[0], &msg);
	mutex_lock(&dev_lock);
	spi_sync(dev->spi, &msg);
	mutex_unlock(&dev_lock);
	kfree(xfer);
	if (xfer != NULL)
		xfer = NULL;

	return 0;
}


int esp_spi_write(uint8 *txbuf, int len){
    int ret = -1;
    int total_len = len+sizeof(spi_header);
    uint8 *tbuf = NULL;
    uint8 *rbuf = NULL;
    tbuf = kzalloc(total_len, GFP_KERNEL);
    rbuf = kzalloc(total_len, GFP_KERNEL);
    memset(tbuf, 0, total_len);
    tbuf[0] = 0x02;
    tbuf[1] = 0x00;
    memcpy(&tbuf[2], txbuf, len);
    ret = esp_spi_sync(esp_dev, tbuf, rbuf, total_len);
    kfree(tbuf);
    kfree(rbuf);
    if (tbuf != NULL)
        tbuf = NULL;
    if (rbuf != NULL)
        rbuf = NULL;
    return ret;
}

int esp_spi_read(uint8 *rxbuf, int len){
    int ret = -1;
    int total_len = len;
    uint8 *tbuf = NULL;
    tbuf = kzalloc(total_len, GFP_KERNEL);
    tbuf[0] = 0x03;
    tbuf[1] = 0x00;
    ret = esp_spi_sync(esp_dev, tbuf, rxbuf, total_len);
    kfree(tbuf);
    if (tbuf != NULL)
        tbuf = NULL;
    return ret;
}

int esp_spi_send(struct esp_device *esp_dev, message_t *arg){
    int i = 0;
    int retry = 3;
	int retval = -1;
	int count = 0;
	uint8 txdata[sizeof(spi_data_t)] = {0};
	uint8 rxdata[sizeof(spi_data_t)+RH_LEN] = {0};
	message_header header;
	message_t txbuf;
	message_t rxbuf;

    memset(&header, 0, sizeof(message_header));
	memset(&txbuf, 0, sizeof(message_t));
	memset(&rxbuf, 0, sizeof(message_t));

    copy_from_user(&txbuf, (message_t*)arg, sizeof(message_t));
    ic_debug(INFO_LOG, "%s: id:%d, len:%d\n", __func__, txbuf.header.id, txbuf.header.len);
	if(txbuf.header.result == DATA_READ){
		is_eint_timeout = 0;
		while(retry > 0){
			memset(rxdata, 0, sizeof(rxdata));
			esp_spi_read(rxdata, sizeof(rxdata));
			memcpy(&rxbuf, &rxdata[2], sizeof(spi_data_t));
			/*
			ic_debug(INFO_LOG, "%s DATA_READ rxdata = ", __func__);
			for(i = 0; i < sizeof(rxdata); i++){
				printk("0x%02x ", rxdata[i]);
			}
			ic_debug(INFO_LOG, "\n");
			*/
			ic_debug(INFO_LOG, "%s: data rxbuf.header.result:0x%02x\n", __func__, rxbuf.header.result);
			if(rxbuf.header.result != SUCCESS){
				retry --;
			}else{
				break;
			}
		}
		mdelay(10);
		memset(txdata, 0, sizeof(txdata));
		memcpy(&txdata, &rxbuf, sizeof(txdata));
		ic_debug(INFO_LOG, "%s: DATA_READ write txdata[0]:%d\n", __func__, txdata[0]);
		esp_spi_write(txdata, sizeof(txdata));
	}else{
		if(txbuf.header.len == 0 || txbuf.header.len < FIFO_LEN){
		    is_eint_timeout = 1;
			while(retry > 0){
				memcpy(txdata, &txbuf, sizeof(spi_data_t));
				ic_debug(INFO_LOG, "%s: txbuf.header.len:%d retry:%d\n", __func__, txbuf.header.len, retry);
				esp_spi_write(txdata, sizeof(txdata));
				esp_wait_spi_eint_timeout(200);
				memset(rxdata, 0, sizeof(rxdata));
				esp_spi_read(rxdata, sizeof(rxdata));
				memcpy(&rxbuf, &rxdata[2], sizeof(spi_data_t));
				/*
				ic_debug(INFO_LOG, "%s rxdata = ", __func__);
				for(i = 0; i < sizeof(rxdata); i++){
					printk("0x%02x ", rxdata[i]);
				}
				ic_debug(INFO_LOG, "\n");
				*/
				ic_debug(INFO_LOG, "%s: cmd rxbuf.header.result:0x%02x\n", __func__, rxbuf.header.result);
				if(rxbuf.header.result != SUCCESS){
					retry --;
				}else{
					break;
				}
			}
			is_eint_timeout = 0;
		}else{
		    is_eint_timeout = 1;
			count = txbuf.header.len/FIFO_LEN + 1;
			ic_debug(INFO_LOG, "%s: count:%d\n", __func__, count);
			int k = 0;
			for(i = 0; i < count; i++){
                memcpy(&header, &txbuf, sizeof(message_header));
                if(i == count-1){
                    header.len = txbuf.header.len - i*FIFO_LEN;
                }else{
                    header.len = FIFO_LEN;
                }
                ic_debug(INFO_LOG, "%s: header.len:%d i:%d\n", __func__, header.len, i);
                memset(txdata, 0, sizeof(txdata));
                memcpy(txdata, &header, sizeof(message_header));
                memcpy(txdata + sizeof(message_header), txbuf.para + i*FIFO_LEN, header.len);
                /*
                ic_debug(INFO_LOG, "%s upgrade = ", __func__);
                for(k = 0; k < sizeof(txdata); k++){
                    printk("0x%02x ", txdata[k]);
                }
                ic_debug(INFO_LOG, "\n");
                */
                esp_spi_write(txdata, sizeof(txdata));
				esp_wait_spi_eint_timeout(500);
				memset(rxdata, 0, sizeof(rxdata));
				esp_spi_read(rxdata, sizeof(rxdata));
				memcpy(&rxbuf, &rxdata[2], sizeof(spi_data_t));
				ic_debug(INFO_LOG, "%s: updrade rxbuf.header.result:0x%02x\n", __func__, rxbuf.header.result);
                mdelay(1);
			}
		}
	}
    retval = copy_to_user((message_t*)arg, &rxbuf, sizeof(message_t));
    ic_debug(ERR_LOG, "%s: retval:%d result:%x\n", __func__, retval, rxbuf.header.result);
    return retval;
}

void spi_test(void){
	struct spi_message msg;
	struct spi_transfer *xfer = NULL;
	int i = 0;
	int len = sizeof(spi_data_t);
	uint8 tbuf[sizeof(spi_data_t)] = {0};
	uint8 rbuf[sizeof(spi_data_t)+sizeof(spi_header)] = {0};

    tbuf[0] = 0x55;
    tbuf[1] = 0xaa;
    tbuf[2] = 0x5a;
    tbuf[3] = 0xa5;

    esp_spi_write(tbuf, len);

    memset(rbuf, 0, len);
    esp_spi_read(rbuf, len);

    ic_debug(ERR_LOG, "%s rbuf = ", __func__);
    for(i = 0; i < sizeof(rbuf); i++){
        printk("0x%02x ", rbuf[i]);
    }
    ic_debug(ERR_LOG, "\n");

	return 0;
}

static enum hrtimer_restart esp_timer_func(struct hrtimer *timer)
{
	schedule_work(&esp_work);
	return HRTIMER_NORESTART;
}

static void wq_func_hrtimer(struct work_struct *work)  
{   
    struct timespec uptime;  
 
	mdelay(1);
	spi_test();
	esp_timer.function = esp_timer_func;   
	ktime = ktime_set( interval/1000000, (interval%1000000)*10000);  
	hrtimer_start(&esp_timer, ktime, HRTIMER_MODE_REL );  
}   

static void esp_spi_clk_enable(struct esp_device *esp_dev, u8 bonoff)
{
	static int count = 0;
#ifdef CONFIG_MTK_CLKMGR
        if (bonoff && (count == 0)) {
                ic_debug(DEBUG_LOG, "%s, start to enable spi clk && count = %d.\n", __func__, count);
                enable_clock(MT_CG_PERI_SPI0, "spi");
                count = 1;
        } else if ((count > 0) && (bonoff == 0)) {
                ic_debug(DEBUG_LOG, "%s, start to disable spi clk&& count = %d.\n", __func__, count);
                disable_clock(MT_CG_PERI_SPI0, "spi");
                count = 0;
    }
#else
	/* changed after MT6797 platform */
	struct mt_spi_t *ms = NULL;

	ms = spi_master_get_devdata(esp_dev->spi->master);

	if (bonoff && (count == 0)) {
		//enable_clk(ms);    // FOR MT6797
//		clk_enable(ms->clk_main); // FOR MT6755/MT6750
		count = 1;
	} else if ((count > 0) && (bonoff == 0)) {
		//disable_clk(ms);    // FOR MT6797
//		clk_disable(ms->clk_main); // FOR MT6755/MT6750
		count = 0;
	}
#endif
}

static void esp_reset_gpio_cfg(struct esp_device *esp_dev)
{
#ifdef CONFIG_OF
	pinctrl_select_state(esp_dev->pinctrl_gpios, esp_dev->pins_reset_high);
#endif

}

/* delay ms after reset */
static void esp_hw_power(u8 delay, u8 on)
{
#ifdef CONFIG_OF
	hct_security_set_reset(on);
#endif
	if (delay) {
		/* delay is configurable */
		mdelay(delay);
	}
}

/* -------------------------------------------------------------------- */
/* file operation function                                              */
/* -------------------------------------------------------------------- */
static ssize_t esp_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	ic_debug(ERR_LOG, "%s: Not support write opertion \n", __func__);
	return -EFAULT;
}

static ssize_t esp_write(struct file *filp, const char __user *buf,
			size_t count, loff_t *f_pos)
{
	ic_debug(ERR_LOG, "%s: Not support write opertion \n", __func__);
	return -EFAULT;
}


static long esp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct esp_device *esp_dev = NULL;
	int ioarg = 0;
	int retval = -1;
	FUNC_ENTRY();
	if (_IOC_TYPE(cmd) != ESP_IOC_MAGIC)
		return -EINVAL;

	/* Check access direction once here; don't repeat below.
	* IOC_DIR is from the user perspective, while access_ok is
	* from the kernel perspective; so they look reversed.
	*/
	if (_IOC_DIR(cmd) & _IOC_READ)
		retval = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));

	if (retval == 0 && _IOC_DIR(cmd) & _IOC_WRITE)
		retval = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));

	if (retval)
		return -EINVAL;

	esp_dev = (struct esp_device *)filp->private_data;
	if (!esp_dev) {
		ic_debug(ERR_LOG, "%s: esp_dev IS NULL ======\n", __func__);
		return -EINVAL;
	}

	switch (cmd) {
		case ESP_IOC_OPEN_DEV:
			ic_debug(INFO_LOG, "%s: ESP_IOC_OPEN_DEV\n", __func__);
            hct_security_set_down(1);
			esp_hw_power(60, 0);
            hct_security_set_spi_mode(0);
            esp_hw_power(120, 1);
            hct_security_set_spi_mode(1);
            retval = 0;
			break;
		case ESP_IOC_CLOSE_DEV:
			ic_debug(INFO_LOG, "%s: ESP_IOC_CLOSE_DEV\n", __func__);
            hct_security_set_down(0);
			esp_hw_power(60, 0);
			retval = 0;
			break;
		case ESP_IOC_START_SEARCH:
			ic_debug(INFO_LOG, "%s: ESP_IOC_START_SEARCH\n", __func__);
            retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_STOP_SEARCH:
			ic_debug(INFO_LOG, "%s: ESP_IOC_STOP_SEARCH\n", __func__);
			retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_GET_VER:
			ic_debug(INFO_LOG, "%s: ESP_IOC_GET_VER\n", __func__);
			retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_FIRMWARE_UPGRADE:
			ic_debug(INFO_LOG, "%s: ESP_IOC_FIRMWARE_UPGRADE\n", __func__);
			retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_UPDATE_RATE:
			ic_debug(INFO_LOG, "%s: ESP_IOC_UPDATE_RATE\n", __func__);
            retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_GET_DATA:
			ic_debug(INFO_LOG, "%s: ESP_IOC_GET_DATA\n", __func__);
            retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_SPI_EINT:
			ic_debug(INFO_LOG, "%s: ESP_IOC_SPI_EINT\n", __func__);
			ioarg = esp_wait_spi_eint();
			retval = copy_to_user((int*)arg, &ioarg, sizeof(int));
			break;
		case ESP_IOC_SET_FIRMWARE_SIZE:
			ic_debug(INFO_LOG, "%s: ESP_IOC_SET_FIRMWARE_SIZE\n", __func__);
            retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		case ESP_IOC_SET_FIRMWARE_DONE:
			ic_debug(INFO_LOG, "%s: ESP_IOC_SET_FIRMWARE_DONE\n", __func__);
            retval = esp_spi_send(esp_dev, (message_t*)arg);
			break;
		default:
			ic_debug(ERR_LOG, "esp doesn't support this command(%x)\n", cmd);
			break;
	}
	FUNC_EXIT();
	return retval;
}

#ifdef CONFIG_COMPAT
static long esp_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	retval = filp->f_op->unlocked_ioctl(filp, cmd, arg);
	return retval;
}
#endif

static unsigned int esp_poll(struct file *filp, struct poll_table_struct *wait)
{
	ic_debug(ERR_LOG, "Not support poll opertion in TEE version\n");
	return -EFAULT;
}


/* -------------------------------------------------------------------- */
/* devfs                                                              */
/* -------------------------------------------------------------------- */
static ssize_t esp_debug_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	ic_debug(INFO_LOG, "%s: Show debug_level = 0x%x\n", __func__, g_debug_level);
	return sprintf(buf, "vendor id 0x%x\n", g_vendor_id);
}

static ssize_t esp_debug_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count)
{
	struct esp_device *esp_dev =  dev_get_drvdata(dev);
	int retval = 0;
	u8 flag = 0;
	struct mt_spi_t *ms = NULL;

	ms = spi_master_get_devdata(esp_dev->spi->master);

	if (!strncmp(buf, "-8", 2)) {
		ic_debug(INFO_LOG, "%s: parameter is -8, enable spi clock test===============\n", __func__);
		//enable_clk(ms);

	} else if (!strncmp(buf, "-9", 2)) {
		ic_debug(INFO_LOG, "%s: parameter is -9, disable spi clock test===============\n", __func__);
		//disable_clk(ms);

	} else if (!strncmp(buf, "-12", 3)) {
		ic_debug(INFO_LOG, "%s: parameter is -12, GPIO test===============\n", __func__);
		esp_reset_gpio_cfg(esp_dev);

#ifdef CONFIG_OF
	if (flag == 0) {
		pinctrl_select_state(esp_dev->pinctrl_gpios, esp_dev->pins_miso_pulllow);
		ic_debug(INFO_LOG, "%s: set miso PIN to low\n", __func__);
		flag = 1;
	} else {
		pinctrl_select_state(esp_dev->pinctrl_gpios, esp_dev->pins_miso_pullhigh);
		ic_debug(INFO_LOG, "%s: set miso PIN to high\n", __func__);
		flag = 0;
	}
#endif

	} else if (!strncmp(buf, "-13", 3)) {
		ic_debug(INFO_LOG, "%s: parameter is -13, Vendor ID test --> 0x%x\n", __func__, g_vendor_id);
	} else {
		ic_debug(ERR_LOG, "%s: wrong parameter!===============\n", __func__);
	}

	return count;
}

/* -------------------------------------------------------------------- */
/* device function								  */
/* -------------------------------------------------------------------- */
static int esp_open(struct inode *inode, struct file *filp)
{
	struct esp_device *esp_dev = NULL;
	int status = -ENXIO;
	
	FUNC_ENTRY();
	//vcorefs_request_dvfs_opp(KIR_REESPI, OPPI_PERF);
	mutex_lock(&device_list_lock);
	list_for_each_entry(esp_dev, &device_list, device_entry) {
		if (esp_dev->devno == inode->i_rdev) {
			ic_debug(INFO_LOG, "%s, Found\n", __func__);
			status = 0;
			break;
		}
	}
	mutex_unlock(&device_list_lock);
    if (status == 0) {
        filp->private_data = esp_dev;
        nonseekable_open(inode, filp);
    }else {
        ic_debug(ERR_LOG, "%s, No device for esp_dev\n", __func__);
    }
	FUNC_EXIT();
	return status;
}

static int esp_release(struct inode *inode, struct file *filp)
{
	struct esp_device *esp_dev = NULL;
	int    status = 0;

	FUNC_ENTRY();
	esp_dev = filp->private_data;
    if(esp_dev != NULL){
	    esp_dev->need_update = 0;
	}
	FUNC_EXIT();
	return status;
}

static const struct file_operations esp_fops = {
	.owner =	THIS_MODULE,
	/* REVISIT switch to aio primitives, so that userspace
	* gets more complete API coverage.	It'll simplify things
	* too, except for the locking.
	*/
	.write =	esp_write,
	.read =		esp_read,
	.unlocked_ioctl = esp_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = esp_compat_ioctl,
#endif
	.open =		esp_open,
	.release =	esp_release,
	.poll	= esp_poll,
};

static int esp_probe(struct spi_device *spi)
{
	int retval = 0;
	int status = -EINVAL;
	
	if(hct_security_probe_isok)
	{
	    printk(" esp_probe fail, other security already probe ok!!!!\n");
	    return -1;
	}
	FUNC_ENTRY();
	hct_waite_for_security_dts_paser();
    /*enable the power*/
    // hct_security_set_spi_mode(0);
    // esp_hw_power(120, 1);
	hct_security_set_spi_mode(1);
	/* Allocate driver data */
	esp_dev = kzalloc(sizeof(struct esp_device), GFP_KERNEL);
	if (!esp_dev) {
		status = -ENOMEM;
		goto err;
	}
	spin_lock_init(&esp_dev->spi_lock);
	mutex_init(&esp_dev->buf_lock);
	mutex_init(&esp_dev->release_lock);

	INIT_LIST_HEAD(&esp_dev->device_entry);
	esp_dev->device_count     = 0;
	esp_dev->probe_finish     = 0;
	esp_dev->system_status    = 0;
	esp_dev->need_update      = 0;

	/*setup esp configurations.*/
	ic_debug(INFO_LOG, "%s, Setting esp device configuration==========\n", __func__);

	/* Initialize the driver data */
	esp_dev->spi = spi;

	/* setup SPI parameters */
	/* CPOL=CPHA=0, speed 1MHz */
	esp_dev->spi->mode = SPI_MODE_0;
	esp_dev->spi->bits_per_word = 8;
	esp_dev->spi->max_speed_hz = 10 * 1000 * 1000; //4 * 1000 * 1000;
	memcpy(&esp_dev->spi_mcc, &spi_ctrdata, sizeof(struct mt_chip_conf));
	esp_dev->spi->controller_data = (void *)&esp_dev->spi_mcc;
	spi_setup(esp_dev->spi);
	spi_set_drvdata(spi, esp_dev);
	/* allocate buffer for SPI transfer */
	esp_dev->spi_buffer = kzalloc(bufsiz, GFP_KERNEL);
	if (esp_dev->spi_buffer == NULL) {
		status = -ENOMEM;
		goto err_buf;
	}

	esp_init_interrupt();
	//esp_spi_clk_enable(esp_dev, 1);
#ifdef TEST_DATA
	INIT_WORK(&esp_work, wq_func_hrtimer);

	hrtimer_init(&esp_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	esp_timer.function = esp_timer_func;
	ktime = ktime_set( interval/1000000, (interval%1000000)*1000 );  
	hrtimer_start(&esp_timer, ktime, HRTIMER_MODE_REL);
	//hrtimer_cancel(&esp_timer);
#endif
	/* create class */
	esp_dev->class = class_create(THIS_MODULE, ESP_CLASS_NAME);
	if (IS_ERR(esp_dev->class)) {
		ic_debug(ERR_LOG, "%s, Failed to create class.\n", __func__);
		status = -ENODEV;
		goto err_class;
	}
	/* get device no */
	if (ESP_DEV_MAJOR > 0) {
		esp_dev->devno = MKDEV(ESP_DEV_MAJOR, esp_dev->device_count++);
		status = register_chrdev_region(esp_dev->devno, 1, ESP_DEV_NAME);
	} else {
		status = alloc_chrdev_region(&esp_dev->devno, esp_dev->device_count++, 1, ESP_DEV_NAME);
	}
	if (status < 0) {
		ic_debug(ERR_LOG, "%s, Failed to alloc devno.\n", __func__);
		goto err_devno;
	} else {
		ic_debug(INFO_LOG, "%s, major=%d, minor=%d\n", __func__, MAJOR(esp_dev->devno), MINOR(esp_dev->devno));
	}
	/* create device */
	esp_dev->device = device_create(esp_dev->class, &spi->dev, esp_dev->devno, esp_dev, ESP_DEV_NAME);
	if (IS_ERR(esp_dev->device)) {
		ic_debug(ERR_LOG, "%s, Failed to create device.\n", __func__);
		status = -ENODEV;
		goto err_device;
	} else {
		mutex_lock(&device_list_lock);
		list_add(&esp_dev->device_entry, &device_list);
		mutex_unlock(&device_list_lock);
		ic_debug(INFO_LOG, "%s, device create success.\n", __func__);
	}
	/* create sysfs */
	status = sysfs_create_group(&spi->dev.kobj, &esp_debug_attr_group);
	if (status) {
		ic_debug(ERR_LOG, "%s, Failed to create sysfs file.\n", __func__);
		status = -ENODEV;
		goto err_sysfs;
	} else {
		ic_debug(INFO_LOG, "%s, Success create sysfs file.\n", __func__);
	}
	/* cdev init and add */
	cdev_init(&esp_dev->cdev, &esp_fops);
	esp_dev->cdev.owner = THIS_MODULE;
	status = cdev_add(&esp_dev->cdev, esp_dev->devno, 1);
	if (status) {
		ic_debug(ERR_LOG, "%s, Failed to add cdev.\n", __func__);
		goto err_cdev;
	}	

	esp_dev->probe_finish = 1;
	esp_dev->is_sleep_mode = 0;
	ic_debug(INFO_LOG, "%s probe finished\n", __func__);
	//esp_spi_clk_enable(esp_dev, 0);

	hct_security_probe_isok=1;
	FUNC_EXIT();
	return 0;

err_input_2:
	mutex_lock(&esp_dev->release_lock);
	input_free_device(esp_dev->input);
	esp_dev->input = NULL;
	mutex_unlock(&esp_dev->release_lock);

err_input:
	cdev_del(&esp_dev->cdev);

err_cdev:
	sysfs_remove_group(&spi->dev.kobj, &esp_debug_attr_group);

err_sysfs:
	device_destroy(esp_dev->class, esp_dev->devno);
	list_del(&esp_dev->device_entry);

err_device:
	unregister_chrdev_region(esp_dev->devno, 1);

err_devno:
	class_destroy(esp_dev->class);

err_class:
	//esp_spi_clk_enable(esp_dev, 0);
	kfree(esp_dev->spi_buffer);
err_buf:
	mutex_destroy(&esp_dev->buf_lock);
	mutex_destroy(&esp_dev->release_lock);
	spi_set_drvdata(spi, NULL);
	esp_dev->spi = NULL;
	kfree(esp_dev);
	esp_dev = NULL;
err:

	FUNC_EXIT();
	return status;
}

static int esp_remove(struct spi_device *spi)
{
	struct esp_device *esp_dev = spi_get_drvdata(spi);

	FUNC_ENTRY();

	mutex_lock(&esp_dev->release_lock);
	if (esp_dev->input == NULL) {
		kfree(esp_dev);
		mutex_unlock(&esp_dev->release_lock);
		FUNC_EXIT();
		return 0;
	}
	input_unregister_device(esp_dev->input);
	esp_dev->input = NULL;
	mutex_unlock(&esp_dev->release_lock);

	mutex_lock(&esp_dev->release_lock);
	if (esp_dev->spi_buffer != NULL) {
		kfree(esp_dev->spi_buffer);
		esp_dev->spi_buffer = NULL;
	}
	mutex_unlock(&esp_dev->release_lock);

	cdev_del(&esp_dev->cdev);
	sysfs_remove_group(&spi->dev.kobj, &esp_debug_attr_group);
	device_destroy(esp_dev->class, esp_dev->devno);
	list_del(&esp_dev->device_entry);

	unregister_chrdev_region(esp_dev->devno, 1);
	class_destroy(esp_dev->class);
	//esp_spi_clk_enable(esp_dev, 0);

	spin_lock_irq(&esp_dev->spi_lock);
	spi_set_drvdata(spi, NULL);
	esp_dev->spi = NULL;
	spin_unlock_irq(&esp_dev->spi_lock);

	mutex_destroy(&esp_dev->buf_lock);
	mutex_destroy(&esp_dev->release_lock);

	kfree(esp_dev);
	FUNC_EXIT();
	return 0;
}

/*-------------------------------------------------------------------------*/
static struct spi_board_info spi_board_esp_dev[] __initdata = {
	[0] = {
	    .modalias = ESP_DEV_NAME,
	    .bus_num = 0,
	    .chip_select = 0,
	    .mode = SPI_MODE_0,
	    .controller_data = &spi_ctrdata,
	},
};

static struct spi_driver esp_spi_driver = {
	.driver = {
		.name = ESP_DEV_NAME,
		.bus = &spi_bus_type,
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = esp_of_match,
#endif
	},
	.probe = esp_probe,
	.remove = esp_remove,
};

static int __init esp_init(void)
{
	int status = 0;

	FUNC_ENTRY();

	spi_register_board_info(spi_board_esp_dev, ARRAY_SIZE(spi_board_esp_dev));
	
	status = spi_register_driver(&esp_spi_driver);
	if (status < 0) {
		ic_debug(ERR_LOG, "%s, Failed to register SPI driver.\n", __func__);
		return -EINVAL;
	}
	FUNC_EXIT();
	return status;
}

static void __exit esp_exit(void)
{
	FUNC_ENTRY();
	spi_unregister_driver(&esp_spi_driver);
	FUNC_EXIT();
}
late_initcall(esp_init); 
module_exit(esp_exit);

MODULE_AUTHOR("ESP");
MODULE_DESCRIPTION("esp REE driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("spi:security_spi");
