/** file:       main.h
  * created:    2015-01-09 13:37:30
  * author:     Richard Bryan
  */

#ifndef __MAIN_H__
#define __MAIN_H__

//___ I N C L U D E S ________________________________________________________

//___ M A C R O S ____________________________________________________________


/* Event Flags */
#define EV_FLAG_NONE                    0
#define EV_FLAG_SINGLE_BTN_PRESS_END    (1 << 1)
#define EV_FLAG_LONG_BTN_PRESS          (1 << 2)
#define EV_FLAG_LONG_BTN_PRESS_END      (1 << 3)
#define EV_FLAG_ACCEL_SCLICK_X          (1 << 4)
#define EV_FLAG_ACCEL_SCLICK_Y          (1 << 5)
#define EV_FLAG_ACCEL_SCLICK_Z          (1 << 6)
#define EV_FLAG_ACCEL_DCLICK_X          (1 << 7)
#define EV_FLAG_ACCEL_DCLICK_Y          (1 << 8)
#define EV_FLAG_ACCEL_DCLICK_Z          (1 << 9)
#define EV_FLAG_ACCEL_TCLICK_X          (1 << 10)
#define EV_FLAG_ACCEL_TCLICK_Y          (1 << 11)
#define EV_FLAG_ACCEL_TCLICK_Z          (1 << 12)
#define EV_FLAG_ACCEL_QCLICK_X          (1 << 13)
#define EV_FLAG_ACCEL_QCLICK_Y          (1 << 14)
#define EV_FLAG_ACCEL_QCLICK_Z          (1 << 15)
#define EV_FLAG_ACCEL_DOWN_UP           (1 << 16)
#define EV_FLAG_ACCEL_Z_LOW             (1 << 17)
#define EV_FLAG_SLEEP                   (1 << 18)

/* Error Codes */
#define ERROR_NONE                      0
#define ERROR_ACCEL_READ_ID             1
#define ERROR_ACCEL_BAD_ID              2
#define ERROR_ACCEL_WRITE_ENABLE        3
#define ERROR_DISP_DRAW_BAD_COMP_TYPE   4
#define ERROR_DISP_ALLOC_FAIL           5
#define ERROR_DISP_CLEAR_BAD_COMP_TYPE  6
#define ERROR_ANIM_BAD_TYPE             7
#define ERROR_ANIM_ALLOC_FAIL           8
#define ERROR_ASSERTION_FAIL            9

/* System (main) timer */
#define MS_PER_TICK             1
#define MS_IN_TICKS(M)         (M/MS_PER_TICK)
#define TICKS_IN_MS(T)         (T*MS_PER_TICK)
#define MAIN_TIMER_TICK_US      (MS_PER_TICK*1000)

#define assert(B) if (!(B)) main_terminate_in_error(ERROR_ASSERTION_FAIL)

//___ T Y P E D E F S ________________________________________________________
typedef uint32_t event_flags_t;

typedef enum sensor_type_t {
    sensor_vbatt,
    sensor_light,
} sensor_type_t;

//___ V A R I A B L E S ______________________________________________________

//___ P R O T O T Y P E S ____________________________________________________
void main_terminate_in_error( uint8_t error_code);
  /* @brief terminate program with error code
   * @param error flags
   * @retrn None
   */

void main_start_sensor_read ( void );
  /* @brief start an adc read of current sensor
   * @param None
   * @retrn None
   */
sensor_type_t main_get_current_sensor ( void );
  /* @brief get current sensor setting
   * @param None
   * @retrn None
   */

void main_set_current_sensor ( sensor_type_t sensor);
  /* @brief set sensor to read adc values from
   * @param None
   * @retrn None
   */

uint16_t main_read_current_sensor( bool blocking );
  /* @brief read adc for most recently started sensor read
   * the current sensor is based on the most recent sensor_read() call
   * @param blocking - dont use cached value -- blocks until new value
   * @retrn adc value for the current sensor
   */

uint16_t main_get_light_sensor_value ( void );
  /* @brief get most recent light sensor adc value
   * @param None
   * @retrn None
   */

uint16_t main_get_vbatt_value ( void );
  /* @brief get running-averaged vbatt value
   * @param None
   * @retrn None
   */

uint8_t main_get_multipress_count( void );
  /* @brief get number of taps for a multi-tap event
   * @param None
   * @retrn # of presses
   */

void main_log_data( uint8_t *data, uint16_t length, bool flush );
  /* @brief store data in unused flash space
   * @param data - pointer to data array
   * @param len - number of 32-bit words (byte count / 4) to write
   * @param flush - if true, write immediately
   * @retrn None
   */

uint32_t main_get_button_hold_ticks( void );
  /* @brief # of button ticks since long press started
   * @param None
   * @retrn # of button ticks since long press started
   */

uint32_t main_get_waketime_ms( void );
  /* @brief get time since last wake in ms
   * @param None
   * @retrn time awake in ms
   */

#endif /* end of include guard: __MAIN_H__ */
