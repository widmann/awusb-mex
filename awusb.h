
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
 * $Id: awusb.h,v 1.1.1.1 2004/11/23 00:39:08 ragemaniac Exp $
 *
 */

#ifndef _AWUSB_
#define _AWUSB_


/*** includes ***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <usb.h>


/*** defines ****************************************************************/
 
/* #define ERR(fmt, args...) fprintf(stderr, "awusb error: "fmt" \n", ## args) */
#define ERR(fmt, args...) printf("awusb error: "fmt" \n", ## args)


#define AWUSB_VEND_ID         0x0854
#define AWUSB_PROD_ID         0x0101
#define AWUSB_PROD_NOFIRM_ID  0x0100

#define AWUSB_TRUE  1
#define AWUSB_FALSE 0

#define AWUSB_OK   1
#define AWUSB_ERR -1


/*** typedefs ***************************************************************/

typedef struct {
  struct usb_device *udev;       /* pointer to usb device */
  usb_dev_handle *handle;        /* device handle after opening */
  int claimed;                   /* status of open/closed device */
} awusb_device;


/*** prototypes *************************************************************/

int awusb_scanbus(awusb_device *dev_table[], int maxdevs, int verbose);

int awusb_toggleport(awusb_device *dev, unsigned int direction);

int awusb_read(awusb_device *dev, unsigned int *data);

int awusb_write(awusb_device *dev, unsigned int data);

int awusb_claim(awusb_device *dev);

int awusb_release(awusb_device *dev);


/****************************************************************************/

#endif

/*
 * $Log: awusb.h,v $
 * Revision 1.1.1.1  2004/11/23 00:39:08  ragemaniac
 * initial import
 *
 *
 */
