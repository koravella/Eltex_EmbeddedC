#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#define LEN 15

static struct proc_dir_entry *test_proc = NULL;
static char test_string[LEN] = "Hello!\n";
static rwlock_t lock;

ssize_t test_read(struct file *fd, char __user *buf, size_t size,
                  loff_t *off) {
  size_t rc;

  read_lock(&lock);
  rc = simple_read_from_buffer(buf, size, off, test_string, LEN);
  pr_info("Read %ld bytes\n", rc);
  read_unlock(&lock);
  return rc;
}

ssize_t test_write(struct file *fd, const char __user *buf, size_t size,
                   loff_t *off) {
  size_t rc;

  write_lock(&lock);
  rc = simple_write_to_buffer(test_string, LEN, off, buf, size);
  pr_info("Write %ld bytes\n", rc);
  write_unlock(&lock);
  return rc;
}

static const struct proc_ops pops = {
  .proc_read = test_read,
  .proc_write = test_write
};

int init_module(void) {
  pr_info("Test module loaded!\n");
  rwlock_init(&lock);
  
  test_proc = proc_create("test_proc", 0666, NULL, &pops);
  return 0;
}

void cleanup_module(void) {
  proc_remove(test_proc);
  pr_info("Test module unloaded!\n");
}

MODULE_LICENSE("GPL");
