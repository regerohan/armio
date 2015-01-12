/**
 * \file main.c
 *
 * \brief main application entry point for armio
 *
 */

#include <asf.h>
#include "leds.h"
#include "display.h"
#include "aclock.h"
#include "accel.h"

//___ M A C R O S   ( P R I V A T E ) ________________________________________
#define BUTTON_PIN          PIN_PA31
#define BUTTON_PIN_EIC      PIN_PA31A_EIC_EXTINT11
#define BUTTON_PIN_EIC_MUX  MUX_PA31A_EIC_EXTINT11
#define BUTTON_EIC_CHAN     11

#define LIGHT_BATT_ENABLE_PIN       PIN_PA30
#define BATT_ADC_PIN                PIN_PA02
#define LIGHT_ADC_PIN               PIN_PA03

#define ACCEL_INIT_ERROR    1 << 0

//___ T Y P E D E F S   ( P R I V A T E ) ____________________________________


//___ P R O T O T Y P E S   ( P R I V A T E ) ________________________________
void configure_input( void );
  /* @brief configure input buttons
   * @param None
   * @retrn None
   */

void setup_clock_pin_outputs( void );
  /* @brief multiplex clocks onto output pins
   * @param None
   * @retrn None
   */

void button_extint_cb( void );
  /* @brief interrupt callback for button value changes
   * @param None
   * @retrn None
   */

static void configure_extint(void);
  /* @brief enable external interrupts
   * @param None
   * @retrn None
   */

//___ V A R I A B L E S ______________________________________________________
static bool btn_extint = false;
static uint8_t error_status = 0; /* mask of error codes */

//___ F U N C T I O N S   ( P R I V A T E ) __________________________________

void button_extint_cb( void ) {
    /* Only trigger on wakeup since button interrupts are disabled normally */
}



void configure_input(void) {

    /* Configure our button as an input */
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);
    pin_conf.direction = PORT_PIN_DIR_INPUT;
    pin_conf.input_pull = PORT_PIN_PULL_NONE;
    port_pin_set_config(BUTTON_PIN, &pin_conf);

    /* Enable interrupts for the button */
    configure_extint();

  }

static void configure_extint(void)
{
	struct extint_chan_conf eint_chan_conf;
	extint_chan_get_config_defaults(&eint_chan_conf);

	eint_chan_conf.gpio_pin             = BUTTON_PIN_EIC;
	eint_chan_conf.gpio_pin_mux         = BUTTON_PIN_EIC_MUX;
        eint_chan_conf.gpio_pin_pull        = EXTINT_PULL_NONE;
        /* NOTE: cannot wake from standby with filter or edge detection ... */
	eint_chan_conf.detection_criteria   = EXTINT_DETECT_LOW;
	eint_chan_conf.filter_input_signal  = false;
	eint_chan_conf.wake_if_sleeping     = true;
	extint_chan_set_config(BUTTON_EIC_CHAN, &eint_chan_conf);

        extint_register_callback(button_extint_cb,
                    BUTTON_EIC_CHAN,
                    EXTINT_CALLBACK_TYPE_DETECT);

}

void setup_clock_pin_outputs( void ) {
    /* For debugging purposes, multiplex our
     * clocks onto output pins.. GCLK gens 4 and 7
     * should be enabled for this to function
     * properly
     * */

    struct system_pinmux_config pin_mux;
    system_pinmux_get_config_defaults(&pin_mux);

    /* MUX out the system clock to a I/O pin of the device */
    pin_mux.mux_position = MUX_PA10H_GCLK_IO4;
    system_pinmux_pin_set_config(PIN_PA10H_GCLK_IO4, &pin_mux);

    pin_mux.mux_position = MUX_PA23H_GCLK_IO7;
    system_pinmux_pin_set_config(PIN_PA23H_GCLK_IO7, &pin_mux);

}


void enter_sleep( void ) {

    /* Enable button callback to awake us from sleep */
    extint_chan_enable_callback(BUTTON_EIC_CHAN,
                    EXTINT_CALLBACK_TYPE_DETECT);

    led_controller_disable();
    aclock_disable();
    accel_disable();

    //system_ahb_clock_clear_mask( PM_AHBMASK_HPB2 | PM_AHBMASK_DSU);

    system_sleep();

}

