
// TODO(BNC): Add Docs
#ifndef MKMAZE_H_
#define MKMAZE_H_

void wallification(int, int, int, int);
void walkfrom(int, int);
void makemaz(const char *);
void mazexy(coord *);
void bound_digging();
void mkportal(xchar, xchar, xchar, xchar);
bool bad_location(xchar, xchar, xchar, xchar, xchar, xchar);
void place_lregion(xchar, xchar, xchar, xchar, xchar, xchar, xchar, xchar,
                     xchar, d_level *);
void movebubbles();
void water_friction();
void save_waterlevel(int, int);
void restore_waterlevel(int);
const char *waterbody_name(xchar, xchar);


#endif  // MKMAZE_H_
