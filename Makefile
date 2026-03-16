# Derleyici ve bayraklar
CC = gcc
CFLAGS = -g -Wall -Wextra

# Kaynak dosyalar
SRC = src/ministream.c src/bellek_izci.c src/hash_map.c src/linked_list.c

# --- Hedefler ---
all: test_temel test_bellek benchmark ministream.so

test_temel: $(SRC) test/test_temel.c
	$(CC) $(CFLAGS) $(SRC) test/test_temel.c -o test_temel

test_bellek: $(SRC) test/test_bellek.c
	$(CC) $(CFLAGS) $(SRC) test/test_bellek.c -o test_bellek

benchmark: $(SRC) test/benchmark.c
	$(CC) $(CFLAGS) $(SRC) test/benchmark.c -o benchmark

# Opsiyonel: shared library (Python wrapper için)
ministream.so: $(SRC)
	$(CC) -shared -fPIC $(CFLAGS) $(SRC) -o ministream.so

# --- Veri ---

# Sentetik veri: Kaggle olmadan 100 000 şarkı üretir (data/sarkilar.csv)
veri: data/uretec
	./data/uretec 100000

data/uretec: data/uretec.c
	$(CC) $(CFLAGS) data/uretec.c -o data/uretec

# Gerçek veri: Kaggle API ile indir (kaggle CLI + token gerekir)
kaggle-veri:
	bash scripts/download_data.sh

# --- Test ve Temizlik ---
test: test_temel test_bellek
	./test_temel
	./test_bellek

valgrind: test_temel
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test_temel

clean:
	rm -f test_temel test_bellek benchmark ministream.so test_temel.exe test_bellek.exe benchmark.exe data/uretec data/uretec.exe
