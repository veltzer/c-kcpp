// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/timex.h>
#include <linux/cpufreq.h>

#include "services.h"
#include "kernel_helper.h"

/*
 * memory handling code
 */

/*
 * Memory allocation API
 */
void *service_malloc(unsigned int size)
{
	void *p;

	DEBUG("start");
	/*
	 * if (size == 0) {
	 *	DEBUG("kmalloc size=0");
	 *	BUG();
	 * }
	 * p = kmalloc(size, GFP_KERNEL);
	 */
	p = kmalloc(size, GFP_KERNEL);
	DEBUG("kmalloc size=%d pointer=%p", size, p);
	if (p == NULL)
		ERROR("unable to allocate memory");
	return p;
}
/*
 * Memory relase API
 */
void service_free(void *pointer)
{
	DEBUG("start");
	DEBUG("kfree pointer=%p", pointer);
	kfree(pointer);
}
