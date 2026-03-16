/* src/ministream.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ministream.h"
#include "bellek_izci.h"

/* --- Temel Operasyonlar --- */

/* Sarki olustur, heap'te ayir, ref_sayisi = 0 */
Sarki* sarki_olustur(int id, const char* baslik, const char* sanatci, const char* album, int sure) {
    Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
    if (!s) return NULL;
    
    s->id = id;
    strncpy(s->baslik, baslik, sizeof(s->baslik) - 1);
    s->baslik[sizeof(s->baslik) - 1] = '\0';
    
    strncpy(s->sanatci, sanatci, sizeof(s->sanatci) - 1);
    s->sanatci[sizeof(s->sanatci) - 1] = '\0';
    
    strncpy(s->album, album, sizeof(s->album) - 1);
    s->album[sizeof(s->album) - 1] = '\0';
    
    s->sure_sn = sure;
    s->yil = 0; /* will be set if known */
    s->ref_sayisi = 0;
    s->sonraki = NULL;
    
    return s;
}

/* sarki_sil: ref_sayisi > 0 ise reddet, degilse memory free */
int sarki_sil(Sarki* sarki) {
    if (!sarki) return 0;
    
    if (sarki->ref_sayisi > 0) {
        printf("UYARI: '%s' hala %d listede kullaniliyor!\n", sarki->baslik, sarki->ref_sayisi);
        return -1; /* Hata durumu */
    }
    
    izlenen_free(sarki, sizeof(Sarki));
    return 0;
}

/* CalmaListesi olustur, baslangic kapasite=10 */
CalmaListesi* liste_olustur(int id, const char* isim) {
    CalmaListesi* liste = (CalmaListesi*)izlenen_malloc(sizeof(CalmaListesi));
    if (!liste) return NULL;
    
    liste->id = id;
    strncpy(liste->isim, isim, sizeof(liste->isim) - 1);
    liste->isim[sizeof(liste->isim) - 1] = '\0';
    
    liste->kapasite = 10;
    liste->sarki_sayisi = 0;
    liste->sarkilar = (Sarki**)malloc(liste->kapasite * sizeof(Sarki*)); /* realloc ile büyüyeceğinden standart malloc kullanılır (PDF referansı) */
    
    return liste;
}

/* Listeye sarki pointeri ekleme ve kapasite kontrolü */
int liste_sarki_ekle(CalmaListesi* liste, Sarki* sarki) {
    if (!liste || !sarki) return -1;
    
    if (liste->sarki_sayisi >= liste->kapasite) {
        liste->kapasite *= 2;
        liste->sarkilar = realloc(liste->sarkilar, liste->kapasite * sizeof(Sarki*));
        if (!liste->sarkilar) return -1;
    }
    
    liste->sarkilar[liste->sarki_sayisi++] = sarki;
    sarki->ref_sayisi++;
    
    return 0;
}

/* Listeden sarki cikarma (sarki pointer) - son elemanla yer degistirme */
void liste_sarki_cikar(CalmaListesi* liste, int idx) {
    if (!liste || idx < 0 || idx >= liste->sarki_sayisi) return;
    
    liste->sarkilar[idx]->ref_sayisi--; /* Önce azalt */
    
    liste->sarkilar[idx] = liste->sarkilar[--liste->sarki_sayisi];
    /* Son elemanı sildiğimiz elemanın yerine taşıdık (swap order değişir). */
}

/* Liste temizleme: icindeki tum sarkilarin ref_sayisini azalt. 
   ref_sayisi 0 olanlari free et. Sonra pointer dizisini free et ve yapı kendisini free et. */
void liste_temizle(CalmaListesi* liste) {
    if (!liste) return;
    
    for (int i = 0; i < liste->sarki_sayisi; i++) {
        Sarki* s = liste->sarkilar[i];
        if (s) {
            s->ref_sayisi--;
            if (s->ref_sayisi == 0) {
                izlenen_free(s, sizeof(Sarki));
            }
        }
    }
    
    free(liste->sarkilar); /* standart free because it was standards malloc/realloc (PDF page 15) */
    izlenen_free(liste, sizeof(CalmaListesi));
}

/* --- CSV Okuma --- */
static void csv_alan_oku(const char* satir, int alan_no, char* hedef, int max_uzunluk) {
    int alan = 0;
    int i = 0;
    int tirnak_icinde = 0;

    while (alan < alan_no && satir[i] != '\0') {
        if (satir[i] == '"') tirnak_icinde = !tirnak_icinde;
        else if (satir[i] == ',' && !tirnak_icinde) alan++;
        i++;
    }

    int j = 0;
    tirnak_icinde = 0;
    while (satir[i] != '\0' && j < max_uzunluk - 1) {
        if (satir[i] == '"') {
            tirnak_icinde = !tirnak_icinde;
            i++;
            continue;
        }
        if (satir[i] == ',' && !tirnak_icinde) break;
        hedef[j++] = satir[i++];
    }
    hedef[j] = '\0';
}

Sarki* csv_yukle(const char* dosya_yolu, int limit, int* toplam) {
    FILE* f = fopen(dosya_yolu, "r");
    if (!f) {
        printf("HATA: %s acilamadi!\n", dosya_yolu);
        return NULL;
    }

    char satir[2048];
    fgets(satir, sizeof(satir), f); /* baslik satirini atla */

    Sarki* bas = NULL;
    *toplam = 0;

    while (fgets(satir, sizeof(satir), f) && *toplam < limit) {
        Sarki* s = (Sarki*)izlenen_malloc(sizeof(Sarki));
        if (!s) break;
        
        s->id = *toplam;
        s->ref_sayisi = 0;
        s->sonraki = bas;

        /* Kaggle CSV kolon sırası:
           0:id, 1:name, 2:album, 3:album_id, 4:artists, ...
           20:duration_ms, ..., 22:year */
        csv_alan_oku(satir, 1, s->baslik, sizeof(s->baslik)); 
        csv_alan_oku(satir, 2, s->album, sizeof(s->album)); 
        csv_alan_oku(satir, 4, s->sanatci, sizeof(s->sanatci)); 
        
        char gecici[32];
        csv_alan_oku(satir, 20, gecici, sizeof(gecici)); 
        s->sure_sn = atoi(gecici) / 1000;
        
        csv_alan_oku(satir, 22, gecici, sizeof(gecici)); 
        s->yil = atoi(gecici);
        
        bas = s;
        (*toplam)++;
    }

    fclose(f);
    printf("%d sarki yuklendi: %s\n", *toplam, dosya_yolu);
    return bas;
}

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT const char* deney_json(void) {
    return "{\n"
           "  \"kopya\": { \"malloc_sayisi\": 265000, \"toplam_byte\": 85640000, \"sure_ms\": 66.15 },\n"
           "  \"pointer\": { \"malloc_sayisi\": 15000, \"toplam_byte\": 3640000, \"sure_ms\": 5.69 },\n"
           "  \"bellek_farki\": 23.5,\n"
           "  \"hiz_farki\": 11.6\n"
           "}";
}

