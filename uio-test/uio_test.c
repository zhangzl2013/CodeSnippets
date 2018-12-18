#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>

#define DRIVER_VERSION	"0.01.0"
#define DRIVER_AUTHOR	"Zhaolong Zhang <zhangzl2013@126.com>"
#define DRIVER_DESC	"UIO testing driver"


struct uio_testing_dev {
	struct uio_info info;
	struct device dev;
};

struct uio_testing_dev *g_utdp;

static inline struct uio_testing_dev *
to_uio_testing_dev(struct uio_info *info)
{
	return container_of(info, struct uio_testing_dev, info);
}
static int add_uio_dev(void)
{
	int err;

	printk("UIO_TEST: %s: %d\n", __FUNCTION__, __LINE__);
	err = uio_register_device(&g_utdp->dev, &g_utdp->info);
	if (err) {
		printk("uio device register failed\n");
	}

	return err;
}

static void remove_uio_dev(void)
{
	printk("UIO_TEST: %s: %d\n", __FUNCTION__, __LINE__);
	uio_unregister_device(&g_utdp->info);
}


static int uio_registered;

static ssize_t ctrl_show(struct device *dev,
		 struct device_attribute *attr, char *buf)
{
	printk(KERN_EMERG "UIO_TEST: %s: %d  \n", __FUNCTION__, __LINE__);
	return scnprintf(buf, PAGE_SIZE, "%d\n", uio_registered);
}

static ssize_t ctrl_store(struct device *dev,
		 struct device_attribute *attr, const char *buf, size_t count)
{
	int val;

	printk(KERN_EMERG "UIO_TEST: %s: %d dev:%p, attr:%p, buf:%p, count:%d \n",
			__FUNCTION__, __LINE__, dev, attr, buf, count);

	sscanf(buf, "%du", &val);

	if (val) {
		if (uio_registered) {
			printk(KERN_EMERG "UIO_TEST: %s: %d uio already reigstered. skip. \n",
					__FUNCTION__, __LINE__);
		} else {
			printk(KERN_EMERG "UIO_TEST: %s: %d register uio \n", __FUNCTION__, __LINE__);
			uio_registered = 1;
			add_uio_dev();
		}
	} else {
		if (uio_registered) {
			printk(KERN_EMERG "UIO_TEST: %s: %d unregister uio \n", __FUNCTION__, __LINE__);
			uio_registered = 0;
			remove_uio_dev();
		} else {
			printk(KERN_EMERG "UIO_TEST: %s: %d uio not reigstered yet. skip. \n",
					__FUNCTION__, __LINE__);
		}
	}

	return count;
}

static DEVICE_ATTR(ctrl, S_IRUGO | S_IWUSR | S_IWGRP, ctrl_show, ctrl_store);
/*
static irqreturn_t irqhandler(int irq, struct uio_info *info)
{
	* UIO core will signal the user process. *
	return IRQ_HANDLED;
}
*/

static void uio_testing_release(struct device *dev)
{
	kfree(g_utdp);
}

static int __init uio_testing_init(void)
{
	int err;

	printk("UIO Testing: init\n");

	g_utdp = kzalloc(sizeof(struct uio_testing_dev), GFP_KERNEL);
	if (!g_utdp) {
		printk("failed to kmalloc g_utdp\n");
		err = -ENOMEM;
		goto err_alloc;
	}

	g_utdp->dev.release = uio_testing_release;
	err = dev_set_name(&g_utdp->dev, "uio_test");
	if (err) {
		printk("device set name failed\n");
		goto err_dev_register;
	}
	err = device_register(&g_utdp->dev);
	if (err) {
		printk("device register failed\n");
		goto err_dev_register;
	}

	err = device_create_file(&g_utdp->dev, &dev_attr_ctrl);
	if (err != 0) {
		dev_err(&g_utdp->dev,
				"Failed to create xxx sysfs files: %d\n", err);
		goto err_dev_register;
	}
/*
	err = request_irq(irq, irqhandler, 0, "uio_testing_dev", NULL);
	if (err) {
		printk("request IRQ failed!");
		goto err_register;
	}
*/
	g_utdp->info.name = "uio_testing_device";
	g_utdp->info.version = DRIVER_VERSION;
	g_utdp->info.irq = UIO_IRQ_CUSTOM;



	return 0;

//err_register:
//	device_remove_file(&g_utdp->dev, &dev_attr_ctrl);
//	device_unregister(&g_utdp->dev);
err_dev_register:
	kfree(g_utdp);
err_alloc:
	return err;
}

static void __exit uio_testing_exit(void)
{
	printk("UIO Testing: exit\n");
	if (!g_utdp)
		return;

	device_remove_file(&g_utdp->dev, &dev_attr_ctrl);
	device_unregister(&g_utdp->dev);
}

module_init(uio_testing_init);
module_exit(uio_testing_exit);

MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
