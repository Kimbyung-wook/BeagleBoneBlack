// kernel로 GPIO 버튼과 LED쌍을 제어하는 간단한 모듈이다. 
// 장치에 sysfs를 통해서 장치들을 마운트한다. 따라서 이 LKM 회로 테스트는
// P9_23에 연결된 LED를 P9_27에 연결된 버튼이 있으며 커스텀 overlay를
// 할 필요가 없고 핀은 초기 mux mode 상태 그대로이다.

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>			//GPIO기능에 필요함
#include <linux/interrupt.h>		//IRQ 코드에 필요함

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");


static unsigned int gpioLED = 49;       ///< hard coding the LED gpio for this example to P9_23 (GPIO49)
static unsigned int gpioButton = 115;   ///< hard coding the button gpio for this example to P9_27 (GPIO115)
static unsigned int irqNumber;          ///< 이 파일 내에서 IRQ number를 공유하는 용도
static unsigned int numberPresses = 0;  ///< 버튼이 눌린거를 저장
static bool	    ledOn = 0;          ///< LED on/off

static irq_handler_t Gpio_irq_handler(unigned int irq,void *dev_id,struct pt_regs *regs)
{
	ledOn = !ledOn;		//버튼이 눌리면 LED 상태를 바꾼다
	gpio_set_value(gpioLED, ledOn);		// 상태를 적용한다.
	printk(KERN_INFO,"GPIO_TEST : Interrupt! (button state is %d)\n",gpio_get_value(gpioButton);
	numberPresses++;
}
// 위 함수는 커스텀 interrupt handler이며 gpio를 위와같이 붙인다.
// 
//인터럽트 서비스 함수의 형식은 다음과 같다.
// irq : 사용하는 인터럽트 번호이며 함수가 수행될때 처리루틴에서 참고할 수 있게 넘겨주는 번호임
//	
// dev_id : 인터럽트 ID로 공유에 사용되거나 인터럽트 서비스 함수 수행시
//		필요한 정보가 저장된 메모리의 선두 주소를 담고있다.
// regs 사용할 일 없을듯..
// __init 매크로는 built-in driver를 위한 것이다. 이는 시간 초기화에 사용되며 분리할 수 있고 이때 분리될 수 있다.
// 이 예제는 성공했을때 0을 반환한다.

static _int __init Gpio_init(void)
{
	int result = 0;
	printk(KER_INFO "GPIO_TEST : Initializing the GPIO_TEST LKM\n");
	//GPIO number가 유효한가? 올바른 번호인지 확인
	if(!gpio_is_valid(gpioLED)
	{
		printk(KERN_INFO "GPIO_TEST : invalid LED GPIO\n");
		return -ENODEV;
	}

	//LED 세팅
	ledOn = true;
	gpio_request(gpioLED, "sysfs");		//gpioLED는 49로 hardcoding되었고, 요청한다.
	gpio_direction_output(gpioLED,ledOn);	//gpio핀을 out으로 설정한다.
	gpio_export(gpioLED,false);		//왜냐하면 gpio49가 /sys/class/gpio에 존재하기 때문
						//bool 표기는 방향 변화를 막기 위함이다.
	gpio_request(gpioButton,"sysfs");	//gpioButton을 설정한다.
	gpio_direction_intput(gpioButton);	//gpio핀을 in으로 설정한다.
	gpio_set_debounce(gpioButton, 200);	//채터링을 없에기 위해 200ms 후에 다시 체크한다.
	gpio_export(gpioButton,false);		//왜냐하면 gpio115가 /sys/class/gpio에 존재하기 때문
						//bool 표기는 방향 변화를 막기 위함이다.
	printk(KERN_INFO "GPIO_TEST : The button state is currently: %d\n", gpio_get_value(gpioButton));
						//LKM 장착을 예상하고 버튼이 작동하는지 테스트
	
	// GPIO number는 IRQ number와는 다르다. 아래 함수는 우리를 위해 맵핑해준다.
	irqNumber = gpio_to_irq(gpioButton);
	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
	
	//아래 문장은 interrupt를 요청한다.
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
	gpio_set_value(gpioLED, 0);              // LED를 끄고, 장치가 탈착됨을 알림
	gpio_unexport(gpioLED);                  // LED GPIO를 제거
	free_irq(irqNumber, NULL);               // IRQ number를 삭제, no *dev_id required in this case
	gpio_unexport(gpioButton);               // Unexport the Button GPIO
	gpio_free(gpioLED);                      // Free the LED GPIO
	gpio_free(gpioButton);                   // Free the Button GPIO
	printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");
}

// 아래에는 의무적으로 정해져있는 것으로 초기화 함수를 확인하며
// 기능을 초기화한다.
module_init(ebbgpio_init);
module_exit(ebbgpio_exit);