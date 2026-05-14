#!/usr/bin/env python3
"""
ManaCoin (MNC) — Genesis Block Generator
=========================================
Run this ONCE before launching your coin to compute:
  - The correct nNonce
  - hashGenesisBlock
  - hashMerkleRoot

Usage:
    pip install pycoin  (or: pip3 install pycoin)
    python3 genesis_generator.py

Then copy the output values into chainparams.cpp.
"""

import hashlib
import struct
import time

# ── ManaCoin Genesis Parameters ────────────────────────────────────────────────
TIMESTAMP_MSG  = b"ManaCoin MNC launched 2025 - Power to every holder"
GENESIS_TIME   = 1735689600   # 2025-01-01 00:00:00 UTC  (change if needed)
GENESIS_NBITS  = 0x1e0ffff0   # Starting difficulty
GENESIS_REWARD = 50 * 100_000_000  # 50 MNC in satoshis
GENESIS_VER    = 1

# Public key for coinbase output (standard Litecoin genesis key — safe placeholder)
PUBKEY_HEX = (
    "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb64"
    "9f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f"
)

# ── Helpers ────────────────────────────────────────────────────────────────────

def sha256d(data: bytes) -> bytes:
    """Double SHA-256"""
    return hashlib.sha256(hashlib.sha256(data).digest()).digest()

def uint32(x):
    return x & 0xFFFFFFFF

def pack_varint(n):
    if n < 0xfd:
        return struct.pack("B", n)
    elif n <= 0xffff:
        return b"\xfd" + struct.pack("<H", n)
    elif n <= 0xffffffff:
        return b"\xfe" + struct.pack("<I", n)
    else:
        return b"\xff" + struct.pack("<Q", n)

def script_push(data: bytes) -> bytes:
    n = len(data)
    if n < 0x4c:
        return struct.pack("B", n) + data
    elif n <= 0xff:
        return b"\x4c" + struct.pack("B", n) + data
    else:
        return b"\x4d" + struct.pack("<H", n) + data

def build_coinbase_tx(timestamp_msg: bytes, pubkey_hex: str, reward: int) -> bytes:
    """Build the genesis coinbase transaction."""
    # scriptSig: block height push + nBits push + timestamp
    scriptsig = (
        struct.pack("<I", 486604799)  +  # nBits
        b"\x01\x04"                   +  # push 4 bytes
        script_push(timestamp_msg)
    )

    pubkey   = bytes.fromhex(pubkey_hex)
    scriptpk = script_push(pubkey) + b"\xac"  # OP_CHECKSIG

    tx  = struct.pack("<I", 1)          # version
    tx += pack_varint(1)                # vin count
    tx += b"\x00" * 32                 # prev hash (null)
    tx += struct.pack("<I", 0xFFFFFFFF) # prev index
    tx += pack_varint(len(scriptsig))
    tx += scriptsig
    tx += struct.pack("<I", 0xFFFFFFFF) # sequence
    tx += pack_varint(1)                # vout count
    tx += struct.pack("<q", reward)     # value
    tx += pack_varint(len(scriptpk))
    tx += scriptpk
    tx += struct.pack("<I", 0)          # locktime
    return tx

def merkle_root(tx_hash: bytes) -> bytes:
    """For genesis block, merkle root = single txid."""
    return tx_hash

def build_block_header(ver, prev_hash, merkle, ntime, nbits, nonce) -> bytes:
    return (
        struct.pack("<I", ver)
        + prev_hash
        + merkle
        + struct.pack("<I", ntime)
        + struct.pack("<I", nbits)
        + struct.pack("<I", nonce)
    )

def target_from_nbits(nbits: int) -> int:
    exp   = nbits >> 24
    coeff = nbits & 0xFFFFFF
    return coeff * (256 ** (exp - 3))

# ── Main ───────────────────────────────────────────────────────────────────────

def mine_genesis():
    print("=" * 60)
    print("  ManaCoin (MNC) — Genesis Block Mining")
    print("=" * 60)
    print(f"  Timestamp : {TIMESTAMP_MSG.decode()}")
    print(f"  nTime     : {GENESIS_TIME}")
    print(f"  nBits     : 0x{GENESIS_NBITS:08x}")
    print(f"  Reward    : {GENESIS_REWARD / 1e8:.1f} MNC")
    print("=" * 60)
    print("Mining genesis block... (may take a few minutes)\n")

    coinbase_tx  = build_coinbase_tx(TIMESTAMP_MSG, PUBKEY_HEX, GENESIS_REWARD)
    coinbase_hash = sha256d(coinbase_tx)
    merkle        = merkle_root(coinbase_hash)
    prev_hash     = b"\x00" * 32
    target        = target_from_nbits(GENESIS_NBITS)

    nonce      = 0
    start_time = time.time()

    while nonce <= 0xFFFFFFFF:
        header     = build_block_header(
            GENESIS_VER, prev_hash, merkle,
            GENESIS_TIME, GENESIS_NBITS, nonce)
        block_hash = sha256d(header)
        hash_int   = int.from_bytes(block_hash[::-1], "big")  # little→big endian

        if hash_int < target:
            elapsed = time.time() - start_time
            print(f"✅  Genesis block found in {elapsed:.1f}s!\n")
            print("─" * 60)
            print(f"  nNonce           : {nonce}")
            print(f"  hashGenesisBlock : {block_hash[::-1].hex()}")
            print(f"  hashMerkleRoot   : {merkle[::-1].hex()}")
            print("─" * 60)
            print("\n📋  Copy these into src/chainparams.cpp:\n")
            print(f'    genesis = CreateManaCoinGenesisBlock(')
            print(f'        {GENESIS_TIME},       // nTime')
            print(f'        {nonce},     // nNonce  ← NEW')
            print(f'        0x{GENESIS_NBITS:08x},   // nBits')
            print(f'        {GENESIS_VER},            // nVersion')
            print(f'        50 * COIN     // genesisReward')
            print(f'    );\n')
            print(f'    assert(consensus.hashGenesisBlock ==')
            print(f'        uint256S("0x{block_hash[::-1].hex()}"));\n')
            print(f'    assert(genesis.hashMerkleRoot ==')
            print(f'        uint256S("0x{merkle[::-1].hex()}"));\n')
            return

        if nonce % 100_000 == 0:
            elapsed = time.time() - start_time
            rate    = nonce / elapsed if elapsed > 0 else 0
            print(f"  Tried {nonce:,} nonces... ({rate:,.0f} H/s)", end="\r")

        nonce += 1

    print("❌  Nonce space exhausted — increase nTime by 1 and retry.")

if __name__ == "__main__":
    mine_genesis()
