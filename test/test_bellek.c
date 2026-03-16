/* test/test_bellek.c */
#include <stdio.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"

void test_toplu_olustur_sil() {
    printf("\n[BELLEK TEST] toplu_olustur/sil\n");
    izci_sifirla();
    for (int i = 0; i < 10000; i++) {
        Sarki* s = sarki_olustur(i, "Test", "Sanatci", "Album", 200);
        izlenen_free(s, sizeof(Sarki));
    }
    bellek_raporu_yazdir();
}

void test_liste_bellek() {
    printf("\n[BELLEK TEST] liste_bellek_yonetimi\n");
    izci_sifirla();
    Sarki* sarkilar[100];
    for (int i = 0; i < 100; i++) {
        sarkilar[i] = sarki_olustur(i, "Track", "Artist", "Album", 180);
    }

    CalmaListesi* listeler[100];
    for (int i = 0; i < 100; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        /* Sabit ve hatasiz test dizaynı yapıldı */
        for (int j = 0; j < 50; j++) {
            liste_sarki_ekle(listeler[i], sarkilar[(i + j) % 100]);
        }
    }
    
    for (int i = 0; i < 100; i++) {
        liste_temizle(listeler[i]); /* Tüm müziklerin ref değeri 0 a düşecek ve serbest bırakılacaklar */
    }
    
    bellek_raporu_yazdir();
}

int main() {
    printf("--- MiniStream Bellek Testleri ---\n");
    test_toplu_olustur_sil();
    test_liste_bellek();
    printf("\n-----------------------------------\n");
    return 0;
}
