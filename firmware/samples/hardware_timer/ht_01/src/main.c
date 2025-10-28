
/* include kernel, gpio device drivers and hardware timer */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/sys/printk.h>

/* timer value 500 ms*/
#define TIMER_VALUE 500

#define ALARM_CHANNEL_ID 0

/* led0 node */
#define LED0_NODE DT_ALIAS(led0)

/* led0 device tree */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* timer2 configuration */
#define TIMER2 DT_NODELABEL(timer20)
struct counter_alarm_cfg timer2_cfg;
const struct device *const dev_timer2 = DEVICE_DT_GET(TIMER2);

void timer2_cb(const struct device *dev, uint8_t chan_id, uint32_t ticks, void *user_data);

int main(void)
{
	int ret=0;
	int err=0;

	/* check for led0 device readiness */
	if(!gpio_is_ready_dt(&led0))
	{
		printk("led0 device not ready\n");
		return -1;
	}
	else
	{
		printk("led0 device ready\n");
	}

	/* configure led0 pin as output */
	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		printk("led0 configure failed\n");
		return ret;
	}
	else
	{
		printk("led0 configured\n");
	}

	/* check for timer2 device readiness */
	if(!device_is_ready(dev_timer2))
	{
		printk("timer2 device not ready\n");
		return -1;
	}
	else
	{
		printk("timer2 device ready\n");
	}

	printk("configure timer2\n");

	/* start timer with TIMER_VALUE */
	counter_start(dev_timer2);
	timer2_cfg.flags = 0;
	timer2_cfg.ticks = counter_us_to_ticks(dev_timer2, (TIMER_VALUE * 1000));
	timer2_cfg.callback = timer2_cb;
	timer2_cfg.user_data = (void*)&timer2_cfg;

	/* set alarm */
	err = counter_set_channel_alarm(dev_timer2, ALARM_CHANNEL_ID, &timer2_cfg);
	if (err < 0)
	{
		printk("timer2 set alarm failed\n");
		return err;
	}
	else
	{
		printk("timer2 set alarm\n");
	}

	printk("led0 on\n");
	gpio_pin_set_dt(&led0, false);

	while (true)
	{
		k_sleep(K_FOREVER);
	}

	return 0;
}

void timer2_cb(const struct device *dev, uint8_t chan_id, uint32_t ticks, void *user_data)
{
	int err;
    struct counter_alarm_cfg *cfg = (struct counter_alarm_cfg*)user_data;

    gpio_pin_toggle_dt(&led0);
	printk("led0 toggle\n");

	printk("reconfigure timer2 alarm\n");

    // Set ticks to the interval you want (relative to now)
    cfg->ticks = counter_us_to_ticks(dev_timer2, (TIMER_VALUE * 1000));
	cfg->callback = timer2_cb;
	cfg->flags = 0;
	cfg->user_data = (void*)cfg;

    err = counter_set_channel_alarm(dev_timer2, ALARM_CHANNEL_ID, cfg);
}