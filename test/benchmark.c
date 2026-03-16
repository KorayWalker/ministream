/* test/benchmark.c */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../src/ministream.h"
#include "../src/bellek_izci.h"
#include "../src/linked_list.h"
#include "../src/hash_map.h"

double ms_olc(struct timespec t1, struct timespec t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_nsec - t1.tv_nsec) / 1e6;
}

void benchmark_arama(int n_sarki) {
    struct timespec t1, t2;
    srand(42);
    
    /* --- Linked list --- */
    izci_sifirla();
    Sarki* bas = veri_uret_liste(n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++) {
        sarki_ara_liste(bas, rand() % n_sarki);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double liste_ms = ms_olc(t1, t2);
    liste_temizle_hepsi(bas);

    /* --- Hash map --- */
    izci_sifirla();
    HashMap* map = veri_uret_map(n_sarki);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < 1000; i++) {
        sarki_ara_map(map, rand() % n_sarki);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double map_ms = ms_olc(t1, t2);
    hashmap_temizle(map);

    printf("| %8d | %10.3f ms | %10.3f ms | %7.1fx |\n",
           n_sarki, liste_ms, map_ms,
           liste_ms > 0 ? liste_ms / map_ms : 0);
}

void kopya_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    Sarki** havuz = malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        havuz[i] = sarki_olustur(i, "Havuz Track", "Artist", "Album", 200);
    }
    
    CalmaListesi** listeler = malloc(n_liste * sizeof(CalmaListesi*));
    for (int i = 0; i < n_liste; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            Sarki* kopya = (Sarki*)izlenen_malloc(sizeof(Sarki));
            memcpy(kopya, havuz[(i * sarki_per_liste + j) % n_sarki], sizeof(Sarki));
            kopya->ref_sayisi = 0;
            kopya->sonraki = NULL;
            
            if (listeler[i]->sarki_sayisi >= listeler[i]->kapasite) {
                listeler[i]->kapasite *= 2;
                listeler[i]->sarkilar = realloc(listeler[i]->sarkilar, listeler[i]->kapasite * sizeof(Sarki*));
            }
            listeler[i]->sarkilar[listeler[i]->sarki_sayisi++] = kopya;
        }
    }
    
    for (int i = 0; i < n_liste; i++) {
        for (int j = 0; j < listeler[i]->sarki_sayisi; j++) {
            izlenen_free(listeler[i]->sarkilar[j], sizeof(Sarki));
        }
        free(listeler[i]->sarkilar);
        izlenen_free(listeler[i], sizeof(CalmaListesi));
    }
    free(listeler);
    for (int i=0; i<n_sarki; i++) sarki_sil(havuz[i]);
    free(havuz);
}

void pointer_modeli_test(int n_sarki, int n_liste, int sarki_per_liste) {
    Sarki** havuz = malloc(n_sarki * sizeof(Sarki*));
    for (int i = 0; i < n_sarki; i++) {
        havuz[i] = sarki_olustur(i, "Havuz Track", "Artist", "Album", 200);
    }
    
    CalmaListesi** listeler = malloc(n_liste * sizeof(CalmaListesi*));
    for (int i = 0; i < n_liste; i++) {
        listeler[i] = liste_olustur(i, "Liste");
        for (int j = 0; j < sarki_per_liste; j++) {
            liste_sarki_ekle(listeler[i], havuz[(i * sarki_per_liste + j) % n_sarki]);
        }
    }
    
    for (int i = 0; i < n_liste; i++) {
        liste_temizle(listeler[i]); /* also frees songs automatically */
    }
    
    free(listeler);
    free(havuz);
}

void deney_calistir() {
    int N_SARKI = 10000;
    int N_LISTE = 5000;
    int SARKI_PER_LISTE = 50;
    
    struct timespec t1, t2;
    
    printf("\n--- KOPYA MODELI ---\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    kopya_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    bellek_raporu_yazdir();
    double kopya_sure = ms_olc(t1, t2);
    printf("Sure : %.2f ms\n", kopya_sure);
    
    printf("\n--- POINTER MODELI ---\n");
    izci_sifirla();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pointer_modeli_test(N_SARKI, N_LISTE, SARKI_PER_LISTE);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    bellek_raporu_yazdir();
    double pointer_sure = ms_olc(t1, t2);
    printf("Sure : %.2f ms\n", pointer_sure);
}

int main() {
    printf("\n--- ARAMA BENCHMARK (1000 sorgu) ---\n");
    printf("| N sarki  | LinkedList | HashMap    | Fark   |\n");
    printf("|----------|------------|------------|--------|\n");
    int boyutlar[] = {100, 1000, 10000, 100000};
    for (int i = 0; i < 4; i++) {
        benchmark_arama(boyutlar[i]);
    }
    
    printf("\n--- BELLEK DENEYI ---\n");
    deney_calistir();
    
    return 0;
}
