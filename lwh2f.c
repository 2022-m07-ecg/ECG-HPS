#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <asm/io.h>
#include "hps.h"
#include "hps_0.h"

#define LWH2F_BASE ALT_LWFPGASLVS_ADDR
#define LWH2F_SPAN ALT_LWFPGASLVS_UB_ADDR - ALT_LWFPGASLVS_LB_ADDR + 1

#define PIO_DATA_OFST 0x0
#define PIO_INT_MASK_OFST 0x8
#define PIO_EDGE_CAPT_OFST 0xC
#define PIO_IRQ 72 + DATA_BUFFER_IRQ

static void *lwh2f_virtual_base = NULL;			//Virtual address of LWH2F bridge
static void *pio_base = NULL;					//Virtual address of PIO port from FPGA
static volatile u16 *pio_data = NULL;		//PIO data register
static volatile u16 *pio_int_mask = NULL;	//PIO interrupt mask register
static volatile u16 *pio_edge_capt = NULL;	//PIO edge caputre register

static int __init lwh2f_init(void);
static void lwh2f_exit(void);

module_init(lwh2f_init);
module_exit(lwh2f_exit);

irq_handler_t irq_handler(int irq, void *dev_id, struct pt_regs *regs) {
	u16 input_data = *pio_data;

	*pio_edge_capt = 0xF;	//Clear interrupt condition

	return (irq_handler_t) IRQ_HANDLED;
}

static int __init lwh2f_init(void) {
	int int_req;

	lwh2f_virtual_base = (void *)ioremap_nocache((resource_size_t)LWH2F_BASE, LWH2F_SPAN);

	//Get virtual addresses of PIO port
	pio_base = lwh2f_virtual_base + DATA_BUFFER_BASE;
	pio_data = (volatile uint16_t *)(pio_base + PIO_DATA_OFST);
	pio_int_mask = (volatile uint16_t *)(pio_base + PIO_INT_MASK_OFST);
	pio_edge_capt = (volatile uint16_t *)(pio_base + PIO_EDGE_CAPT_OFST);

	//Setup PIO port
	*pio_edge_capt = 0xF;	//Clear edge capture register
	*pio_int_mask = 0xF;		//Enable interrupt on all input pins

	int_req = request_irq(PIO_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
								"PIO_irq_handler", (void *)irq_handler);
	return int_req;
}

static void __exit lwh2f_exit(void) {
	//Close LWH2F bridge
	iounmap(lwh2f_virtual_base);

	//Disable interrupt flag from PIO port
	*pio_int_mask = 0;
	lwh2f_virtual_base = NULL;
	pio_base = NULL;
	pio_data = NULL;
	pio_int_mask = NULL;
	pio_edge_capt = NULL;
	
	//Free interrupt
	free_irq(PIO_IRQ, (void*) irq_handler);
}