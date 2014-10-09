/** file:       aclock.h
  * author:     <<AUTHOR>>
  */

#ifndef __CLOCK_H__
#define __CLOCK_H__

//___ I N C L U D E S ________________________________________________________

//___ M A C R O S ____________________________________________________________

//___ T Y P E D E F S ________________________________________________________

typedef struct aclock_state_t {
    /* similar to rtc_calendar_time */
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    bool     pm;
    /** Day value, where day 1 is the first day of the month. */
    uint8_t  day;
    /** Month value, where month 1 is January. */
    uint8_t  month;
    uint16_t year;
} aclock_state_t;

//___ V A R I A B L E S ______________________________________________________

//___ P R O T O T Y P E S ____________________________________________________

void aclock_init( aclock_state_t start_state );
  /* @brief initalize clock module
   * @param start_state - startup values for clock (time, date)
   * @retrn None
   */



#endif /* end of include guard: __CLOCK_H__ */
// vim: shiftwidth=2