#ifndef MANACOIN_AMOUNT_H
#define MANACOIN_AMOUNT_H

#include <stdint.h>

// ============================================================
//  MANACOIN (MNC) — MONETARY SUPPLY CONSTANTS
// ============================================================

/** The amount of satoshis in one MNC */
static const int64_t COIN = 100000000;   // 1 MNC = 100,000,000 MNC-satoshis

/** The amount of satoshis in 0.01 MNC */
static const int64_t CENT = 1000000;

/** Maximum supply: 84,000,000 MNC (like Litecoin — 4x Bitcoin) */
static const int64_t MAX_MONEY = 84000000LL * COIN;

/** Block reward starts at 50 MNC and halves every 840,000 blocks */
static const int64_t INITIAL_BLOCK_REWARD = 50 * COIN;

/** Halving interval: every 840,000 blocks (~4 years at 2.5 min/block) */
static const int SUBSIDY_HALVING_INTERVAL = 840000;

/**
 * GetBlockSubsidy — returns the block reward at a given height.
 * halvings: 0  → 50.0 MNC
 *           1  → 25.0 MNC  (after block 840,000)
 *           2  → 12.5 MNC  (after block 1,680,000)
 *           ...
 *           33 →  0         (fully mined out)
 */
inline int64_t GetBlockSubsidy(int nHeight, int nHalvingInterval = SUBSIDY_HALVING_INTERVAL)
{
    int halvings = nHeight / nHalvingInterval;
    // Once halvings exceed 63 (never in practice), reward is 0
    if (halvings >= 64) return 0;

    int64_t nSubsidy = INITIAL_BLOCK_REWARD;
    nSubsidy >>= halvings;   // right-shift = divide by 2 for each halving
    return nSubsidy;
}

/** Sanity check: result must be within valid money range */
inline bool MoneyRange(const int64_t& nValue) {
    return (nValue >= 0 && nValue <= MAX_MONEY);
}

#endif // MANACOIN_AMOUNT_H
