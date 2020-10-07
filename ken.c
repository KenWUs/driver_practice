#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h> //cdev_add() / cdev_del()
#include <linux/uaccess.h> //copy_*_user()
#include "ioct.h"
#define EXAMPLE_MAJOR 60
#define EXAMPLE_NAME "ken"

static int dev_major;
static int dev_minor;
struct cdev *dev_cdevp = NULL;
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static long dev_ioctl(struct file*, unsigned int, unsigned long);
ssize_t dev_read(struct file*, char __user*, size_t, loff_t*);
ssize_t dev_write(struct file*, const char __user*, size_t, loff_t*);
static void __exit exit_modules(void);

static struct file_operations dev_fops = {
   .owner = THIS_MODULE,
   .open = dev_open,
   .release= dev_release,
   .unlocked_ioctl = dev_ioctl,
   .read = dev_read,
   .write = dev_write
};
static int dev_open(struct inode *inode, struct file *filp) {
    printk("%s():\n",__FUNCTION__);
    return 0;
}

static int dev_release(struct inode *inode, struct file *filp) {
    printk("%s():\n",__FUNCTION__);
    return 0;
}

static int val = 0;
static long dev_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
      int tmp, err=0;
      if(_IOC_TYPE(cmd) != IOC_MAGIC)
              return -ENOTTY;
      if(_IOC_NR(cmd) > IOC_MAXNR)
              return -ENOTTY;
      if(_IOC_DIR(cmd) & _IOC_READ){
              err = !access_ok(VERIFY_WRITE, (void __user*)args, _IOC_SIZE(cmd));
      } else if (_IOC_DIR(cmd) & (_IOC_WRITE)){
              err = !access_ok(VERIFY_READ, (void __user *)args,_IOC_SIZE(cmd));
      }
      if (err)
              return -EFAULT;
      switch (cmd) {
        case SETNUM:
             //copy data from args(user) to val(kernel)
             if(copy_from_user(&val,(int __user *)args,1))
                 return -1;

             printk("%s():get val from user = %d\n",__FUNCTION__,val);
                   break;
        case GETNUM:
                        //copy data from val to args
              if(copy_to_user((int __user *)args,&val,1))
                    return -1;

              printk("%s():set val to %d\n",__FUNCTION__, val);
                    break;
        case XNUM:
                       // exchange data passed by user
              tmp = val;
              if(copy_from_user(&val,(int __user *)args,1))
                        return -1;

              if(copy_to_user((int __user *)args,&val,1))
                        return -1;

           printk("%s():change val from %d to %d\n",__FUNCTION__,tmp,val);
           break;

        default: /* redundant. as cmd was checked against MAXNR */
                 return -ENOTTY;
     }
     return 0;
}

static const char str[]="Hello Irene from kernel!";
static const ssize_t str_size = sizeof(str);
// send messages to user
ssize_t dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

     printk("%s():\n",__FUNCTION__);

     if( *f_pos >= str_size)
          return 0;
     if( *f_pos + count > str_size)
        count = str_size - *f_pos;
     if (copy_to_user(buf, str + *f_pos, count))
        return -EFAULT;
     *f_pos += count;
     return count;

}

ssize_t dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
     printk("%s():\n",__FUNCTION__);
     return 0;
}

/*static ssize_t example_read(struct file *filp, char *buf, size_t size, loff_t *f_pos) {
    printk("<1>EXAMPLE: read  (size=%zu)\n", size);
    return 0;
}

static ssize_t example_write(struct file *filp, const char *buf, size_t size, loff_t *f_pos) {
    size_t pos;
    uint8_t byte;
    printk("<1>EXAMPLE: write  (size=%zu)\n", size);
    for (pos = 0; pos < size; ++pos) {
        if (copy_from_user(&byte, buf + pos, 1) != 0) {
            break;
        }
        printk("<1>EXAMPLE: write  (buf[%zu] = %02x)\n", pos, (unsigned)byte);
    }
    return pos;
}*/

static int __init init_modules(void)
{ 
   dev_t dev;
   int ret;
   // get major number dynamically
   ret = alloc_chrdev_region(&dev, 0, 1, "mydev");
   if (ret <0){
      printk("can't alloc chrdev\n");
      return ret;
   }
   dev_major = MAJOR(dev);
   dev_minor = MINOR(dev);
   printk("register chrdev(%d,%d)\n",dev_major,dev_minor);

   dev_cdevp = kmalloc(sizeof(struct cdev), GFP_KERNEL);
   if (dev_cdevp == NULL){
        printk("kmalloc failed\n");
        goto failed;
   }
   // system call handler
   cdev_init(dev_cdevp, &dev_fops);
   dev_cdevp->owner = THIS_MODULE;
   //register my device to kernel
   ret = cdev_add(dev_cdevp, MKDEV(dev_major, dev_minor),1);
   if(ret < 0){
        printk("add chr dev failed\n");
        goto failed;
   }
   return 0;

failed:
   if(dev_cdevp) {
   kfree(dev_cdevp);
   dev_cdevp = NULL;
}
  return 0;
}

static void __exit exit_modules(void)
{
   dev_t dev;
   dev = MKDEV(dev_major, dev_minor);
   if (dev_cdevp){
      cdev_del(dev_cdevp);
      kfree(dev_cdevp);
    }
   unregister_chrdev_region(dev,1);
   printk("unregister chrdev\n");
}

/*static int __init init_modules(void)
{
    int result;
    printk("<1>EXAMPLE: init\n");

    // Register character device 
    result = register_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME, &example_fops);
    if (result < 0) {
        printk("<1>EXAMPLE: Failed to register character device\n");
        return result;
    }

	printk(KERN_INFO "Hello, world 4\n");
	return 0;
}
static void __exit exit_modules(void)
{
	printk("<1>EXAMPLE: exit\n");

    // Unregister character device 
    unregister_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME);
	printk(KERN_INFO "Goodbye, world 4\n");
}*/
module_init(init_modules);
module_exit(exit_modules);
MODULE_LICENSE("GPL");
