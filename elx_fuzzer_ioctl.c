#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/debugfs.h>
#include <linux/circ_buf.h>
#include <linux/acpi.h>

#include "elx_fuzzer_ioctl.h"

//static int elx_fuzzer_major_num;
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

	if(cdev_add(&elx_fuzzer_cdev, dev_num, 0))
	{
		pr_info("%s: %s cdev_add() failed!\n", ELX_FUZZER_MODULE_NAME, __func__);
		unregister_chrdev_region(dev_num, 1);
	}

	if(IS_ERR(cls = class_create(THIS_MODULE, ELX_FUZZER_CLASS_NAME)))
	{
		pr_info("%s: %s class_create() failed\n", ELX_FUZZER_MODULE_NAME, __func__);
		cdev_del(&elx_fuzzer_cdev);
		unregister_chrdev_region(dev_num, 1);
		return 0;
	}

	if(IS_ERR(dev = device_create(cls, NULL, dev_num, NULL, ELX_FUZZER_MODULE_NAME)))
	{

		pr_info("%s: %s class_create() failed\n", ELX_FUZZER_MODULE_NAME, __func__);
		class_destroy(cls);
		cdev_del(&elx_fuzzer_cdev);
		unregister_chrdev_region(dev_num, 1);
	}

	pr_info("%s: %s suncessfully initialized the driver!\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;
}

static void __exit elx_fuzzer_exit(void)
{
	pr_info("%s: %s unloading module...\n", ELX_FUZZER_MODULE_NAME, __func__);
	device_destroy(cls, dev_num);
	class_destroy(cls);
	cdev_del(&elx_fuzzer_cdev);
	unregister_chrdev_region(dev_num, 1);
	pr_info("%s: %s module is unloaded\n", ELX_FUZZER_MODULE_NAME, __func__);
	return;
}

ssize_t elx_fuzzer_read(struct file *filep, char __user *outbuf, size_t size,
	loff_t *offset)
{
	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;
}

ssize_t elx_fuzzer_write(struct file *filep, const char __user *inbuf,
	size_t size, loff_t *offset)
{
	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);
	return 0;

}
long elx_fuzzer_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	unsigned int target = cmd;
	struct elx_args_s elx_args = {0};
	struct smc_args_s *smc_args = 0;
	//uint64_t i;

	pr_info("%s: %s\n", ELX_FUZZER_MODULE_NAME, __func__);

	if (copy_from_user(&elx_args, (void __user *)arg, sizeof(struct elx_args_s))) {
		pr_info("%s: %s error while filling elx_args struct\n", \
			ELX_FUZZER_MODULE_NAME, __func__);
			return -1;
	}

	/**	if there is no argument or the number of arguments are more than 15
	**	reject the request, because it doesn't make any sense **/
	if (elx_args.num_of_args == 0 || elx_args.num_of_args > 0xF) {
		pr_info("%s: %s num_of_args doesn\'t make sense!\n", \
			ELX_FUZZER_MODULE_NAME, __func__);
			return -1;
	}

	smc_args = (struct smc_args_s *)kmalloc(elx_args.num_of_args * sizeof(struct smc_args_s), GFP_KERNEL);
	if(!smc_args){
		pr_info("%s: %s can\'t allocate memory for smc args array!\n", \
			ELX_FUZZER_MODULE_NAME, __func__);
			return -1;
	}

	if (copy_from_user(smc_args, elx_args.args, elx_args.num_of_args * sizeof(struct smc_args_s))) {
		pr_info("%s: %s error while copying smc_args from userspace\n", \
			ELX_FUZZER_MODULE_NAME, __func__);
			return -1;
	}

	/**
	** do the rest of the argument parsing and formward to tz.
	**/

	switch(target){
		case KPENG_SPECIFIC_OPS:
			break;
		case QTEE_SMC_HANDLERS:
			break;
		case QTEE_APPLICATIONS:
			break;
		default:
			pr_info("%s: %s there is no default fuzzing option. choose a target.\n", ELX_FUZZER_MODULE_NAME, __func__);
			break;
	}

	return 0;
}

struct file_operations elx_fuzzer_fops = {
	.owner = THIS_MODULE,
//	.open = elx_fuzzer_open,
//	.read = elx_fuzzer_read,
//	.write = elx_fuzzer_write,
	.unlocked_ioctl = elx_fuzzer_ioctl
};

module_init(elx_fuzzer_init);
module_exit(elx_fuzzer_exit);
MODULE_AUTHOR("Arash Tohidi");
//MODULE_LISENCE("GPL");
MODULE_DESCRIPTION("A driver to simply fuzz the code living in higher exception levels");
