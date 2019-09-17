#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/compat.h>
#include <linux/platform_device.h>
#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#endif
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include "mtk_spi.h"

#include <linux/hct_include/hct_project_all_config.h>


DECLARE_WAIT_QUEUE_HEAD(security_init_waiter);

int hct_security_probe_isok = 0;

struct pinctrl *hct_security_pinctrl;
struct pinctrl_state *hct_security_reset_high,*hct_security_reset_low,*hct_security_spi0_mi_as_spi0_mi,*hct_security_spi0_mi_as_gpio,
*hct_security_spi0_mo_as_spi0_mo,*hct_security_spi0_mo_as_gpio,*hct_security_spi0_clk_as_spi0_clk,*hct_security_spi0_clk_as_gpio,
*hct_security_spi0_cs_as_spi0_cs,*hct_security_spi0_cs_as_gpio,*hct_security_eint_en0, *hct_security_down_high,*hct_security_down_low;

int hct_security_get_gpio_info(struct platform_device *pdev)
{
	struct device_node *node;
	int ret;
	node = of_find_compatible_node(NULL, NULL, "mediatek,hct_security");
	printk("node.name %s full name %s",node->name,node->full_name);

	wake_up_interruptible(&security_init_waiter);

	hct_security_pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(hct_security_pinctrl)) {
		ret = PTR_ERR(hct_security_pinctrl);
		dev_err(&pdev->dev, "hct_security cannot find pinctrl\n");
		return ret;
	}

	printk("[%s] hct_securityr_pinctrl+++++++++++++++++\n",pdev->name);

	hct_security_reset_high = pinctrl_lookup_state(hct_security_pinctrl, "security_reset_en1");
	if (IS_ERR(hct_security_reset_high)) {
		ret = PTR_ERR(hct_security_reset_high);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_reset_high!\n");
		return ret;
	}
	hct_security_reset_low = pinctrl_lookup_state(hct_security_pinctrl, "security_reset_en0");
	if (IS_ERR(hct_security_reset_low)) {
		ret = PTR_ERR(hct_security_reset_low);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_reset_low!\n");
		return ret;
	}
	hct_security_down_high = pinctrl_lookup_state(hct_security_pinctrl, "security_down_en1");
	if (IS_ERR(hct_security_down_high)) {
		ret = PTR_ERR(hct_security_down_high);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_down_high!\n");
		return ret;
	}
	hct_security_down_low = pinctrl_lookup_state(hct_security_pinctrl, "security_down_en0");
	if (IS_ERR(hct_security_down_low)) {
		ret = PTR_ERR(hct_security_down_low);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_down_low!\n");
		return ret;
	}
	hct_security_spi0_mi_as_spi0_mi = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_mi_as_spi0_mi");
	if (IS_ERR(hct_security_spi0_mi_as_spi0_mi)) {
		ret = PTR_ERR(hct_security_spi0_mi_as_spi0_mi);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_mi_as_spi0_mi!\n");
		return ret;
	}
	hct_security_spi0_mi_as_gpio = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_mi_as_gpio");
	if (IS_ERR(hct_security_spi0_mi_as_gpio)) {
		ret = PTR_ERR(hct_security_spi0_mi_as_gpio);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_mi_as_gpio!\n");
		return ret;
	}
	hct_security_spi0_mo_as_spi0_mo = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_mo_as_spi0_mo");
	if (IS_ERR(hct_security_spi0_mo_as_spi0_mo)) {
		ret = PTR_ERR(hct_security_spi0_mo_as_spi0_mo);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_mo_as_spi0_mo!\n");
		return ret;
	}
	hct_security_spi0_mo_as_gpio = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_mo_as_gpio");
	if (IS_ERR(hct_security_spi0_mo_as_gpio)) {
		ret = PTR_ERR(hct_security_spi0_mo_as_gpio);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_mo_as_gpio!\n");
		return ret;
	}
	hct_security_spi0_clk_as_spi0_clk = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_clk_as_spi0_clk");
	if (IS_ERR(hct_security_spi0_clk_as_spi0_clk)) {
		ret = PTR_ERR(hct_security_spi0_clk_as_spi0_clk);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_clk_as_spi0_clk!\n");
		return ret;
	}
	hct_security_spi0_clk_as_gpio = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_clk_as_gpio");
	if (IS_ERR(hct_security_spi0_clk_as_gpio)) {
		ret = PTR_ERR(hct_security_spi0_clk_as_gpio);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_clk_as_gpio!\n");
		return ret;
	}
	hct_security_spi0_cs_as_spi0_cs = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_cs_as_spi0_cs");
	if (IS_ERR(hct_security_spi0_cs_as_spi0_cs)) {
		ret = PTR_ERR(hct_security_spi0_cs_as_spi0_cs);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_cs_as_spi0_cs!\n");
		return ret;
	}
	hct_security_spi0_cs_as_gpio = pinctrl_lookup_state(hct_security_pinctrl, "security_spi0_cs_as_gpio");
	if (IS_ERR(hct_security_spi0_cs_as_gpio)) {
		ret = PTR_ERR(hct_security_spi0_cs_as_gpio);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_spi0_cs_as_gpio!\n");
		return ret;
	}
	hct_security_eint_en0 = pinctrl_lookup_state(hct_security_pinctrl, "security_eint_en0");
	if (IS_ERR(hct_security_eint_en0)) {
		ret = PTR_ERR(hct_security_eint_en0);
		dev_err(&pdev->dev, " Cannot find hct_security pinctrl hct_security_eint_en0!\n");
		return ret;
	}
	printk("hct_security get gpio info ok--------");
	return 0;
}

