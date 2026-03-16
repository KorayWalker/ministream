/* src/hash_map.c */
#include "hash_map.h"
#include "linked_list.h"
#include "bellek_izci.h"

int hash_fonk(int id) {
    return id % TABLO_BOYUTU;
}

HashMap* hashmap_olustur(void) {
    HashMap* map = (HashMap*)izlenen_malloc(sizeof(HashMap));
    if (!map) return NULL;
    for (int i = 0; i < TABLO_BOYUTU; i++) {
        map->kovalar[i] = NULL;
    }
    return map;
}

void hashmap_ekle(HashMap* map, Sarki* sarki) {
    if (!map || !sarki) return;
    int idx = hash_fonk(sarki->id);
    sarki->sonraki = map->kovalar[idx];
    map->kovalar[idx] = sarki;
}

Sarki* sarki_ara_map(HashMap* map, int id) {
    if (!map) return NULL;
    int idx = hash_fonk(id);
    return sarki_ara_liste(map->kovalar[idx], id);
}

void hashmap_temizle(HashMap* map) {
    if (!map) return;
    for (int i = 0; i < TABLO_BOYUTU; i++) {
        Sarki* curr = map->kovalar[i];
        while (curr) {
            Sarki* sonraki = curr->sonraki;
            izlenen_free(curr, sizeof(Sarki));
            curr = sonraki;
        }
    }
    izlenen_free(map, sizeof(HashMap));
}

HashMap* veri_uret_map(int n) {
    HashMap* map = hashmap_olustur();
    for (int i = 0; i < n; i++) {
        Sarki* s = sarki_olustur(i, "TestTrack", "Artist", "Album", 180);
        if (s) hashmap_ekle(map, s);
    }
    return map;
}
