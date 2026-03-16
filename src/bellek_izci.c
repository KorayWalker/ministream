/* bellek_izci.c */
#include <stdio.h>
#include "bellek_izci.h"

static BellekIzci izci = {0};

void* izlenen_malloc(size_t boyut) {
    void* ptr = malloc(boyut);
    if (ptr) {
        izci.toplam_ayrildi += boyut;
        izci.malloc_sayisi++;
    }
    return ptr;
}

void izlenen_free(void* ptr, size_t boyut) {
    if (ptr) {
        free(ptr);
        izci.toplam_serbest += boyut;
        izci.free_sayisi++;
    }
}

void izci_sifirla(void) {
    izci.toplam_ayrildi = 0;
    izci.toplam_serbest = 0;
    izci.malloc_sayisi = 0;
    izci.free_sayisi = 0;
}

size_t aktif_bellek(void) {
    return izci.toplam_ayrildi - izci.toplam_serbest;
}

void bellek_raporu_yazdir(void) {
    printf("--- BELLEK RAPORU ---\n");
    printf("malloc : %5d kez, %10zu byte\n", izci.malloc_sayisi, izci.toplam_ayrildi);
    printf("free   : %5d kez, %10zu byte\n", izci.free_sayisi, izci.toplam_serbest);
    printf("aktif  : %10zu byte (%.2f MB)\n", aktif_bellek(), aktif_bellek() / 1048576.0);
    
    if (izci.toplam_ayrildi == izci.toplam_serbest)
        printf("SIZINTI: 0 byte V\n");
    else
        printf("SIZINTI: %zu byte A - free eksik!\n", izci.toplam_ayrildi - izci.toplam_serbest);
    printf("---------------------------\n");
}

/* --- Getter fonksiyonları --- */
int izci_malloc_sayisi(void) { return izci.malloc_sayisi; }
int izci_free_sayisi(void) { return izci.free_sayisi; }
size_t izci_toplam_ayrildi(void) { return izci.toplam_ayrildi; }
size_t izci_toplam_serbest(void) { return izci.toplam_serbest; }
