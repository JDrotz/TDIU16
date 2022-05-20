#pragma once

/**
 * Basic timing facilities. Mostly follows the API in Pintos, but uses ms as a base unit instead of
 * the less well-defined unit 'ticks'.
 *
 * The value returned from 'timer_ticks' is in some undefined unit, but the value returned from
 * 'timer_elapsed' is always in ms.
 */

int64_t timer_ticks(void);
double  timer_elapsed(int64_t since);
