/*
	The basic principle of work:

	(1) Using IO trigger for at least 10us high-level signal,
	(2) The Module automatically sends eight 40 kHz and detect whether there is a pulse signal back.
	(3) IF the signal back, through high level, time of high output IO duration is the time from sending ultrasonic to returning.
	(4) Test distance = (high level time×velocity of sound (340M/S) / 2

*/


/* include headers */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys_clock.h>
#include <zephyr/sys/util.h>
#include <limits.h>

/* define port, echo pin and trig pin */
#define TRIG_PIN		1
#define ECHO_PIN		3
#define GPIO0_PORT		"GPIO0"

#define GPIO_PIN_ON		1
#define GPIO_PIN_OFF	0

#define MAX_ECHO_TIMEOUT_NS			38000000
#define SPEED_OF_SPEED_MM_PER_NS	0.00034


const struct device *dev_gpio0;

double Read_Sensor(void);

int main(void)
{
	
	/* small delay at start */
	k_sleep(K_SECONDS(5));

	/* get device binding to port */
	dev_gpio0 = device_get_binding(GPIO0_PORT);
	if(dev_gpio0 == NULL)
	{
		printk("create device binding for port gpio0 failed");
		return;
	}
	else
	{
		printk("create device binding for port gpio0 sucessful");
	}

	/* configure trig pin as output and echo pin as input pin */
	gpio_pin_configure(dev_gpio0,TRIG_PIN,GPIO_OUTPUT);
	gpio_pin_configure(dev_gpio0,ECHO_PIN,(GPIO_INPUT | GPIO_INT_EDGE| GPIO_ACTIVE_HIGH ));

	while(true)
	{
		printk("distance: %.2fmm",Read_Sensor());
		k_sleep(K_SECONDS(1));
	}

	return 0;
}

double Read_Sensor(void)
{
	int val;
	uint32_t starttime;
	uint32_t endtime;
	uint32_t cycles;
	double   distance;
	
	/* activate trigger pin */
	gpio_pin_set(dev_gpio0,TRIG_PIN,GPIO_PIN_ON);
	k_sleep(K_MSEC(10));
	gpio_pin_set(dev_gpio0,TRIG_PIN,GPIO_PIN_OFF);

	/* read echo pin with time */
	do{
		val = gpio_pin_get(dev_gpio0,TRIG_PIN);
	}while(val == 0);

	starttime = k_cycle_get_32();

	do{
		val = gpio_pin_get(dev_gpio0,TRIG_PIN);
		endtime = k_cycle_get_32();
		cycles = endtime - starttime;
		if(SYS_CLOCK_HW_CYCLES_TO_NS_AVG(cycles,1) > MAX_ECHO_TIMEOUT_NS)
		{
			break;
		}
	}while(val == 1);
	
	distance = (SPEED_OF_SPEED_MM_PER_NS * SYS_CLOCK_HW_CYCLES_TO_NS_AVG(cycles,1)) / 2;

	return distance;
}