unsigned int hct_security_get_irq_gpio(void){
    struct device_node *node = NULL;
    node = of_find_compatible_node(NULL, NULL, "mediatek,hct_security");
    return irq_of_parse_and_map(node, 0);
}

int hct_security_set_irq(int cmd)
{
    if(IS_ERR(hct_security_eint_en0)){
		pr_err( "err: hct_security_eint_en0 is error!!!!");
		return -1;
	}	

	switch (cmd){
	case 0:		
		pinctrl_select_state(hct_security_pinctrl, hct_security_eint_en0);
		break;
	}
	return 0;
}

int hct_security_set_reset(int cmd)
{

	if(IS_ERR(hct_security_reset_low)||IS_ERR(hct_security_reset_high)){
		pr_err( "err: hct_security_reset_low or hct_security_reset_high is error!!!");
		return -1;
	}

	switch (cmd){
		case 0 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_reset_low);
			break;
		case 1 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_reset_high);
			break;
	}
	return 0;
}

int hct_security_set_down(int cmd)
{

	if(IS_ERR(hct_security_down_low)||IS_ERR(hct_security_down_high)){
		pr_err( "err: hct_security_down_low or hct_security_down_high is error!!!");
		return -1;
	}

	switch (cmd){
		case 0 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_down_low);
			break;
		case 1 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_down_high);
			break;
	}
	return 0;
}

unsigned int hct_security_get_reset_gpio(void){
    struct device_node *node = NULL;
    node = of_find_compatible_node(NULL, NULL, "mediatek,hct_security");
    return of_get_named_gpio(node, "reset-gpio", 0);
}

int hct_security_set_spi_mode(int cmd)
{

	if(IS_ERR(hct_security_spi0_clk_as_gpio)||IS_ERR(hct_security_spi0_cs_as_gpio)||IS_ERR(hct_security_spi0_mi_as_gpio) \
		||IS_ERR(hct_security_spi0_mo_as_gpio)||IS_ERR(hct_security_spi0_clk_as_spi0_clk)||IS_ERR(hct_security_spi0_cs_as_spi0_cs) \
		||IS_ERR(hct_security_spi0_mi_as_spi0_mi)||IS_ERR(hct_security_spi0_mo_as_spi0_mo)){
		 pr_err( "err: hct_security_reset_low or hct_security_reset_high is error!!!");
		 return -1;
	}	

	switch (cmd){
		case 0 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_clk_as_gpio);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_cs_as_gpio);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_mi_as_gpio);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_mo_as_gpio);
		break;
		case 1 : 		
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_clk_as_spi0_clk);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_cs_as_spi0_cs);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_mi_as_spi0_mi);
			pinctrl_select_state(hct_security_pinctrl, hct_security_spi0_mo_as_spi0_mo);
		break;
	}
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id hct_security_match[] = {
	{ .compatible = "mediatek,hct_security", },
	{}
};
MODULE_DEVICE_TABLE(of, hct_security_match);
#endif

static struct platform_device *hct_security_plat = NULL;


void hct_waite_for_security_dts_paser(void)
{
    if(hct_security_plat==NULL){
        wait_event_interruptible_timeout(security_init_waiter, hct_security_plat != NULL, 3 * HZ);
    }else{

	}
	return;
}

int hct_security_plat_probe(struct platform_device *pdev) {
	hct_security_plat = pdev;
	hct_security_get_gpio_info(pdev);
	return 0;
}

int hct_security_plat_remove(struct platform_device *pdev) {
	hct_security_plat = NULL;
	return 0;
} 


#ifndef CONFIG_OF
static struct platform_device hct_security_dev = {
	.name	  = "hct_security",
	.id		  = -1,
};
#endif


static struct platform_driver hct_security_pdrv = {
	.probe	  = hct_security_plat_probe,
	.remove	 = hct_security_plat_remove,
	.driver = {
		.name  = "hct_security",
		.owner = THIS_MODULE,			
#ifdef CONFIG_OF
		.of_match_table = hct_security_match,
#endif
	}
};


static int __init hct_security_init(void)
{
#ifndef CONFIG_OF
    int retval=0;
    retval = platform_device_register(&hct_security_dev);
    if (retval != 0){
        return retval;
    }
#endif
    if(platform_driver_register(&hct_security_pdrv))
    {
    	printk("failed to register driver");
    	return -ENODEV;
    }
    
    return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit hct_security_exit(void)
{
	platform_driver_unregister(&hct_security_pdrv);
}


rootfs_initcall(hct_security_init);
module_exit(hct_security_exit);

MODULE_AUTHOR("hct_drv");
MODULE_DESCRIPTION("for hct security driver");
MODULE_LICENSE("GPL");


