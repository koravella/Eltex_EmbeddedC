#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/kobject.h>

#define LEN 15

static struct kobject *test_kobj = NULL;
static char test_string[LEN] = "Hello!\n";
static rwlock_t lock;

static ssize_t string_show(struct kobject *kobj, struct kobject_attribute *attr,
                           char *buf) {
  size_t rc;

  read_lock(&lock);
  memcpy(buf, test_string, LEN);
  rc = strlen(test_string);
  pr_info("Read %ld bytes\n", rc);
  read_unlock(&lock);
  return rc;
}

static ssize_t string_store(struct kobject *kobj, struct kobject_attribute *attr,
                            char const *buf, size_t count) {
  size_t rc;

  write_lock(&lock);
  memcpy(test_string, buf, LEN);
  rc = strlen(buf);
  pr_info("Write %ld bytes\n", rc);
  write_unlock(&lock);
  return rc;
}

static struct kobj_attribute string_attribute =
  __ATTR(test_string, 0660, (void *)string_show, (void *)string_store);

static struct attribute *attrs[] = {
  &string_attribute.attr,
  NULL
};

static struct attribute_group attr_group = {
  .attrs = attrs
};

int init_module(void) {
  int ret;

  pr_info("Test module loaded!\n");
  rwlock_init(&lock);
  
  test_kobj = kobject_create_and_add("test_sys", kernel_kobj);
  if (test_kobj == NULL)
    return -ENOMEM;

  ret = sysfs_create_group(test_kobj, &attr_group);
  if (ret) {
    kobject_put(test_kobj);
    return ret;
  }
  return 0;
}

void cleanup_module(void) {
  kobject_put(test_kobj);
  pr_info("Test module unloaded!\n");
}

MODULE_LICENSE("GPL");
