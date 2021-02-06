// kernel�� GPIO ��ư�� LED���� �����ϴ� ������ ����̴�. 
// ��ġ�� sysfs�� ���ؼ� ��ġ���� ����Ʈ�Ѵ�. ���� �� LKM ȸ�� �׽�Ʈ��
// P9_23�� ����� LED�� P9_27�� ����� ��ư�� ������ Ŀ���� overlay��
// �� �ʿ䰡 ���� ���� �ʱ� mux mode ���� �״���̴�.

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>			//GPIO��ɿ� �ʿ���
#include <linux/interrupt.h>		//IRQ �ڵ忡 �ʿ���

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");


static unsigned int gpioLED = 49;       ///< hard coding the LED gpio for this example to P9_23 (GPIO49)
static unsigned int gpioButton = 115;   ///< hard coding the button gpio for this example to P9_27 (GPIO115)
static unsigned int irqNumber;          ///< �� ���� ������ IRQ number�� �����ϴ� �뵵
static unsigned int numberPresses = 0;  ///< ��ư�� �����Ÿ� ����
static bool	    ledOn = 0;          ///< LED on/off

static irq_handler_t Gpio_irq_handler(unigned int irq,void *dev_id,struct pt_regs *regs)
{
	ledOn = !ledOn;		//��ư�� ������ LED ���¸� �ٲ۴�
	gpio_set_value(gpioLED, ledOn);		// ���¸� �����Ѵ�.
	printk(KERN_INFO,"GPIO_TEST : Interrupt! (button state is %d)\n",gpio_get_value(gpioButton);
	numberPresses++;
}
// �� �Լ��� Ŀ���� interrupt handler�̸� gpio�� ���Ͱ��� ���δ�.
// 
//���ͷ�Ʈ ���� �Լ��� ������ ������ ����.
// irq : ����ϴ� ���ͷ�Ʈ ��ȣ�̸� �Լ��� ����ɶ� ó����ƾ���� ������ �� �ְ� �Ѱ��ִ� ��ȣ��
//	
// dev_id : ���ͷ�Ʈ ID�� ������ ���ǰų� ���ͷ�Ʈ ���� �Լ� �����
//		�ʿ��� ������ ����� �޸��� ���� �ּҸ� ����ִ�.
// regs ����� �� ������..
// __init ��ũ�δ� built-in driver�� ���� ���̴�. �̴� �ð� �ʱ�ȭ�� ���Ǹ� �и��� �� �ְ� �̶� �и��� �� �ִ�.
// �� ������ ���������� 0�� ��ȯ�Ѵ�.

static _int __init Gpio_init(void)
{
	int result = 0;
	printk(KER_INFO "GPIO_TEST : Initializing the GPIO_TEST LKM\n");
	//GPIO number�� ��ȿ�Ѱ�? �ùٸ� ��ȣ���� Ȯ��
	if(!gpio_is_valid(gpioLED)
	{
		printk(KERN_INFO "GPIO_TEST : invalid LED GPIO\n");
		return -ENODEV;
	}

	//LED ����
	ledOn = true;
	gpio_request(gpioLED, "sysfs");		//gpioLED�� 49�� hardcoding�Ǿ���, ��û�Ѵ�.
	gpio_direction_output(gpioLED,ledOn);	//gpio���� out���� �����Ѵ�.
	gpio_export(gpioLED,false);		//�ֳ��ϸ� gpio49�� /sys/class/gpio�� �����ϱ� ����
						//bool ǥ��� ���� ��ȭ�� ���� �����̴�.
	gpio_request(gpioButton,"sysfs");	//gpioButton�� �����Ѵ�.
	gpio_direction_intput(gpioButton);	//gpio���� in���� �����Ѵ�.
	gpio_set_debounce(gpioButton, 200);	//ä�͸��� ������ ���� 200ms �Ŀ� �ٽ� üũ�Ѵ�.
	gpio_export(gpioButton,false);		//�ֳ��ϸ� gpio115�� /sys/class/gpio�� �����ϱ� ����
						//bool ǥ��� ���� ��ȭ�� ���� �����̴�.
	printk(KERN_INFO "GPIO_TEST : The button state is currently: %d\n", gpio_get_value(gpioButton));
						//LKM ������ �����ϰ� ��ư�� �۵��ϴ��� �׽�Ʈ
	
	// GPIO number�� IRQ number�ʹ� �ٸ���. �Ʒ� �Լ��� �츮�� ���� �������ش�.
	irqNumber = gpio_to_irq(gpioButton);
	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
	
	//�Ʒ� ������ interrupt�� ��û�Ѵ�.
	result = request_irq(irqNumber,				// The interrupt number requested
			(irq_handler_t) ebbgpio_irq_handler,	// The pointer to the handler function below
			IRQF_TRIGGER_RISING,			// Interrupt on rising edge (button press, not release)
			"ebb_gpio_handler",			// Used in /proc/interrupts to identify the owner
			NULL);					// The *dev_id for shared interrupt lines, NULL is okay

	printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
	return result;
}
static void __exit ebbgpio_exit(void){
	printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));
	printk(KERN_INFO "GPIO_TEST: The button was pressed %d times\n", numberPresses);
	gpio_set_value(gpioLED, 0);              // LED�� ����, ��ġ�� Ż������ �˸�
	gpio_unexport(gpioLED);                  // LED GPIO�� ����
	free_irq(irqNumber, NULL);               // IRQ number�� ����, no *dev_id required in this case
	gpio_unexport(gpioButton);               // Unexport the Button GPIO
	gpio_free(gpioLED);                      // Free the LED GPIO
	gpio_free(gpioButton);                   // Free the Button GPIO
	printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");
}

// �Ʒ����� �ǹ������� �������ִ� ������ �ʱ�ȭ �Լ��� Ȯ���ϸ�
// ����� �ʱ�ȭ�Ѵ�.
module_init(ebbgpio_init);
module_exit(ebbgpio_exit);