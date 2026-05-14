#!/bin/bash
# ============================================================
#  ManaCoin (MNC) — Full Build & Launch Script
#  Run on Ubuntu 20.04 / 22.04 / 24.04 LTS
#  Compatible with Oracle Cloud Free Tier & AWS Free Tier
# ============================================================

set -e  # Exit on any error

echo "╔══════════════════════════════════════════╗"
echo "║   ManaCoin (MNC) — Build & Launch Script ║"
echo "╚══════════════════════════════════════════╝"
echo ""

# ── STEP 1: Install dependencies ────────────────────────────
echo "▶  [1/7] Installing build dependencies..."
sudo apt-get update -y
sudo apt-get install -y \
    build-essential libtool autotools-dev automake pkg-config \
    libssl-dev libevent-dev bsdmainutils python3 \
    libboost-system-dev libboost-filesystem-dev \
    libboost-chrono-dev libboost-test-dev \
    libboost-random-dev libboost-thread-dev \
    libdb4.8-dev libdb4.8++-dev \
    libminiupnpc-dev libzmq3-dev libqt5gui5 \
    libqt5core5a libqt5dbus5 qttools5-dev \
    qttools5-dev-tools libqrencode-dev git curl
echo "✅  Dependencies installed."
echo ""

# ── STEP 2: Clone Litecoin (base for ManaCoin) ─────────────
echo "▶  [2/7] Cloning Litecoin source..."
if [ ! -d "litecoin" ]; then
    git clone https://github.com/litecoin-project/litecoin.git
fi
cd litecoin
echo "✅  Source cloned."
echo ""

# ── STEP 3: Copy ManaCoin customizations ───────────────────
echo "▶  [3/7] Applying ManaCoin customizations..."

# Copy our custom files (assumes ManaCoin/ folder is beside litecoin/)
cp ../ManaCoin/src/chainparams.cpp src/chainparams.cpp
cp ../ManaCoin/src/amount.h        src/amount.h

# Rename binary references from litecoin to manacoin
find . -name "*.cpp" -o -name "*.h" -o -name "*.py" | \
    xargs grep -l "litecoin" | \
    xargs sed -i \
        -e 's/Litecoin/ManaCoin/g' \
        -e 's/litecoin/manacoin/g' \
        -e 's/LITECOIN/MANACOIN/g' \
        -e 's/LTC/MNC/g' \
        -e 's/ltc/mnc/g' 2>/dev/null || true

echo "✅  Customizations applied."
echo ""

# ── STEP 4: Generate genesis block ─────────────────────────
echo "▶  [4/7] Generating genesis block..."
python3 ../ManaCoin/tools/genesis_generator.py
echo ""
echo "⚠️  IMPORTANT: Copy the output above into src/chainparams.cpp"
echo "   then press ENTER to continue building..."
read -p "   Press ENTER when done → "
echo ""

# ── STEP 5: Build ManaCoin ──────────────────────────────────
echo "▶  [5/7] Building ManaCoin (this takes 10-30 minutes)..."
./autogen.sh
./configure \
    --with-gui=no \
    --disable-tests \
    --disable-bench \
    --without-miniupnpc \
    CXXFLAGS="-O2"
make -j$(nproc)
echo "✅  Build complete."
echo ""

# ── STEP 6: Install ─────────────────────────────────────────
echo "▶  [6/7] Installing ManaCoin..."
sudo make install
echo "✅  Installed to /usr/local/bin/"
echo ""

# ── STEP 7: First run ───────────────────────────────────────
echo "▶  [7/7] Setting up ManaCoin node..."
mkdir -p ~/.manacoin
cp ../ManaCoin/config/manacoin.conf ~/.manacoin/manacoin.conf
echo "✅  Config file placed at ~/.manacoin/manacoin.conf"
echo ""
echo "⚠️  IMPORTANT: Edit ~/.manacoin/manacoin.conf"
echo "   Change rpcpassword to something strong!"
echo ""

echo "╔══════════════════════════════════════════╗"
echo "║          BUILD COMPLETE! 🎉              ║"
echo "╠══════════════════════════════════════════╣"
echo "║  Start node:   manacoind -daemon         ║"
echo "║  Check status: manacoin-cli getinfo       ║"
echo "║  New address:  manacoin-cli getnewaddress ║"
echo "║  Start mining: manacoin-cli generate 1   ║"
echo "╚══════════════════════════════════════════╝"
