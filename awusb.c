
/*
 * ActivWire USB Development Board API
 *
 * Copyright (c) 2004 Bryan Kate <ragemaniac@users.sourceforge.net>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * $Id: awusb.c,v 1.1.1.1 2004/11/23 00:39:08 ragemaniac Exp $
 *
 */

#include "awusb.h"


/*** new function ************************************************************
 *
 * Print the contents of the USB bus to stdout (if the verbose flag is set) 
 * and update the device table for currently plugged in devices.
 *
 * @param dev_table An array of awusb_device pointers (to be allocated by
 *                  this method. All existing pointers are freed).
 * @param maxdevs The number of devices that can be stored in the array.
 * @param verbose A flag to indicate if the devices should be printed. 
 *                It should be AWUSB_TRUE or AWUSB_FALSE. Default AWUSB_TRUE.
 *
 * @return The number of devices found.
 *
 */
int awusb_scanbus(awusb_device *dev_table[], int maxdevs, int verbose) {

  int i                  = 0;
  int numdevs            = 0;
  struct usb_bus *bus    = NULL;
  struct usb_device *dev = NULL;
  awusb_device *newdev   = NULL;

  usb_init();

  /* look for devices */
  usb_find_busses();
  usb_find_devices();

  /* look through the device table and free any existing devices */
  if (dev_table != NULL) {
    
    for (i = 0; i < maxdevs; i++) {
      
      if (dev_table[i] != NULL) {
	
	if (dev_table[i]->claimed) {
	  awusb_release(dev_table[i]);
	}
	
	/* free(dev_table[i]); */
	mxFree(dev_table[i]);
      }
      
      dev_table[i] = NULL;
    }
  }

  if (verbose) {
    printf("\n\n*** printing USB bus ***\n\n");
  }

  /* loop through busses and attached devices */
  for (bus = usb_busses; bus; bus = bus->next) {

    for (dev = bus->devices; dev; dev = dev->next) {

      if (verbose) {
	printf("usb device %s found\n", dev->filename);
      }

      /* see if it is an activewire usb device */
      if (dev->descriptor.idVendor == AWUSB_VEND_ID) {

	if (dev->descriptor.idProduct == AWUSB_PROD_NOFIRM_ID) {
	  
	  if (verbose) {
	    printf("%s is an awusb product (pre-firmware)\n", dev->filename);
	  }
	}
	else if (dev->descriptor.idProduct == AWUSB_PROD_ID) {

	  /* make a new device and put it in the table */
	  /* newdev = (awusb_device *)mxMalloc(sizeof(awusb_device)); */
	  newdev = (awusb_device *)mxMalloc(sizeof(awusb_device));
      mexMakeMemoryPersistent(newdev);
	  
	  if (newdev != NULL) {

	    newdev->udev    = dev;
	    newdev->claimed = AWUSB_FALSE;

	    if (dev_table != NULL) {

	      /* add the device to the table */
	      if (numdevs < maxdevs) {
		
		/* try to claim the device */
		/* if (awusb_claim(newdev) == AWUSB_ERR) {
		  ERR("could not claim the device %s", dev->filename);
		} */
		
		dev_table[numdevs++] = newdev;
	      }
	      else {
		ERR("couldn't add device %s, table is full\n", dev->filename);
	      }
	    }
	  }
	  else {
	    ERR("could not allocate memory for new device");
	  }
	  
	  if (verbose) {
	    printf("%s is an awusb product (firmware loaded)\n", dev->filename);
	  }
	}
      }
    }
  }

  if (verbose) {
    printf("\n************************\n");
  }

  return numdevs;
}


/*** new function ************************************************************
 *
 * Set the I/O pins to be input or output. 
 *
 * NOTES: Talk to endpoint 2, which controls the port direction. Send 2 bytes
 *        to specify the direction of the 16 pins, a '1' bit means output, '0' 
 *        means input, and each pin can be set individually (like 0xffff for 
 *        all output).
 *
 * @param dev The device structure to be operated on.
 * @param direction The pin direction(s) to set.
 *
 * @return AWUSB_OK or AWUSB_ERROR
 *
 */
int awusb_toggleport(awusb_device *dev, unsigned int direction) {

  int ret;
  int endpoint = 2;
  int timeout  = 500;
  unsigned char bytes[2];

  /* make sure device is valid */
  if (!dev) {
    ERR("device is null, cannot toggle port");
    return AWUSB_ERR;
  }

  if (!dev->handle) {
    ERR("usb device handle is null, cannot toggle port");
    return AWUSB_ERR;
  }

  /* claim device if not already done */
  if (!dev->claimed) {

    if (awusb_claim(dev) == AWUSB_ERR) {
      ERR("could not claim device for port toggle");
      return AWUSB_ERR;
    }
  }

  /* convert the bits into the proper format */
  bytes[0] = (unsigned char)(direction & 0x00ff);
  bytes[1] = (unsigned char)((direction >> 8) & 0x00ff);

  /* toggle the pins */
  ret = usb_bulk_write(dev->handle, endpoint, bytes, 2, timeout);

  if (ret < 2) {
    ERR("write error setting I/O pin direction");
    return AWUSB_ERR;
  }
  
  return AWUSB_OK;
}


