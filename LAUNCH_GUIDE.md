# ManaCoin (MNC) — Complete Launch Guide

> **Your coin. Your blockchain. Your rules.**
> Built from scratch, minable by anyone, tradeable on DEXs.

---

## 📋 ManaCoin Specs

| Parameter         | Value                          |
|-------------------|-------------------------------|
| Name              | ManaCoin                       |
| Ticker            | MNC                            |
| Algorithm         | Scrypt (CPU/GPU mineable)      |
| Total Supply      | 84,000,000 MNC                 |
| Block Reward      | 50 MNC (halves every 840,000 blocks) |
| Block Time        | 2.5 minutes                    |
| Halving Interval  | 840,000 blocks (~4 years)      |
| P2P Port          | 9888                           |
| RPC Port          | 9889                           |
| Address Prefix    | M (starts with M)              |
| Bech32 Prefix     | mnc                            |

---

## 🖥️ PHASE 1 — Set Up Your Server (Free)

### Option A: Oracle Cloud Free Tier (Recommended)
1. Sign up at https://www.oracle.com/cloud/free/
2. Create an **ARM VM** (Always Free):
   - Shape: `VM.Standard.A1.Flex`
   - RAM: 6 GB, CPU: 4 cores
   - OS: **Ubuntu 22.04 LTS**
3. Open ports **9888** and **9889** in firewall settings
4. SSH into your server

### Option B: AWS Free Tier
1. Sign up at https://aws.amazon.com/free/
2. Launch **EC2 t2.micro** (Ubuntu 22.04)
3. Open ports 9888, 9889 in Security Group

---

## ⚙️ PHASE 2 — Build ManaCoin Node

```bash
# 1. Upload files to your server
scp -r ManaCoin/ ubuntu@YOUR_SERVER_IP:~/

# 2. Run the build script
cd ~
chmod +x ManaCoin/build.sh
./ManaCoin/build.sh
```

The script will:
- Install all dependencies automatically
- Clone Litecoin source code
- Apply ManaCoin customizations
- Generate your genesis block
- Compile the full node
- Install binaries

> ⏱️ Build takes 10–30 minutes on free tier servers.

---

## ⛏️ PHASE 3 — Launch Genesis Block & Start Mining

```bash
# Edit your config first!
nano ~/.manacoin/manacoin.conf
# Change rpcpassword to something strong

# Start the node
manacoind -daemon

# Wait 10 seconds, then check it's running
manacoin-cli getinfo

# Generate your first wallet address
manacoin-cli getnewaddress
# → Save this MNC address — it's yours!

# Mine your first block (regtest/solo)
manacoin-cli generate 1
```

---

## 🏊 PHASE 4 — Set Up Public Mining Pool

This lets **anyone in the world** mine MNC automatically.

```bash
# Install Node.js
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs

# Install Redis (required by NOMP)
sudo apt-get install -y redis-server
sudo systemctl start redis

# Clone NOMP (pool software)
git clone https://github.com/zone117x/node-open-mining-portal.git nomp
cd nomp
npm install

# Copy ManaCoin config
mkdir -p pool_configs
cp ~/ManaCoin/pool/pool_config.json pool_configs/manacoin.json

# Edit config — add your MNC wallet address
nano pool_configs/manacoin.json
# Replace "YOUR_POOL_MNC_ADDRESS" with your MNC address from Phase 3

# Start the pool
node init.js
```

> 🌐 Your mining pool will be live at: `http://YOUR_SERVER_IP:8888`

### Share with miners:
```
Pool:     stratum+tcp://YOUR_SERVER_IP:3333
Worker:   YOUR_MNC_ADDRESS.worker1
Password: x
```

---

## 📡 PHASE 5 — Add More Nodes (Network Stability)

Run a second node on another free server:

```bash
# On second server, add to manacoin.conf:
addnode=FIRST_SERVER_IP:9888
```

Share your node IP publicly so others can connect.

---

## 📈 PHASE 6 — List on DEX (Free, Instant)

### To list on PancakeSwap (BNB Chain):

Since your coin is its own blockchain, you need a **bridge token** on BNB Chain:

1. Go to https://remix.ethereum.org
2. Create a BEP-20 wrapped token:
```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;
import "@openzeppelin/contracts/token/ERC20/ERC20.sol";

contract WrappedManaCoin is ERC20 {
    address public owner;
    constructor() ERC20("Wrapped ManaCoin", "wMNC") {
        owner = msg.sender;
    }
    function mint(address to, uint256 amount) external {
        require(msg.sender == owner);
        _mint(to, amount);
    }
}
```
3. Deploy on BNB Chain (costs ~$0.20 in BNB gas)
4. Go to https://pancakeswap.finance/add-liquidity
5. Add wMNC + BNB liquidity (even $20 starts a market)
6. Anyone can now trade MNC on PancakeSwap!

---

## 📋 PHASE 7 — Get Listed on CoinGecko & CMC (Free)

### CoinGecko:
1. Go to https://www.coingecko.com/en/coins/new
2. Fill out the form:
   - Coin Name: ManaCoin
   - Ticker: MNC
   - Website: (your landing page)
   - GitHub: (your fork repo)
   - Explorer: (link to your block explorer)
3. Submit — review takes 2–4 weeks

### CoinMarketCap:
1. Go to https://coinmarketcap.com/request/
2. Select "Add cryptocurrency"
3. Fill out details
4. Submit — review takes 4–8 weeks

---

## 🔍 PHASE 8 — Set Up Block Explorer (Free)

Use **Iquidus Explorer** (open source):

```bash
git clone https://github.com/iquidus/explorer.git mnc-explorer
cd mnc-explorer
npm install

# Configure for ManaCoin
cp settings.json.template settings.json
nano settings.json
# Set: coin: "ManaCoin", symbol: "MNC", port: 9889
# Set: rpcuser, rpcpassword to match manacoin.conf

npm start
```

> Block explorer runs at: `http://YOUR_SERVER_IP:3001`

---

## ✅ Launch Checklist

- [ ] Server set up (Oracle/AWS free tier)
- [ ] ManaCoin node built & running
- [ ] Genesis block mined
- [ ] `manacoin.conf` configured with strong password
- [ ] Mining pool (NOMP) running
- [ ] Pool address shared publicly
- [ ] Second node running (network stability)
- [ ] Wrapped token deployed on BNB Chain
- [ ] PancakeSwap liquidity added
- [ ] CoinGecko listing submitted
- [ ] CoinMarketCap listing submitted
- [ ] Block explorer running
- [ ] Website/landing page live
- [ ] Telegram/Discord community created

---

## 🆘 Common Commands

```bash
# Check node status
manacoin-cli getinfo

# Check block count
manacoin-cli getblockcount

# Check connected peers
manacoin-cli getpeerinfo

# Get new address
manacoin-cli getnewaddress

# Check wallet balance
manacoin-cli getbalance

# Stop node
manacoin-cli stop

# Restart node
manacoind -daemon
```

---

## 💬 Need Help?

- Litecoin forums: https://litecointalk.io (ManaCoin is based on Litecoin)
- Bitcointalk ANN: Post your coin launch at https://bitcointalk.org/index.php?board=159.0
- GitHub: https://github.com/litecoin-project/litecoin (base code reference)

---

*ManaCoin — Built for the community, by the community.*
