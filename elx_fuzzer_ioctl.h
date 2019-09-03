#ifndef _ELX_FUZZER_IOCTL_H_
#define _ELX_FUZZER_IOCTL_H_
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/cdev.h>

#define ELX_FUZZER_CLASS_NAME "fuzz"
#define ELX_FUZZER_MODULE_PATH "/dev/elx_fuuzer"
#define ELX_FUZZER_MODULE_NAME "elx_fuzzer"

#define KPENG_OPS_MAJOR 0x10000000
#define QTEE_OPS_MAJOR  0x20000000
#define MISC_OPS_MAJOR  0x30000000


#define KPENG_SPECIFIC_OPS (KPENG_OPS_MAJOR | 0x1)
#define KPENG_TRAPPED_OPS  (KPENG_OPS_MAJOR | 0x2)
#define QTEE_SMC_HANDLERS  (QTEE_OPS_MAJOR  | 0x1)
#define QTEE_APPLICATIONS  (QTEE_OPS_MAJOR  | 0x2)
#define QHEE_SPECIFIC_OPS  (MISC_OPS_MAJOR  | 0x1)

static int __init elx_fuzzer_init(void);
static void __exit elx_fuzzer_exit(void);
ssize_t elx_fuzzer_read(struct file *filep, char __user *outbuf, size_t size, loff_t *offset);
ssize_t elx_fuzzer_write(struct file *filep, const char __user *inbuf, size_t size, loff_t *offset);
long elx_fuzzer_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);


#endif /* ELX_FUZZER_IOCTL_H */
