// Copyright (c) 2025 ManaCoin Developers
// Distributed under the MIT software license.
// ManaCoin (MNC) - Based on Litecoin Core

#include <chainparams.h>
#include <consensus/merkle.h>
#include <arith_uint256.h>
#include <chainparamsseeds.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <versionbitsinfo.h>
#include <assert.h>

// ============================================================
//  MANACOIN GENESIS BLOCK GENERATOR (run once at launch)
// ============================================================
static CBlock CreateGenesisBlock(const char* pszTimestamp,
    const CScript& genesisOutputScript,
    uint32_t nTime, uint32_t nNonce,
    uint32_t nBits, int32_t nVersion,
    const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript()
        << 486604799
        << CScriptNum(4)
        << std::vector<unsigned char>(
               (const unsigned char*)pszTimestamp,
               (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateManaCoinGenesisBlock(uint32_t nTime, uint32_t nNonce,
    uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    // -------------------------------------------------------
    //  Genesis timestamp — choose a meaningful date/headline
    // -------------------------------------------------------
    const char* pszTimestamp =
        "ManaCoin MNC launched 2025 - Power to every holder";

    const CScript genesisOutputScript = CScript()
        << ParseHex(
            "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f6"
            "1deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702"
            "b6bf11d5f")
        << OP_CHECKSIG;

    return CreateGenesisBlock(pszTimestamp, genesisOutputScript,
                              nTime, nNonce, nBits, nVersion, genesisReward);
}

// ============================================================
//  MAIN NETWORK PARAMETERS
// ============================================================
class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";

        // --------------------------------------------------
        //  CONSENSUS RULES
        // --------------------------------------------------
        consensus.nSubsidyHalvingInterval      = 840000;  // halve every 840k blocks
        consensus.BIP34Height                  = 1;
        consensus.BIP34Hash                    = uint256();
        consensus.BIP65Height                  = 1;
        consensus.BIP66Height                  = 1;
        consensus.powLimit                     =
            uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan           = 3.5 * 24 * 60 * 60; // 3.5 days
        consensus.nPowTargetSpacing            = 2.5 * 60;            // 2.5 minutes
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting            = false;
        consensus.nRuleChangeActivationThreshold = 6048; // 75% of 8064
        consensus.nMinerConfirmationWindow       = 8064;

        // --------------------------------------------------
        //  GENESIS BLOCK
        //  Run genesis_generator.py to compute nNonce & hashes
        //  then replace values below.
        // --------------------------------------------------
        genesis = CreateManaCoinGenesisBlock(
            1735689600,   // nTime  → 2025-01-01 00:00:00 UTC
            2084524493,   // nNonce → UPDATE after running genesis_generator.py
            0x1e0ffff0,   // nBits
            1,            // nVersion
            50 * COIN     // genesisReward (not spendable)
        );

        consensus.hashGenesisBlock = genesis.GetHash();

        // TODO: Replace with actual computed hashes after genesis generation
        // assert(consensus.hashGenesisBlock ==
        //     uint256S("0x<YOUR_GENESIS_HASH>"));
        // assert(genesis.hashMerkleRoot ==
        //     uint256S("0x<YOUR_MERKLE_ROOT>"));

        // --------------------------------------------------
        //  NETWORK
        // --------------------------------------------------
        vSeeds.clear(); // Add your DNS seed nodes here after launch
        // vSeeds.emplace_back("seed1.manacoin.org");
        // vSeeds.emplace_back("seed2.manacoin.org");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 50);  // M
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 5);
        base58Prefixes[SCRIPT_ADDRESS2]= std::vector<unsigned char>(1, 50);
        base58Prefixes[SECRET_KEY]     = std::vector<unsigned char>(1, 178);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "mnc";  // ManaCoin bech32 prefix

        // --------------------------------------------------
        //  PORTS
        // --------------------------------------------------
        nDefaultPort = 9888;   // P2P port
        // RPC port configured in manacoin.conf → rpcport=9889

        fDefaultConsistencyChecks = false;
        fRequireStandard          = true;
        m_is_test_chain           = false;
        m_is_mockable_chain       = false;

        // Checkpoints — add after blocks are mined
        checkpointData = {
            {
                // { 10000, uint256S("0x<hash_at_block_10000>") },
            }
        };

        chainTxData = ChainTxData{
            0,  // nTime of last known checkpoint
            0,  // nTxCount
            0   // dTxRate
        };
    }
};

// ============================================================
//  TEST NETWORK PARAMETERS
// ============================================================
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval      = 840000;
        consensus.BIP34Height                  = 1;
        consensus.BIP65Height                  = 1;
        consensus.BIP66Height                  = 1;
        consensus.powLimit                     =
            uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan           = 3.5 * 24 * 60 * 60;
        consensus.nPowTargetSpacing            = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting            = false;
        consensus.nRuleChangeActivationThreshold = 1512;
        consensus.nMinerConfirmationWindow       = 2016;

        genesis = CreateManaCoinGenesisBlock(
            1735689600, 385274206, 0x1e0ffff0, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        nDefaultPort    = 19888;
        bech32_hrp      = "tmnc";
        fDefaultConsistencyChecks = false;
        fRequireStandard          = false;
        m_is_test_chain           = true;
        m_is_mockable_chain       = false;

        checkpointData  = {};
        chainTxData     = ChainTxData{0, 0, 0};
    }
};

// ============================================================
//  REGRESSION TEST PARAMETERS
// ============================================================
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval      = 150;
        consensus.BIP34Height                  = 1;
        consensus.BIP65Height                  = 1;
        consensus.BIP66Height                  = 1;
        consensus.powLimit                     =
            uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan           = 3.5 * 24 * 60 * 60;
        consensus.nPowTargetSpacing            = 2.5 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting            = true;
        consensus.nRuleChangeActivationThreshold = 108;
        consensus.nMinerConfirmationWindow       = 144;

        genesis = CreateManaCoinGenesisBlock(
            1296688602, 0, 0x207fffff, 1, 50 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        nDefaultPort    = 19444;
        bech32_hrp      = "rmnc";
        fDefaultConsistencyChecks = true;
        fRequireStandard          = true;
        m_is_test_chain           = true;
        m_is_mockable_chain       = true;

        checkpointData  = {};
        chainTxData     = ChainTxData{0, 0, 0};
    }
};

// ============================================================
//  GLOBAL FACTORY
// ============================================================
static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams& Params() {
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain) {
    if (chain == CBaseChainParams::MAIN)
        return std::make_unique<CMainParams>();
    else if (chain == CBaseChainParams::TESTNET)
        return std::make_unique<CTestNetParams>();
    else if (chain == CBaseChainParams::REGTEST)
        return std::make_unique<CRegTestParams>();
    throw std::runtime_error(
        strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network) {
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}
