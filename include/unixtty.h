
// TODO(BNC): Add Docs
#ifndef UNIXTTY_H_
#define UNIXTTY_H_

void gettty();
void settty(const char *);
void setftty();
void intron();
void introff();
void ErrMsg(const char *, ...) PRINTF_F(1, 2);
void VErrMsg(const char *, va_list);
void error(const char *, ...) PRINTF_F(1, 2);


#endif  // UNIXTTY_H_
