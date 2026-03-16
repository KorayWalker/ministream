# ministream

MiniStream, C diliyle yazılmış hafif bir müzik akışı (streaming) kütüphanesidir.  
Bellek yönetimi, pointer modeli ve hash map yapılarını inceleyen bir eğitim projesidir.

---

## Kurulum

```bash
# Derleme
make

# Testleri çalıştır
make test
```

---

## Veri Seti

Proje, Kaggle'daki [Spotify 12M Songs](https://www.kaggle.com/datasets/rodolfofigueroa/spotify-12m-songs) veri setini kullanmaktadır.  
Bu dosya ~1 GB boyutunda olduğundan doğrudan repoya **eklenemez**.

### Seçenek 1 — Sentetik Veri (Kaggle hesabı gerekmez)

`data/uretec.c` aracı, aynı CSV formatında rastgele şarkı verisi üretir:

```bash
make veri          # 100 000 şarkı üretir → data/sarkilar.csv
# veya özel boyut:
./data/uretec 500000
```

### Seçenek 2 — Gerçek Kaggle Verisi

1. [https://www.kaggle.com](https://www.kaggle.com) adresinde ücretsiz hesap oluşturun.
2. **Account → Create New Token** ile `kaggle.json` dosyasını indirin.
3. Token'ı aşağıdaki konuma koyun:
   - **Linux / macOS:** `~/.kaggle/kaggle.json`
   - **Windows:** `%USERPROFILE%\.kaggle\kaggle.json`
4. Kaggle CLI'ı yükleyin ve veri setini indirin:

```bash
pip install kaggle
make kaggle-veri
```

> **Not:** `data/*.csv` dosyaları `.gitignore` ile repo dışında tutulmaktadır.  
> Projeyi klonladıktan sonra yukarıdaki adımlardan birini uygulayarak veriyi yerel olarak oluşturun.

---

## Proje Yapısı

```
ministream/
├── src/           # Kütüphane kaynak kodları (C)
├── test/          # Birim testleri ve benchmark
├── data/          # Veri üretici (uretec.c) — CSV dosyaları .gitignore'da
├── backend/       # Python HTTP sunucu (ministream.so ile)
├── dashboard/     # Web arayüzü
├── scripts/       # Yardımcı betikler (Kaggle indirme vb.)
└── rapor/         # Tasarım raporu
```