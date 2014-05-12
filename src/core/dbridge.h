/*
 * dbridge.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef DBRIDGE_H_
#define DBRIDGE_H_

extern bool is_pool(int, int);
extern bool is_lava(int, int);
extern bool is_ice(int, int);
extern int is_drawbridge_wall(int, int);
extern bool is_db_wall(int, int);
extern bool find_drawbridge(int *, int *);
extern bool create_drawbridge(int, int, int, bool);
extern void open_drawbridge(int, int);
extern void close_drawbridge(int, int);
extern void destroy_drawbridge(int, int);

#endif /* DBRIDGE_H_ */
