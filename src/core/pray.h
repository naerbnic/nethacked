
// TODO(BNC): Add Docs
#ifndef PRAY_H_
#define PRAY_H_

int dosacrifice();
bool can_pray(bool);
int dopray();
const char *u_gname();
int doturn();
const char *a_gname();
const char *a_gname_at(xchar x, xchar y);
const char *align_gname(ALIGNTYP_P);
const char *halu_gname(ALIGNTYP_P);
const char *align_gtitle(ALIGNTYP_P);
void altar_wrath(int, int);


#endif  // PRAY_H_
