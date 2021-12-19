#include <linux/platform_device.h>
#include <linux/of_mdio.h>
#include <linux/cdev.h>

#define MVMDIO_DEV_NAME "mvmdio-uio"
#define MAX_MDIO_BUS 8

static struct mii_bus *mv_mii_buses[MAX_MDIO_BUS];

typedef struct s_mii_data {
	int bus_id;
        int phy_id;
        int reg;
        u16 data;
} mii_data;

static dev_t dev_num;
static struct cdev mv_cdev;
static struct class *mv_cl;
static int retVal;

/* Create character device */
static int mv_mdio_device_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t mv_mdio_device_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	mii_data mii;

	if(copy_from_user(&mii, (mii_data *)buf, sizeof(mii_data))){
		pr_err("%s: copy_from_user failed\n", MVMDIO_DEV_NAME);
		return -1;
	}

	if(mv_mii_buses[mii.bus_id] == NULL) {
		pr_err("%s: invalid bus_id\n", MVMDIO_DEV_NAME);
		return -1;
	}

	ret = mdiobus_read(mv_mii_buses[mii.bus_id], mii.phy_id, mii.reg);
	if (ret < 0) {
		pr_err("%s: smi read failed at Bus: %X, devAddr: %X, regAddr: %X\n", MVMDIO_DEV_NAME,
		       mii.bus_id, mii.phy_id, mii.reg);
		return -1;
	}
	mii.data = (u16)ret;

	if(copy_to_user((mii_data *)buf, &mii, sizeof(mii_data))){
		pr_err("%s: copy_to_user failed\n", MVMDIO_DEV_NAME);
		return -1;
	}

	return sizeof(mii_data);
}

static ssize_t mv_mdio_device_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	mii_data mii;

	if(copy_from_user(&mii, (mii_data *)buf, sizeof(mii_data))){
		pr_err("%s: copy_from_user failed\n", MVMDIO_DEV_NAME);
		return -1;
	}

	if(mv_mii_buses[mii.bus_id] == NULL) {
		pr_err("%s: invalid bus_id\n", MVMDIO_DEV_NAME);
		return -1;
	}

	ret = mdiobus_write(mv_mii_buses[mii.bus_id], mii.phy_id, mii.reg, mii.data);
	if (ret < 0) {
		pr_err("%s: smi write failed at bus: %X, devAddr: %X, regAddr: %X\n", MVMDIO_DEV_NAME,
		       mii.bus_id, mii.phy_id, mii.reg);
		return -1;
	}

	return sizeof(mii_data);
}

static int mv_mdio_device_close(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations fops =
{
	.owner                = THIS_MODULE,
	.open                 = mv_mdio_device_open,
	.read                 = mv_mdio_device_read,
	.write                = mv_mdio_device_write,
	.release              = mv_mdio_device_close,
};

static int mv_mdio_device_init(void)
{
	struct device_node *np;
        struct device_node *mdio;
	static struct mii_bus *mv_mii_bus;

	int bus_count = 0;

	memset(mv_mii_buses, 0, sizeof(mv_mii_buses));
	for_each_compatible_node(np, NULL, "marvell,mvmdio-uio") {
		if(bus_count == 8) {
			break;
		}

		mdio = of_parse_phandle(np, "mii-bus", 0);
		if (mdio == NULL ) {
			pr_err("%s: parse handle failed\n", MVMDIO_DEV_NAME);
			continue;
		}
		mv_mii_bus = of_mdio_find_bus(mdio);
		if (mv_mii_bus == NULL) {
			pr_err("%s:: mdio find bus failed\n", MVMDIO_DEV_NAME);
			continue;
		}
		pr_info("%s: bus %d added at %s\n",
			MVMDIO_DEV_NAME, bus_count, mdio->name);
		mv_mii_buses[bus_count++] = mv_mii_bus;
	}

	if(bus_count == 0) {
		pr_err("%s: no useful mdio bus found\n", MVMDIO_DEV_NAME);
		return -ENODEV;
	}

	retVal = alloc_chrdev_region(&dev_num, 0, 1, MVMDIO_DEV_NAME);
	if(retVal < 0) {
		goto failed;
	}

	if((mv_cl = class_create(THIS_MODULE, "chardev")) == NULL) {
		unregister_chrdev_region(dev_num, 1);
		goto failed;
	}

	if(device_create(mv_cl, NULL, dev_num, NULL, MVMDIO_DEV_NAME) == NULL) {
		class_destroy(mv_cl);
		unregister_chrdev_region(dev_num, 1);
	}

	cdev_init(&mv_cdev, &fops);

	if( cdev_add(&mv_cdev, dev_num, 1) == -1) {
		device_destroy(mv_cl, dev_num);
		class_destroy(mv_cl);
		unregister_chrdev_region(dev_num, 1);
		goto failed;
	}

	return 0;

failed:
	pr_err("%s: driver registration failed\n", MVMDIO_DEV_NAME);
	return -ENODEV;
}

static void mv_mdio_device_exit(void)
{
	device_destroy(mv_cl, dev_num);
	class_destroy(mv_cl);
	unregister_chrdev_region(dev_num, 1);
}

module_init(mv_mdio_device_init);
module_exit(mv_mdio_device_exit);

MODULE_DESCRIPTION("Marvell MDIO uio driver");
MODULE_LICENSE("GPL");
