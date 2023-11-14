#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rwlock.h>
#include <linux/string.h>
#include <linux/fs.h>

#define LENGTH 15

static int major = 0;
static rwlock_t lock;
static char test_string[LENGTH] = "Hello\n";

ssize_t test_read(struct file *fd, char __user *buff, size_t size,
                  loff_t *off) {
  size_t rc = 0;

  read_lock(&lock);
  rc = simple_read_from_buffer(buff, size, off, test_string, LENGTH);
  pr_info("Read %ld bytes\n", rc);
  read_unlock(&lock);
  return rc;
}

ssize_t test_write(struct file *fd, const char __user *buff, size_t size,
                loff_t *off) {
 size_t rc = 0;

 if (size > LENGTH)
   return -EINVAL;

  write_lock(&lock);
  rc = simple_write_to_buffer(test_string, LENGTH, off, buff, size);
  pr_info("Write %ld bytes\n", rc);
  write_unlock(&lock);
  return rc;
}

static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = test_read,
  .write = test_write
};

int init_module(void) {
  pr_info("Test module loaded\n");
  rwlock_init(&lock);

  major = register_chrdev(major, "test_chardev", &fops);
  if (major < 0)
    return major;
  pr_info("Major = %d\n", major);
  return 0;
}

void cleanup_module(void) {
  unregister_chrdev(major, "test_chardev");
  pr_info("Test module unloaded\n");
}

MODULE_LICENSE("GPL");
