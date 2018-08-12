#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/keyboard.h>
#include <linux/fs.h>
#include <asm/errno.h>

#define KEYS_SIZE 510

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zee");
MODULE_DESCRIPTION("A Simple module that provides the key state for keyboard and mouse");
MODULE_VERSION("1");

//file operations
static ssize_t KSread(struct file *file, char *buf, size_t size, loff_t *off);
static loff_t KSseek(struct file *file, loff_t off, int whence);

//keyboard event
static int kbev(struct notifier_block *nblock, unsigned long code, void *_param);

static char keys[KEYS_SIZE] = {0};

//device file
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = &KSread,
	.llseek = &KSseek
};
static struct miscdevice device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "keyState",
	.fops = &fops,
	.mode = S_IRUGO
};
//was device created?
static int deviceCreated = 0;
//was keyboard notifier registered?
static int kbReg = 0;

//keyboard notifier
static struct notifier_block nb = {
	.notifier_call = &kbev
};

static int __init modInit(void){
	if(misc_register(&device)){
		printk(KERN_WARNING "failed to create misc device\n");
		return -1;
	}
	deviceCreated = 1;
	register_keyboard_notifier(&nb);
	kbReg = 1;
	printk(KERN_INFO "loaded keyState module successfully, created /dev/keyState\n");
	return 0;
}

static void __exit modExit(void){
	if(deviceCreated){
		misc_deregister(&device);
	}
	if(kbReg){
		unregister_keyboard_notifier(&nb);
	}
	printk(KERN_INFO "successfully unloaded keyState module\n");
}

static ssize_t KSread(struct file *file, char *buf, size_t size, loff_t *off){
	put_user(keys[file->f_pos], buf);
	return 1;
}

static loff_t KSseek(struct file *file, loff_t off, int whence){
	loff_t newpos;
	switch(whence){
		case SEEK_SET:
			newpos = off;
			break;
		case SEEK_CUR:
			newpos = file->f_pos + off;
			break;
		case SEEK_END:
			newpos = KEYS_SIZE + off;
			break;
		default: //literally not possbile, but whatever...
			return -EINVAL;
	}
	if(newpos < 0 || newpos > KEYS_SIZE){
		return -EINVAL;
	}
	file->f_pos = newpos;
	return newpos;
}

static int kbev(struct notifier_block *nblock, unsigned long code, void *_param){
	struct keyboard_notifier_param *param = _param;
	if(code == KBD_KEYCODE && param->value < KEYS_SIZE){
		keys[param->value] = param->down;
	}
	return 0;
}

module_init(modInit);
module_exit(modExit);
