/*
 * ioctl.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef IOCTL_H_
#define IOCTL_H_

void getwindowsz();
void getioctls();
void setioctls();
#ifdef SUSPEND
int dosuspend();
#endif /* SUSPEND */

#endif /* IOCTL_H_ */
