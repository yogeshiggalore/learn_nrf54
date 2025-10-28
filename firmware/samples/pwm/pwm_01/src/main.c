/* include headers */
#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>

#define PWM_PERIOD 	100
#define PWM_STEP	1

#define DIRECTION_FORWARD 0
#define DIRECTION_BACKWARD 1

const struct device *dev_pwm;

uint32_t cycle=5;
int direction=DIRECTION_FORWARD;

int main(void)
{
	int err=0;
	dev_pwm = DEVICE_DT_GET(DT_NODELABEL(pwm20));
	

	while(true)
	{
		//err = pwm_set(dev_pwm,0,PWM_USEC(PWM_PERIOD),PWM_USEC(cycle),PWM_POLARITY_INVERTED);
		err = pwm_set(dev_pwm,0,PWM_USEC(PWM_PERIOD),PWM_USEC(cycle),PWM_POLARITY_INVERTED);
		if(direction == DIRECTION_FORWARD)
		{
			if(cycle < PWM_PERIOD)
			{
				cycle += PWM_STEP; 
			}
			else
			{
				direction = DIRECTION_BACKWARD;
			}
		}
		else
		{
			if(cycle > 0)
			{
				cycle -= PWM_STEP; 
			}
			else
			{
				direction = DIRECTION_FORWARD;
			}
		}
		k_sleep(K_MSEC(1));
	}
	return 0;
}
