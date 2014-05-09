/*
 * exper.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef EXPER_H_
#define EXPER_H_

int experience(Monster *, int);
void more_experienced(int, int);
void losexp(const char *);
void newexplevel();
void pluslvl(bool);
long rndexp(bool);

#endif /* EXPER_H_ */
