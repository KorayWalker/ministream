/* bellek_izci.h */
#ifndef BELLEK_IZCI_H
#define BELLEK_IZCI_H

#include <stdlib.h>

typedef struct {
    size_t toplam_ayrildi;  /* şimdiye kadar malloc edilen toplam bayt */
    size_t toplam_serbest;  /* şimdiye kadar free edilen toplam bayt */
    int malloc_sayisi;      /* kaç kez malloc çağrıldı */
    int free_sayisi;        /* kaç kez free çağrıldı */
} BellekIzci;

/* Tüm projede bu iki fonksiyonu kullan, malloc/free'yi DOĞRUDAN kullanma */
void* izlenen_malloc(size_t boyut);
void izlenen_free(void* ptr, size_t boyut);

void izci_sifirla(void);
void bellek_raporu_yazdir(void);
size_t aktif_bellek(void);

/* Getter fonksiyonları - izci değişkeni static olduğu için test dosyalarından doğrudan erişilemez. Bu fonksiyonları kullanın. */
int izci_malloc_sayisi(void);
int izci_free_sayisi(void);
size_t izci_toplam_ayrildi(void);
size_t izci_toplam_serbest(void);

#endif
