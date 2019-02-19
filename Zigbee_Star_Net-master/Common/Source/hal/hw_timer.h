/*
 * hw_timer.h
 *
 *  Created on: 2018Äê10ÔÂ14ÈÕ
 *      Author: xubov
 */

#ifndef HW_TIMER_H_
#define HW_TIMER_H_

void initPwm(void);
void pwmSet(uint8 port, uint16 Hi, uint16 Lo);
void pwmStop(uint8 port);
void pwmEn(uint8 port);
#endif /* HW_TIMER_H_ */
