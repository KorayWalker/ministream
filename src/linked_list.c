/* src/linked_list.c */
#include "linked_list.h"
#include "bellek_izci.h"

Sarki* sarki_ara_liste(Sarki* bas, int id) {
    Sarki* curr = bas;
    while (curr != NULL) {
        if (curr->id == id) return curr;
        curr = curr->sonraki;
    }
    return NULL;
}

Sarki* veri_uret_liste(int n) {
    Sarki* bas = NULL;
    for (int i = 0; i < n; i++) {
        Sarki* s = sarki_olustur(i, "TestTrack", "Artist", "Album", 180);
        if (s) {
            s->sonraki = bas;
            bas = s;
        }
    }
    return bas;
}

void liste_temizle_hepsi(Sarki* bas) {
    Sarki* curr = bas;
    while (curr) {
        Sarki* sonraki = curr->sonraki;
        izlenen_free(curr, sizeof(Sarki));
        curr = sonraki;
    }
}
