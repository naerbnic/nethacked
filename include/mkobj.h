
// TODO(BNC): Add Docs
#ifndef MKOBJ_H_
#define MKOBJ_H_

Object *MakeRandomObjectAt(char, int, int, bool);
Object *MakeSpecificObjectAt(int, int, int, bool, bool);
Object *MakeRandomObject(char, bool);
int PickRandomMonsterTypeIndex();
Object *SplitObject(Object *, long);
void ReplaceObject(Object *, Object *);
void CreateBillDummyObject(Object *);
Object *MakeSpecificObject(int, bool, bool);
int GetBUCSign(Object *);
int GetWeight(Object *);
Object *MakeGold(long, int, int);
Object *MakeCorpseOrStatue(int, Monster *, MonsterType *, int, int, bool);
Object *AttachMonsterIdToObject(Object *, unsigned);
Monster *NewMonsterFromObject(Object *, bool);
Object *NewTopTenObject(int, int, int);
Object *MakeNamedCorpseOrStatue(int, MonsterType *, int, int, const char *);
Object *MakeRandomTreefruitAt(int, int);
void StartCorpseTimeout(Object *);
void Bless(Object *);
void Unbless(Object *);
void Curse(Object *);
void Uncurse(Object *);
void BlessOrCurse(Object *, int);
bool IsFlammable(Object *);
bool IsRottable(Object *);
void PlaceObject(Object *, int, int);
void RemoveObjectFromFloor(Object *);
void DiscardMonsterInventory(Monster *);
void RemoveObjectFromStorage(Object *);
void ExtractObjectFromList(Object *, Object **);
void extract_nexthere(Object *, Object **);
int AddObjectToMonsterInventory(Monster *, Object *);
Object *AddObjectToContainer(Object *, Object *);
void AddObjectToMigrationList(Object *);
void AddToBuriedList(Object *);
void DeallocateObject(Object *);
void ApplyIceEffectsAt(int, int, bool);
long PeekAtIcedCorpseAge(Object *);
#ifdef WIZARD
void SanityCheckObjects();
#endif


#endif  // MKOBJ_H_
