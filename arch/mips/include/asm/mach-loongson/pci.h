/*
 * Copyright (c) 2008 Zhang Le <r0bertz@gentoo.org>
 * Copyright (c) 2009 Wu Zhangjin <wuzj@lemote.com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 * 02139, USA.
 */

#ifndef _LOONGSON_PCI_H_
#define _LOONGSON_PCI_H_

extern struct pci_ops loongson_pci_ops;

/* this is an offset from mips_io_port_base */
#define LOONGSON_PCI_IO_START	0x00004000UL

#if defined(CONFIG_CPU_LOONGSON2F) && defined(CONFIG_64BIT)

/*
 * we use address window2 to map cpu address space to pci space
 * window2: cpu [1G, 2G] -> pci [1G, 2G]
 * why not use window 0 & 1? because they are used by cpu when booting.
 * window0: cpu [0, 256M] -> ddr [0, 256M]
 * window1: cpu [256M, 512M] -> pci [256M, 512M]
 */

/* the smallest LOONGSON_CPU_MEM_SRC can be 512M */
#define LOONGSON_CPU_MEM_SRC	0x40000000ul		/* 1G */
#define LOONGSON_PCI_MEM_DST	LOONGSON_CPU_MEM_SRC

#define LOONGSON_PCI_MEM_START	LOONGSON_PCI_MEM_DST
#define LOONGSON_PCI_MEM_END	(0x80000000ul-1)	/* 2G */

#define MMAP_CPUTOPCI_SIZE	(LOONGSON_PCI_MEM_END - \
					LOONGSON_PCI_MEM_START + 1)

#else	/* loongson2f/32bit & loongson2e */

#define LOONGSON_PCI_MEM_START	0x14000000UL
#define LOONGSON_PCI_MEM_END	0x1fffffffUL

#endif	/* !(defined(CONFIG_CPU_LOONGSON2F) && defined(CONFIG_64BIT))*/


#endif /* !_LOONGSON_PCI_H_ */