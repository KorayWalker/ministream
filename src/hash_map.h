/* src/hash_map.h */
#ifndef HASH_MAP_H
#define HASH_MAP_H
#include "ministream.h"

#define TABLO_BOYUTU 1024

typedef struct {
    Sarki* kovalar[TABLO_BOYUTU];
} HashMap;

HashMap* hashmap_olustur(void);
void hashmap_ekle(HashMap* map, Sarki* sarki);
Sarki* sarki_ara_map(HashMap* map, int id);
void hashmap_temizle(HashMap* map);
HashMap* veri_uret_map(int n);

#endif
