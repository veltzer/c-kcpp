// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h> /* for MODULE_* macros */
#include "connect.h"

static int __init link_init(void)
{
	pr_info("kcpp: loading\n");
	return cpp_init();
}


static void __exit link_exit(void)
{
	cpp_exit();
	pr_info("kcpp: unloading\n");
}

module_init(link_init);
module_exit(link_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Module written in C++");
