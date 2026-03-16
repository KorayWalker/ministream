/* src/ministream.h */
#ifndef MINISTREAM_H
#define MINISTREAM_H

#include <stdlib.h>

/* --- Şarkı --- */
typedef struct Sarki {
    int id;
    char baslik[100];      /* name - şarkı adı */
    char sanatci[100];     /* artists - sanatçı adı */
    char album[100];       /* album - albüm adı */
    int sure_sn;           /* duration_ms / 1000 -> saniyeye çevrilmiş */
    int yil;               /* year - çıkış yılı */
    int ref_sayisi;        /* kaç çalma listesinde kullanılıyor */
    struct Sarki* sonraki; /* linked list için next pointer */
} Sarki;

/* --- Çalma Listesi --- */
typedef struct {
    int id;
    char isim[50];
    Sarki** sarkilar;      /* heap'teki pointer dizisi - Sarki* lari tutar */
    int sarki_sayisi;      /* şu an listede kaç şarkı var */
    int kapasite;          /* dizi kapasitesi (realloc ile büyüyecek) */
} CalmaListesi;

/* --- Kullanıcı --- */
typedef struct {
    int id;
    char isim[50];
    CalmaListesi** listeler; /* kullanıcının çalma listeleri */
    int liste_sayisi;
} Kullanici;

/* --- Temel Operasyonlar --- */
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci, const char* album, int sure);
int sarki_sil(Sarki* sarki);

CalmaListesi* liste_olustur(int id, const char* isim);
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki);
void liste_sarki_cikar(CalmaListesi* liste, int idx);
void liste_temizle(CalmaListesi* liste);

/* CSV Okuma */
Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam);

/* ... Other declarations will be inside linked_list.h / hash_map.h ... */

#endif
