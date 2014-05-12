
// TODO(BNC): Add Docs
#ifndef VERSION_H_
#define VERSION_H_

char *version_string(char *);
char *getversionstring(char *);
int doversion();
int doextversion();
bool check_version(struct version_info *, const char *, bool);
unsigned long get_feature_notice_ver(char *);
unsigned long get_current_feature_ver();
#ifdef RUNTIME_PORT_ID
void append_port_id(char *);
#endif


#endif  // VERSION_H_
