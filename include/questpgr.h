
// TODO(BNC): Add Docs
#ifndef QUESTPGR_H_
#define QUESTPGR_H_

void load_qtlist();
void unload_qtlist();
short quest_info(int);
const char *ldrname();
bool is_quest_artifact(Object *);
void com_pager(int);
void qt_pager(int);
MonsterType *qt_montype();


#endif  // QUESTPGR_H_
