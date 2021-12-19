#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE "/dev/mvmdio-uio"

/* Following define are from linux kernel include/linux/phy.h */
/* Or MII_ADDR_C45 into regnum for read/write on mii_bus to enable the 21 bit
   IEEE 802.3ae clause 45 addressing mode used by 10GIGE phy chips. */
#define MII_ADDR_C45 (1<<30)

static int fd;

typedef struct s_mii_data {
	int busId;
	int phyId;
	int reg;
	unsigned short data;
} mii_data;


int smiOpen(void)
{
	fd = open(DEVICE, O_RDWR);
	if(fd == -1){
		printf("Error: Cannot open device: %s\n", DEVICE);
		return -1;
	}
	return 0;
}

void smiClose(void)
{
	if(fd != -1) close(fd);
}

int readRegister(
	int busId,
	int devAddr,
	int regAddr)
{
	int retVal;
	mii_data mii;

	mii.busId = busId;
	mii.phyId = devAddr;
	mii.reg   = regAddr;
	retVal = read(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	return (int)mii.data;
}

int writeRegister(
	int busId,
	int devAddr,
	int regAddr,
	unsigned short data)
{
	int retVal;
	mii_data mii;

	/* Write to SMI Command Register */
	mii.busId = busId;
	mii.phyId = devAddr;
	mii.reg   = regAddr;
	mii.data = data;

	retVal = write(fd, (char*)&mii, sizeof(mii));
	if(retVal < 0)
	{
		printf("Error: Accessing SMI reg devAddr: %X, regAddr: %X\n", devAddr, regAddr);
		return -1;
	}

	return retVal;
}

void show_usage(char *program_name)
{
	printf("Usage:\n");
	printf("\tread claus 22: %s 22 bus phy reg\n", program_name);
	printf("\twrite claus 22: %s 22 bus phy reg data\n", program_name);
	printf("\tread claus 45: %s 45 bus phy device reg\n", program_name);
	printf("\twrite claus 45: %s 45 bus phy device reg data\n", program_name);

}

int main(int argc, char *argv[])
{
	int bus, phy, dev, reg, clause;
	unsigned short data;
	int c45_addr;
	int usage = 1;

	if(argc >=2){
		clause = atoi(argv[1]);
		if(((clause == 22) && argc >= 5) || ((clause == 45) && argc >= 6)){
			usage = 0;
			if(smiOpen()){
				return -2;
			}
			bus = strtoul(argv[2], NULL, 16);
			phy = strtoul(argv[3], NULL, 16);
			if(clause == 22){
				reg = strtoul(argv[4], NULL, 16);
				if(argc == 5){
					data = readRegister(bus, phy, reg);
					printf("read22 bus:%x, phy:%02x, reg:%04x = %04x\n",
						bus, phy, reg, data);
				}else{
					data = strtoul(argv[5], NULL, 16);
					printf("write22 %04x to bus:%x, phy:%02x, reg:%04x\n",
						data, bus, phy, reg);
					writeRegister(bus, phy, reg, data);
				}
			}
			if(clause == 45){
				dev = strtoul(argv[4], NULL, 16);
				reg = strtoul(argv[5], NULL, 16);
				c45_addr = MII_ADDR_C45 | ((dev & 0x1f) << 16) | (reg & 0xffff);
				if(argc == 6){
					data = readRegister(bus, phy, c45_addr);
					printf("read45 bus:%x, phy:%02x, dev:%02x, reg:%04x = %04x\n",
						bus, phy, dev, reg, data);
				}else{
					data = strtoul(argv[6], NULL, 16);
					writeRegister(bus, phy, c45_addr, data);
					printf("write45 %04x to bus:%x, phy:%02x, dev:%02x, reg:%04x\n",
						data, bus, phy, dev, reg);
				}
			}
			smiClose();
		}
	}
	if(usage){
		show_usage(argv[0]);
		return -1;
	}
	return 0;
}
