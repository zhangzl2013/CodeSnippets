/**
 * "Hello world" kernel module 
 *
 * Copyright 2014 Zhang Zhaolong <zhangzhaolong0454@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define DEBUG

#include <linux/init.h>
#include <linux/module.h>

#undef pr_fmt
#define pr_fmt(fmt)	KBUILD_MODNAME " %04d@%s: " fmt, __LINE__, __func__

static int __init hello_kernel_init(void)
{
	pr_debug("Hello, Kernel.");
	return 0;
}

static void __exit hello_kernel_exit(void)
{
	pr_debug("Goodbye, Kernel.");
}

module_init(hello_kernel_init);
module_exit(hello_kernel_exit);

MODULE_LICENSE("Apache v2");
MODULE_AUTHOR("Zhang Zhaolong");