/*** new function ************************************************************
 *
 * Read 2 bytes of data from a device, which corresponds to 16 IO bits.
 *
 * NOTES: Talk to endpoint 1 to read and write.
 *
 * @param dev The device structure to be operated on.
 * @param data A variable to hold the data read.
 *
 * @return The number of bytes read, or AWUSB_ERR for an error.
 *
 */
int awusb_read(awusb_device *dev, unsigned int *data) {

  int ret;
  int endpoint = 1;
  int timeout  = 500;
  unsigned char buff[2];

  /* make sure device is valid */
  if (!dev) {
    ERR("device is null, cannot read from it");
    return AWUSB_ERR;
  }

  if (!dev->handle) {
    ERR("usb device handle is null, cannot read from it");
    return AWUSB_ERR;
  }

  /* make sure there is a place to put the data */
  if (!data) {
    ERR("the storage location is not valid, cannot read from device");
    return AWUSB_ERR;
  }

  /* claim device if not already done */
  if (!dev->claimed) {

    if (awusb_claim(dev) == AWUSB_ERR) {
      ERR("could not claim device for port toggle");
      return AWUSB_ERR;
    }
  }

  /* read 2 bytes */
  ret = usb_bulk_read(dev->handle, endpoint, buff, 2, timeout);

  if (ret < 2) {
    ERR("read only returned %d bytes, requested 2", ret);
    return AWUSB_ERR;
  }

  /* convert the data read */
  *data = ((0x00ff & buff[1]) << 8) | (0x00ff & buff[0]);

  return ret;
}


/*** new function ************************************************************
 *
 * Write 2 bytes of data to a device, which corresponds to 16 IO bits.
 *
 * NOTES: Talk to endpoint 1 to read and write.
 *
 * @param dev The device structure to be operated on.
 * @param data A variable to hold the data to write.
 *
 * @return The number of bytes written, or AWUSB_ERR for an error.
 *
 */
int awusb_write(awusb_device *dev, unsigned int data) {

  int ret;
  int endpoint = 1;
  int timeout  = 500;
  unsigned char bytes[2];

  /* make sure device is valid */
  if (!dev) {
    ERR("device is null, cannot read from it");
    return AWUSB_ERR;
  }

  if (!dev->handle) {
    ERR("usb device handle is null, cannot read from it");
    return AWUSB_ERR;
  }

  /* claim device if not already done */
  if (!dev->claimed) {

    if (awusb_claim(dev) == AWUSB_ERR) {
      ERR("could not claim device for port toggle");
      return AWUSB_ERR;
    }
  }

  /* convert data to format needed */
  bytes[0] = (unsigned char)(data & 0x00ff);
  bytes[1] = (unsigned char)((data >> 8) & 0x00ff);

  /* write 2 bytes */
  ret = usb_bulk_write(dev->handle, endpoint, bytes, 2, timeout);

  if (ret < 2) {
    ERR("write only returned %d bytes, tried 2", ret);
    return AWUSB_ERR;
  }
    
  return ret;
}


/*** new function ************************************************************
 *
 * Open, setup, and claim a device for read/write.
 *
 * @param dev The structure to work on.
 *
 * @return AWUSB_OK if device was claimed, AWUSB_ERR otherwise.
 *
 */
int awusb_claim(awusb_device *dev) {

  int config = 1;
  int intf   = 0;
  int alt    = 0;

  /* open the device */
  dev->handle = usb_open(dev->udev);

  if (!dev->handle) {
    ERR("could not open device");
    return AWUSB_ERR;
  }

  /* set the device configuration */
  if (usb_set_configuration(dev->handle, config) != 0) {
    ERR("could not set configuration");
    usb_close(dev->handle);
    return AWUSB_ERR;
  }

  /* set the interface to use */
  if (usb_claim_interface(dev->handle, intf) != 0) {
    ERR("could not claim interface");
    usb_close(dev->handle);
    return AWUSB_ERR;
  }

  /* set the alternate interface */
  if (usb_set_altinterface(dev->handle, alt) != 0) {
    ERR("could not set alternate interface");
    usb_release_interface(dev->handle, intf);
    usb_close(dev->handle);
    return AWUSB_ERR;
  }

  dev->claimed = AWUSB_TRUE;

  return AWUSB_OK;
}


/*** new function ************************************************************
 *
 * Release a device after read/write.
 *
 * @param The device structure to operate on. 
 *
 * @return AWUSB_OK for success, AWUSB_ERR otherwise.
 *
 */
int awusb_release(awusb_device *dev) {

  int intf = 0;

  /* release the interface */
  if (usb_release_interface(dev->handle, intf) != 0) {
    ERR("trouble releasing interface");
    return AWUSB_ERR;
  }

  /* close the device handle */
  if (usb_close(dev->handle) != 0) {
    ERR("trouble closing device");
    return AWUSB_ERR;
  }

  dev->claimed = AWUSB_FALSE;

  return AWUSB_OK;
}

/****************************************************************************/

/*
 * $Log: awusb.c,v $
 * Revision 1.1.1.1  2004/11/23 00:39:08  ragemaniac
 * initial import
 *
 *
 */
