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

void service_empty(void)
{
}