void wakeup (void) {

    //system_ahb_clock_set_mask( PM_AHBMASK_HPB2 | PM_AHBMASK_DSU);

    extint_chan_disable_callback(BUTTON_EIC_CHAN,
                    EXTINT_CALLBACK_TYPE_DETECT);

    system_interrupt_enable_global();
    led_controller_enable();
    aclock_enable();
    accel_enable();
}


static void end_in_error ( void ) {

    /* Display error codes on hour hand leds */
    int i;

    while (1) {
        for (i = 0; i < 11; i++) {
            if (error_status & (1 << i)) {
                led_on(i);
                //delay_ms(50);
                //led_off(i);
                //delay_ms(20);
                //led_set_blink(i, 5);
            }
        }
    }
}

//___ F U N C T I O N S ______________________________________________________

int main (void)
{
    uint16_t sleep_timeout = 0;
    uint8_t hour, minute, second;
    uint8_t hour_prev, minute_prev, second_prev;
    uint32_t button_down_cnt = 0;
    bool btn_down = false;
    uint8_t mode;
    bool fast_tick = false;
    uint32_t cnt = 0;
    int led = 0;


    system_init();
    system_apb_clock_clear_mask (SYSTEM_CLOCK_APB_APBB, PM_APBAMASK_WDT);
    system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
    delay_init();
    led_controller_init();
    led_controller_enable();

    aclock_init();

    if (!accel_init()) {

        error_status |= ACCEL_INIT_ERROR;
    }


    if (error_status != 0) {
        end_in_error();
    }

    system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);

    /* Show a startup LED swirl */
    display_swirl(10, 200, 2, 64);

    /* get intial time */
    aclock_get_time(&hour, &minute, &second);
    configure_input();
    system_interrupt_enable_global();

    while (1) {;

        //if (port_pin_get_input_level(PIN_PA30) &&

            if (port_pin_get_input_level(BUTTON_PIN)) {
                /* button is up */
                sleep_timeout++;
                button_down_cnt = 0;
                fast_tick = false;
                led_off(31);
                if ( sleep_timeout > 10000 ) {
                    /* Just released */
                    display_swirl(10, 100, 2, 64 );
                    enter_sleep();
                    wakeup();
                    sleep_timeout = 0;
                    }
                }
            else {
                led_on(31);
                sleep_timeout = 0;
                button_down_cnt++;
            }


        int16_t x,y,z;
        float mag;
        uint32_t led;
        uint8_t intensity;
        float x_f, y_f, z_f;
        int16_t val;

        if (port_pin_get_input_level(BUTTON_PIN)) {
            if (!btn_down) {
                mode+=1;
                mode = mode % 3;
                btn_down = true;
                display_swirl(10, 200, 2,  64);
                led_on(mode*5);
                delay_ms(500);
                led_off(mode*5);
            }
        }
        else {
                btn_down = false;
        }
        x = y = z = 0;
        if (!accel_data_read(&x, &y, &z)) {
            display_swirl(10, 200, 4, 64 );
            continue;
        }

        x_f = (float)x;
        y_f = (float)y;
        z_f = (float)z;

        switch(mode) {
            case 0:
                val = x;
                break;
            case 1:
                val = y;
                break;
            case 2:
                val = z;
        }

        //mag = sqrtf(x_f*x_f + y_f*y_f);
        if (val < 0) {
            led = 12 + val/42;

        } else {
            led = val/42;
        }

        if (led > 11)
            led = 11;

        led*=5;
        led_on((uint8_t) led);
        led_set_intensity(led, 5);
        delay_ms(100);
        led_clear_all();

#ifdef NOT_NOW
        if (x < 0) {
            intensity = -x >> 12;
            led_on(45);
            led_set_intensity(45, intensity);

        } else {
            intensity = x >> 12;
            led_on(15);
            led_set_intensity(15, intensity);
        }

        if (y < 0) {
            intensity = -y >> 12;
            led_on(30);
            led_set_intensity(30, intensity);

        } else {
            intensity = y >> 12;
            led_on(0);
            led_set_intensity(0, intensity);
        }

         led_clear_all();
#endif
    }

}