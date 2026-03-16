#!/usr/bin/env bash
# scripts/download_data.sh
# Kaggle'dan Spotify 12M Songs veri setini indirir ve data/ klasörüne yerleştirir.
#
# Kullanım:
#   bash scripts/download_data.sh
#
# Ön koşullar:
#   1. Kaggle hesabı (https://www.kaggle.com)
#   2. Kaggle API token'ı — Kaggle > Account > Create New Token
#      Dosyayı ~/.kaggle/kaggle.json olarak kaydedin (Linux/macOS)
#      veya %USERPROFILE%\.kaggle\kaggle.json (Windows)
#   3. kaggle CLI kurulu olmalı:
#      pip install kaggle

set -e

DATASET="rodolfofigueroa/spotify-12m-songs"
DEST="data"

echo "==> kaggle CLI kontrol ediliyor..."
if ! command -v kaggle &> /dev/null; then
    echo "HATA: 'kaggle' komutu bulunamadı."
    echo "  pip install kaggle  komutu ile kurabilirsiniz."
    exit 1
fi

echo "==> Kaggle token kontrol ediliyor..."
KAGGLE_JSON="${KAGGLE_CONFIG_DIR:-$HOME/.kaggle}/kaggle.json"
if [ ! -f "$KAGGLE_JSON" ]; then
    echo "HATA: Kaggle API token'ı bulunamadı: $KAGGLE_JSON"
    echo "  https://www.kaggle.com/account → 'Create New Token' ile indirin."
    exit 1
fi
chmod 600 "$KAGGLE_JSON"

echo "==> Veri seti indiriliyor: $DATASET"
mkdir -p "$DEST"
kaggle datasets download -d "$DATASET" -p "$DEST" --unzip

echo ""
echo "Tamamlandı. Dosyalar: $DEST/"
ls -lh "$DEST"/*.csv 2>/dev/null || true
