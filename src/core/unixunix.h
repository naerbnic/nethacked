
// TODO(BNC): Add Docs
#ifndef UNIXUNIX_H_
#define UNIXUNIX_H_

void getlock();
void regularize(char *);
#if defined(TIMED_DELAY) && !defined(msleep) && defined(SYSV)
void msleep(unsigned);
#endif
#ifdef SHELL
int dosh();
#endif /* SHELL */
#if defined(SHELL) || defined(DEF_PAGER) || defined(DEF_MAILREADER)
int child(int);
#endif


#endif  // UNIXUNIX_H_
