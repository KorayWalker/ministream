# MiniStream - Tasarım Raporu
**Ögrenci:** Koray
**Tarih:** 11.03.2026

## 1. Kopya Modeli vs Pointer Modeli
### Hipotez
Pointer modeli sadece bellek adreslerini kopyaladıgı için bellek kullanımı ve islemci hızı açısından Kopya modeline göre kat kat daha verimli olacaktır. Büyük veri setlerinde sarkı verilerinin her listede tekrar tekrar kopyalanması muazzam bir bellek israfı ve kopyalama yükü yaratır.

### Ölçüm Sonuçları
(N=10000 Sarkı, M=5000 Liste, 50 Sarkı/Liste)

| Metrik | Kopya Modeli | Pointer Modeli | Fark |
| ------ | ------------ | -------------- | ---- |
| malloc sayısı | 265000 kez | 15000 kez | 17.6x daha az |
| Bellek (MB) | 81.67 MB | 3.47 MB | 23.5x daha az |
| Süre (ms) | 66.15 ms | 5.69 ms | 11.6x daha hızlı |

### Yorum
Ölçüm sonuçlarından açıkça anlasıldıgı üzere, Pointer modeli bellek tahsis (allocation) miktarını devasa oranda (81.6 MB'tan 3.4 MB'a - 23.5 kat) azaltmıs ve çalısma süresini kopyalama islem süreçlerini baypas ederek 11.6 kat kısaltmıstır. Bunun nedeni 5000 listede yer alan 250,000 yeni sarkı referansı olusturulması sırasında yeni struct (`malloc`) kopyalamak yerine yalnızca 8 baytlık bellek adresinin eklenmesidir.

## 2. Linked List vs Hash Map
### Benchmark Tablosu
| N sarki | Linked List | Hash Map | Fark |
| ------- | ----------- | -------- | ---- |
| 100 | 0.079 ms | 0.008 ms | 9.6x |
| 1000 | 1.062 ms | 0.009 ms | 124.9x |
| 10000 | 14.246 ms | 0.118 ms | 121.0x |
| 100000 | 2893.191 ms | 7.137 ms | 405.4x |

### Yorum
Hash map `O(1)` sabit zamanlı karmasıklıga sahip oldugu için eleman sayısının (N) artmasından (çakısmalar hariç) neredeyse hiç etkilenmezken, Linked List `O(n)` karmasıklıga sahip oldugu için süre N ile eszamanlı olarak lineer bir sekilde büyümüstür. Ancak sistemde veri seti çok küçük oldugunda (örnegin 1-10 arası) Hash Map tablolarını ayarlamak (1024'lük NULL atamaları) ve bellekte atlama yapmak overhead (gereksiz islem yükü) getireceginden, memory cache optimizasyonu için sıralı bellekte çalısan Linked List kullanmak daha mantıklı olabilir.

## 3. ref_sayisi Olmasaydı Ne Olurdu?
### Deney
Eger `ref_sayisi` (Referans Sayacı) kurgusu iptal edilseydi, ilk çalma listesi temizlendiginde sistem o listeye baglı olan bütün `Sarki` objelerini dogrudan memory'den `free()` edecekti. Eger silinen bu parça aynı zamanda baska müzik listelerine ekliyse, bu ikinci liste üzerindeki pointer tamamen geçersiz kalacaktı (dangling pointer).

### Valgrind Çıktısı
```text
==5128== Invalid read of size 4
==5128==    at 0x10317B: liste_sarki_cikar (ministream.c:75)
==5128==  Address 0x546be80 is 0 bytes after a block of size 344 free'd
==5128==    at 0x103420: sarki_sil (ministream.c:45)
```

### Yorum
Bu hata literatürde "Use-After-Free" (Serbest bırakıldıktan sonra kullanım) olarak geçer. Neden oldukça tehlikelidir? Çünkü C, bellek güvenligini otomatik garanti etmez. Ilk `free` çagrısı yapıldıktan sonra isletim sistemi bu bellegi programa yeniden tahsis edebilir. Eger bu serbest bellegi okumaya veya modifiye etmeye çalısırsanız, o sırada baska bir uygulamanın (veya kendi programınızın farklı bir tarafının) datasını ezmis/kullanmıs olursunuz. Isletim sistemi RAM kurallarını yakalayıp ihlal verirse "Segmentation Fault" yasanır ve uygulama aniden çöker. Yakalayamazsa sessizce mantık hatasıyla veya yanlıs degerlerle arka planda çalısıp veritabanını bozmaya devam eder.

## 4. 10x Büyütme Analizi
### Hesaplama
Sistemi mevcut tasarımdan (500 bin kullanıcı, her birine 20 liste) 10x ölçekleyerek 5,000,000 kullanıcı boyutuna tasısaydık olusacak metrikler:
`5,000,000 kullanıcı x 20 liste x 50 sarkı = 5,000,000,000 sarkı referansı`
* Pointer dizisi (`sarkilar`) bellek masrafı: `5,000,000,000 x 8 byte = 40 GB`
* `CalmaListesi` objelerinin asıl kendi maliyeti (`sizeof=~64 byte`): `100,000,000 x 64 = 6.4 GB`
Yani yalnızca listeleri tutmak için anında en az `46.4 GB` civarı aktif bir RAM gerekecekti.

### Mimari Degisiklik Önerileri
Sistemde bellek darbogazını (bottleneck) kırmak için kökten birkaç mimari degisiklik gereklidir. Gündelik hayatta (örnegin Spotify'da) milyonlarca kullanıcı var fakat bu kullanıcıların hiçbiri tüm anlarda 20 listesini de aktif olarak okumaz. Ilk degisikligim Lazy-Loading (veya Sayfalama) mimarisini getirmek olurdu; veriler veritabanında (Database) bekletilmeli ve sadece client okuma istegi attıgında C programına (RAM'e) yüklenmelidir. Ayrıca, Hash map yapımızdaki `1024` sabit kapasitesini Dinamik (Load Factor'e baglı, büyüyen) bir yapıya tasırdım; aksi takdirde 5 milyonluk kullanıcıda collision (çakısma) oranı `O(n)`'e çok yakınsayıp Hash mantıgını Linked list seviyesinde yavaslatırdı.
