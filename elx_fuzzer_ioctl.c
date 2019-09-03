#include "elx_fuzzer_ioctl.h"


static int elx_fuzzer_major_num;
static struct cdev elx_fuzzer_cdev;
struct file_operations elx_fuzzer_fops;
static dev_t dev_num ;
struct class *cls;
struct device *dev;

static int __init elx_fuzzer_init(void)
{

	if(alloc_chrdev_region(&dev_num, 0, 0, ELX_FUZZER_MODULE_NAME))
	{
		pr_info("%s: %s alloc_chrdev_region() failed\n", ELX_FUZZER_MODULE_NAME, __func__);
		return 0;
	}

	cdev_init(&elx_fuzzer_cdev, &elx_fuzzer_fops);

	if(cdev_add(&&elx_fuzzer_cdev, dev_num, 0))
	{
		pr_info("%s: %s cdev_add() failed!\n", ELX_FUZZER_MODULE_NAME, __func__);
		unregister_chrdev_region(&dev_num, 1);
	}

	if(IS_ERR(cls = class_create(THIS_MODULE, ELX_FUZZER_CLASS_NAME)))
	{
		pr_info("%s: %s class_create() failed\n", ELX_FUZZER_MODULE_NAME, __func__);
		cdev_del(&elx_fuzzer_cdev);
		unregister_chrdev_region(&dev_num, 1);
		return 0;
	}

	if(IS_ERR(dev = device_create(cls, NULL, dev_num, NULL, ELX_FUZZER_MODULE_NAME)))
	{

		pr_info("%s: %s class_create() failed\n", ELX_FUZZER_MODULE_NAME, __func__);
		class_destroy(cls);
		cdev_del(&elx_fuzzer_cdev);
		unregister_chrdev_region(&dev_num, 1);
	}

	pr_info("%s: %s suncessfully initialized the driver!\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;
}

static void __exit elx_fuzzer_exit(void)
{
	pr_info("%s: %s unloading module...\n", ELX_FUZZER_MODULE_NAME, __func__);
	device_destroy(dev);
	class_destroy(cls);
	cdev_del(&elx_fuzzer_cdev);
	unregister_chrdev_region(&dev_num, 1);
	pr_info("%s: %s module is unloaded\n", ELX_FUZZER_MODULE_NAME, __func__);
	return;
}
ssize_t elx_fuzzer_read(struct file *filep, char __user *outbuf, size_t size, loff_t *offset)
{
	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;
}

ssize_t elx_fuzzer_write(struct file *filep, const char __user *inbuf, size_t size, loff_t *offset)
{
	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;

}
long elx_fuzzer_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	unsigned int target = cmd;
	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);

	switch(target){
		case KEPNG_SPECIFIC_OPS:
			pr_info("%s: %s fuzzing kpeng specific operations\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
		case KPENG_TRAPPED_OPS:
			pr_info("%s: %s fuzzing kpeng trapped operations in kpeng\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
		case QTEE_SMC_HANDLERS:
			pr_info("%s: %s fuzzing qtee smc handlers\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
		case QTEE_APPLICATIONS:
			pr_info("%s: %s fuzzing trustzone applications\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
		case QHEE_SPECIFIC_OPS:
			pr_info("%s: %s fuzzing qhee specific operations\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
		default:
			pr_info("%s: %s there is no default fuzzing option. choose a target.\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
	}

	return 0;
}


elx_fuzzer_fops = {
	.owner = THIS_MODULE,
	.open = elx_fuzzer_open,
	.read = elx_fuzzer_read,
	.write = elx_fuzzer_write,
	.unlocked_ioctl = elx_fuzzer_ioctl
};

module_init(elx_fuzzer_init);
module_exit(elx_fuzzer_exit);
MODULE_AUTHOR("Arash Tohidi");
MODULE_LISENCE("GPL");
MODULE_DESCRIPTION("A driver to simply fuzz the code living in higher exception levels");
