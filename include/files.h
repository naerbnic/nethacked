/*
 * files.h
 *
 *  Created on: May 9, 2014
 *      Author: brianchin
 */

#ifndef FILES_H_
#define FILES_H_

#include "dungeon.h"

char *fname_encode(const char *, char, char *, char *, int);
char *fname_decode(char, char *, char *, int);
const char *fqname(const char *, int, int);
FILE *fopen_datafile(const char *, const char *, int);
bool uptodate(int, const char *);
void store_version(int);
void set_levelfile_name(char *, int);
int create_levelfile(int, char *);
int open_levelfile(int, char *);
void delete_levelfile(int);
void clearlocks();
int create_bonesfile(d_level *, char **, char *);
void commit_bonesfile(d_level *);
int open_bonesfile(d_level *, char **);
int delete_bonesfile(d_level *);
void compress_bonesfile();
void set_savefile_name();
#ifdef INSURANCE
void save_savefile_name(int);
#endif
#if defined(WIZARD)
void set_error_savefile();
#endif
int create_savefile();
int open_savefile();
int delete_savefile();
int restore_saved_game();
void compress(const char *);
void uncompress(const char *);
bool lock_file(const char *, int, int);
void unlock_file(const char *);
#ifdef USER_SOUNDS
bool can_read_file(const char *);
#endif
void read_config_file(const char *);
void check_recordfile(const char *);
#if defined(WIZARD)
void read_wizkit();
#endif
void paniclog(const char *, const char *);
int validate_prefix_locations(char *);
char **get_saved_games();
void free_saved_games(char **);
#ifdef SELF_RECOVER
bool recover_savefile();
#endif
#ifdef HOLD_LOCKFILE_OPEN
void really_close();
#endif

#endif /* FILES_H_ */
