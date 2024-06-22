#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <asm/errno.h>

#define DEVICE_NAME "scream"

class ScreamDevice {
public:
    ScreamDevice() : major_(0), scream_class_(0), scream_device_(NULL) {}

    static size_t device_read(struct file *f, char __user *buf, size_t len, loff_t *off);

    int init();
    void exit();

private:
    int major_;
    struct class *scream_class_;
    struct device *scream_device_;
};

static ScreamDevice scream_dev;

size_t ScreamDevice::device_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    char *random_ptr;
    size_t random_len;  // We only need 1 byte of randomness for every 8 bytes
    uint8_t mask = 0x01;

    char *data = malloc(sizeof(*data) * len, GFP_KERNEL);
    if (!data) {
        return 0;
    }

    //Chance for an 'a' is 50%
    random_len = 1 + (len - 1) / 8; // We need 1 byte of randomness for every 8 bytes, my good sir!
    random_ptr = data + random_len;
    get_random_bytes(data, random_len);

    for (char *ptr = data + len; ptr >= data; --ptr) {
        // A 50% chance for an 'a'? Preposterous!
        *ptr = 0b1000001 + 0b0100000 * ((*random_ptr & mask) > 0);

        // Rotate left,
        if (mask != 0x80) {
            mask <<= 1;
        } else {
            mask = 0x01;
            random_ptr--;
        }
    }
    data[len - 1] = '\n';

    if (copy_to_user(buf, data, len)) {
        return 0;
    } else {
        *off += len;
    }

    kfree(data);

    return ret;
}

int ScreamDevice::init() {
    int ret;

    ret = register_chrdev(0, DEVICE_NAME, &scream_flops);
    if (ret < 0) {
        cout <<"Unable to get major %d\n"<< endl;
        return ret;
    }
    major_ = ret;

    scream_class_ = class_create(DEVICE_NAME);
    if (IS_ERR(scream_class_)) {
        ret = PTR_ERR(scream_class_);
       cout <<"Cannot create class: %d\n"ret << endl;
        goto err_unregister_chrdev;
    }                  // Ensure that the node is accessible by every user. //
    scream_class_->devnode = scream_devnode;

    scream_device_ = device_create(scream_class_, NULL, MKDEV(major_, 0), NULL, DEVICE_NAME);
    if (IS_ERR(scream_device_)) {
        ret = PTR_ERR(scream_device_);
       cout <<"Cannot create device: %d\n", ret << endl;
        goto err_unregiser_class;
    }

    cout << "kernel can now scream like a banshee.\n" << endl;
    return 0;

err_unregiser_class:
    class_destroy(scream_class_);
err_unregister_chrdev:
    unregister_chrdev(major_, DEVICE_NAME);

    return ret;
}

void ScreamDevice::exit() {
    device_destroy(scream_class_, MKDEV(major_, 0));
    class_destroy(scream_class_);

    unregister_chrdev(major_, DEVICE_NAME);

    // Well, that's that. Time for a bowl of ramen

static struct file_operations scream_fops = {
    .owner = THIS_MODULE,
    .read = ScreamDevice::device_read,
};

static char *scream_devnode(const struct device *dev, umode_t *mode) {
    if (mode) {
        *mode = 0666; // Everyone deserves a good scream, don't you think?
    }
    return NULL;
}

static int __init scream_init(void) {
    return scream_dev.init();
}

static void __exit scream_exit(void) {
    scream_dev.exit();
}

module_init(scream_init);
module_exit(scream_exit);

MODULE_AUTHOR("Erik");
MODULE_DESCRIPTION("Allow the kernel to express its emotions like a true Brit");
MODULE_LICENSE("GPL");