/*
 * This software is Copyright (c) 2016,2019 Denis Burykin
 * [denis_burykin yahoo com], [denis-burykin2014 yandex ru]
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 */

//===============================================================
//
// Contains functions for operating Ztex USB-FPGA modules.
// Based on original Ztex SDK written in java.
//
//===============================================================
#ifndef _ULX3S_H_
#define _ULX3S_H_

#include "../../list.h"
#include "ulx3s_sn.h"

#define USB_CMD_TIMEOUT 150
#define USB_RW_TIMEOUT 500

#define ULX3S_OPEN_NUM_RETRIES 3
#define ULX3S_OPEN_RETRY_WAIT 150

// includes '\0' terminator
#define ULX3S_PRODUCT_STRING_LEN 32

#define ULX3S_IDVENDOR 0x221A
#define ULX3S_IDPRODUCT 0x0100

#define ULX3S_ENDPOINT_HS 6

// Capability index for EEPROM support.
#define CAPABILITY_EEPROM 0,0
// Capability index for FPGA configuration support.
#define CAPABILITY_FPGA 0,1
// Capability index for FLASH memory support.
#define CAPABILITY_FLASH 0,2
// Capability index for DEBUG helper support.
#define CAPABILITY_DEBUG 0,3
// Capability index for AVR XMEGA support.
#define CAPABILITY_XMEGA 0,4
// Capability index for AVR XMEGA support.
#define CAPABILITY_HS_FPGA 0,5
// Capability index for AVR XMEGA support.
#define CAPABILITY_MAC_EEPROM 0,6
// Capability index for multi FPGA support.
#define CAPABILITY_MULTI_FPGA 0,7
// Unsure if this works (not tested), looks like the right version
// would be CAPABILITY_TEMP_SENSOR 1,0 (bit 0 in byte 1)
// Capability index for Temperature sensor support
#define CAPABILITY_TEMP_SENSOR 0,8
// Capability index for 2nd FLASH memory support
#define CAPABILITY_FLASH2 0,9


extern int ULX3S_DEBUG;


int vendor_command(struct libusb_device_handle *handle, int cmd, int value, int index, unsigned char *buf, int length);

int vendor_request(struct libusb_device_handle *handle, int cmd, int value, int index, unsigned char *buf, int length);


// used by ULX3S SDK VR 0x30: getFpgaState
// for debug purposes only
// inouttraffic doesn't use that
struct ulx3s_fpga_state {
	int fpgaConfigured;
	int fpgaChecksum;
	int fpgaBytes;
	int fpgaInitB;
	//int fpgaFlashResult; // present but not used in ULX3S
	//int fpgaFlashBitSwap;
};

struct ulx3s_device {
	struct libusb_device_handle *handle;
	libusb_device *usb_device;
	int busnum;
	int devnum;
	int num_of_fpgas;
	int selected_fpga;
	int valid;
	int iface_claimed;
	struct ulx3s_device *next;
	char snString[ULX3S_SNSTRING_LEN];
	// ULX3S specific stuff from device
	char snString_orig[ULX3S_SNSTRING_LEN];
	unsigned char productId[4];
	unsigned char fwVersion;
	unsigned char interfaceVersion;
	unsigned char interfaceCapabilities[6];
	unsigned char moduleReserved[12];
	char product_string[ULX3S_PRODUCT_STRING_LEN];
};

struct ulx3s_dev_list {
	struct ulx3s_device *dev;
};

int ulx3s_device_new(libusb_device *usb_dev, struct ulx3s_device **ulx3s_dev);

void ulx3s_device_delete(struct ulx3s_device *dev);

void ulx3s_device_invalidate(struct ulx3s_device *dev);

int ulx3s_device_valid(struct ulx3s_device *dev);


struct ulx3s_dev_list *ulx3s_dev_list_new();

void ulx3s_dev_list_add(struct ulx3s_dev_list *dev_list, struct ulx3s_device *dev);

// Moves valid devices from 'added_list' to 'dev_list'. Returns number of moved devices. 'added_list' emptied.
int ulx3s_dev_list_merge(struct ulx3s_dev_list *dev_list, struct ulx3s_dev_list *added_list);

// Device removed from list and deleted
void ulx3s_dev_list_remove(struct ulx3s_dev_list *dev_list, struct ulx3s_device *dev_remove);

// Deletes all devices and the list itself
void ulx3s_dev_list_delete(struct ulx3s_dev_list *dev_list);

int ulx3s_dev_list_count(struct ulx3s_dev_list *dev_list);

void ulx3s_dev_list_print(struct ulx3s_dev_list *dev_list);

struct ulx3s_device *ulx3s_find_by_sn(struct ulx3s_dev_list *dev_list, char *sn);


// equal to reset_fpga() from ULX3S SDK (VR 0x31)
// FPGA reset, removes bitstream
int ulx3s_reset_fpga(struct ulx3s_device *dev);

// equal to select_fpga() from ULX3S SDK (VR 0x51)
int ulx3s_select_fpga(struct ulx3s_device *dev, int num);

// Gets ULX3S-specific data from device (VR 0x22), including
// device type and capabilities
// Used in ulx3s_device_new()
int ulx3s_get_descriptor(struct ulx3s_device *dev);

// ULX3S Capabilities. Capabilities are pre-fetched and stored in 'struct ulx3s_device'
int ulx3s_check_capability(struct ulx3s_device *dev, int i, int j);

int ulx3s_firmware_is_ok(struct ulx3s_device *dev);

// Scans for devices that aren't already in dev_list, adds to new_dev_list
// - Performs all checks & closes file descriptor ASAP
// Returns:
// >= 0 number of devices added
// <0 error
int ulx3s_scan_new_devices(struct ulx3s_dev_list *new_dev_list,
		struct ulx3s_dev_list *dev_list, int warn_open,
		struct list_main *dev_allow);

// upload bitstream on FPGA
int ulx3s_configureFpgaHS(struct ulx3s_device *dev, FILE *fp, int interfaceHS);

// uploads bitsteam on every FPGA in the device
int ulx3s_upload_bitstream(struct ulx3s_device *dev, FILE *fp);

// reset_cpu used by firmware upload
int ulx3s_reset_cpu(struct ulx3s_device *dev, int r);

// firmware image loaded from an ihx (Intel Hex format) file.
struct ihx_data {
	short *data;
};

// Uploads firmware from .ihx file, device resets.
// < 0 on error.
int ulx3s_firmware_upload(struct ulx3s_device *dev, char *filename);

// resets device. firmware is lost.
void ulx3s_device_reset(struct ulx3s_device *dev);

#endif
