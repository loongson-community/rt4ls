/*
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/fs.h>
#include <linux/mm.h>

#include <asm/bootinfo.h>

#include <loongson.h>
#include <mem.h>

void __init prom_init_memory(void)
{
	add_memory_region(0x0, (memsize << 20), BOOT_MEM_RAM);
#ifdef CONFIG_64BIT
#ifdef CONFIG_CPU_LOONGSON2F
	{
		int bit;

		bit = fls(memsize + highmemsize);
		if (bit != ffs(memsize + highmemsize))
			bit += 20;
		else
			bit = bit + 20 - 1;

		/* set cpu window3 to map CPU to DDR: 2G -> 2G */
		LOONGSON_ADDRWIN_CPUTODDR(ADDRWIN_WIN3, 0x80000000ul,
					  0x80000000ul, (1 << bit));
		mmiowb();
	}
#endif				/* CONFIG_CPU_LOONGSON2F */

	if (highmemsize > 0) {
		add_memory_region(LOONGSON_HIGHMEM_START,
				  highmemsize << 20, BOOT_MEM_RAM);
	}
#endif				/* CONFIG_64BIT */
}

/* override of arch/mips/mm/cache.c: __uncached_access */
int __uncached_access(struct file *file, unsigned long addr)
{
	if (file->f_flags & O_SYNC)
		return 1;

	return addr >= __pa(high_memory) ||
		((addr >= LOONGSON_MMIO_MEM_START) && \
			(addr < LOONGSON_MMIO_MEM_END));
}

#if defined(CONFIG_CPU_LOONGSON2F)

#include <linux/pci.h>
#include <linux/sched.h>
#include <asm/current.h>

static unsigned long uca_start;
static unsigned long uca_size = CONFIG_UCA_SIZE;

pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
				     unsigned long size, pgprot_t vma_prot)
{
	unsigned long offset = pfn << PAGE_SHIFT;
	unsigned long end = offset + size;

	if (__uncached_access(file, offset)) {
		if (((uca_start && offset) >= uca_start) && \
				(end <= (uca_start + uca_size)))
			return __pgprot((pgprot_val(vma_prot) & \
						~_CACHE_MASK) | \
					_CACHE_UNCACHED_ACCELERATED);
		else
			return pgprot_noncached(vma_prot);
	}
	return vma_prot;
}

static int __init find_vga_mem_init(void)
{
	struct pci_dev *dev = 0;
	struct resource *r;
	int idx;

	if (uca_start)
		return 0;

	for_each_pci_dev(dev) {
		if ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA) {
			for (idx = 0; idx < PCI_NUM_RESOURCES; idx++) {
				r = &dev->resource[idx];
				if (!r->start && r->end)
					continue;
				if (r->flags & IORESOURCE_IO)
					continue;
				if (r->flags & IORESOURCE_MEM) {
					uca_start = r->start;

					printk(KERN_INFO
					"find the frame buffer:start=%lx\n",
					uca_start);

					return 0;
				}
			}

		}
	}
	return 0;
}

late_initcall(find_vga_mem_init);
#endif /* !CONFIG_CPU_LOONGSON2F */