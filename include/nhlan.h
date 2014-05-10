/*	SCCS Id: @(#)nhlan.h	3.4	1997/04/12	*/
/* Copyright (c) Michael Allison, 1997			*/
/* NetHack may be freely redistributed.  See license for details. */

#ifndef NHLAN_H
#define NHLAN_H
/*
 * Here are the LAN features currently implemented:
 * LAN_MAIL		Mail facility allowing receipt and
 *			reading of mail.
 * LAN_SHARED_BONES	Allows bones files to be stored on a
 *			network share. (Does NOT imply compatibiliy
 *			between unlike platforms)
 */

#ifdef LAN_FEATURES
#ifdef LAN_MAIL
#define MAIL
#define MAILCKFREQ 50

#ifndef MAX_BODY_SIZE
#define MAX_BODY_SIZE 1024
#endif

struct lan_mail_struct {
  char sender[120];
  char subject[120];
  bool body_in_ram; /* TRUE means body in memory not file */
  char filename[_MAX_PATH];
  char body[MAX_BODY_SIZE];
};
#endif

#endif /*LAN_FEATURES*/
#ifdef LAN_FEATURES
void init_lan_features();
char *lan_username();
#ifdef LAN_MAIL
bool lan_mail_check();
void lan_mail_read(Object *);
void lan_mail_init();
void lan_mail_finish();
void lan_mail_terminate();
#endif
#endif

#endif /*NHLAN_H*/
