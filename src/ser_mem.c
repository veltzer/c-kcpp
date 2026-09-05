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

/* do you want memory debuggin ? (collecting number of allocations
 * and deallocations...).
 */
/* #define DO_MEMDBG */

/*
 * memory handling code
 */

/*
 * Get the page size for the operating system currently running...
 */
unsigned int service_get_page_size(void)
{
	return PAGE_SIZE;
}
/*
 * Translate a virtual address to a physical one
 */
unsigned long service_virt_to_phys(void *virt)
{
	/* TODO: should we add error checking here ?!? */
	return virt_to_phys(virt);
}
/*
 * put 0 in an area of memory...
 */
void service_zeromem(void *addr, unsigned int size)
{
	/* TODO: is there a faster way to do it? */
	memset(addr, 0, size);
}
/*
 * put a certain number in an area of memory
 */
void service_memset(void *addr, int c, unsigned int size)
{
	memset(addr, c, size);
}
/*
 *Memory copy API
 */
void service_memcpy(const void *to, const void *from, unsigned int size)
{
	memcpy((void *)to, from, size);
}
#ifdef DO_MEMDBG
/* These are initialised to 0 by default... */
static unsigned int malloc_num;
static unsigned int free_num;
#endif /* DO_MEMDBG */
/*
 * Memory allocation API
 */
void *service_malloc(unsigned int size)
{
	void *p;

	DEBUG("start");
#ifdef DO_MEMDBG
	malloc_num++;
#endif /* DO_MEMDBG */
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
#ifdef DO_MEMDBG
	free_num++;
#endif /* DO_MEMDBG */
	kfree(pointer);
}
void service_mdebug(void)
{
#ifdef DO_MEMDBG
	DEBUG("malloc_num is %u", malloc_num);
	DEBUG("free_num is %u", free_num);
#endif /* DO_MEMDBG */
}
