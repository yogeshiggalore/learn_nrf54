/* include kernel components */
#include <zephyr/kernel.h>

/* include device drivers */
#include <zephyr/drivers/gpio.h>

#define LED_SLEEP_TIME_MS   100

/* the device tree indentifier for the led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* get devicetree spec for led0 node */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 5

K_THREAD_STACK_DEFINE(thread_stack, THREAD_STACK_SIZE);
struct k_thread led0_thread_data;

void led0_thread_fun(void)
{
	int ret = 0;

	/* check if the led port is ready */
	if (!device_is_ready(led.port))
	{
		ret = -1;
	}
	else
	{
		/* configure the led pin */
		ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
		if (ret < 0)
		{
			ret = -2;
		}
	}
	
	while (true)
	{
		/* sleep for some time */
		k_msleep(LED_SLEEP_TIME_MS);

		if(ret == 0)
		{
			/* toggle the led pin */
			gpio_pin_toggle(led.port, led.pin);
		}
	}
}

int main(void)
{
	return 0;
}

K_THREAD_DEFINE(led0_thread_id, THREAD_STACK_SIZE, led0_thread_fun, NULL, NULL, NULL, THREAD_PRIORITY, 0, 0);