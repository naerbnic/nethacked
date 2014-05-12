
// TODO(BNC): Add Docs
#ifndef PLINE_H_
#define PLINE_H_

void msgpline_add(int, char *);
void msgpline_free();
void pline(const char *, ...) PRINTF_F(1, 2);
void Norep(const char *, ...) PRINTF_F(1, 2);
void free_youbuf();
void You(const char *, ...) PRINTF_F(1, 2);
void Your(const char *, ...) PRINTF_F(1, 2);
void You_feel(const char *, ...) PRINTF_F(1, 2);
void You_cant(const char *, ...) PRINTF_F(1, 2);
void You_hear(const char *, ...) PRINTF_F(1, 2);
void pline_The(const char *, ...) PRINTF_F(1, 2);
void There(const char *, ...) PRINTF_F(1, 2);
void verbalize(const char *, ...) PRINTF_F(1, 2);
void raw_printf(const char *, ...) PRINTF_F(1, 2);
void impossible(const char *, ...) PRINTF_F(1, 2);
const char *align_str(ALIGNTYP_P);
void mstatusline(Monster *);
void ustatusline();
void self_invis_message();


#endif  // PLINE_H_
