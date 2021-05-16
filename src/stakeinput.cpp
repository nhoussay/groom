// Copyright (c) 2017-2020 The groom developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#include "stakeinput.h"

#include "chain.h"
#include "txdb.h"
#include "wallet/wallet.h"

CPivStake* CPivStake::NewPivStake(const CTxIn& txin)
{
    if (txin.IsZerocoinSpend()) {
        error("%s: unable to initialize CPivStake from zerocoin spend", __func__);
        return nullptr;
    }

    // Find the previous transaction in database
    uint256 hashBlock;
    CTransactionRef txPrev;
    if (!GetTransaction(txin.prevout.hash, txPrev, hashBlock, true)) {
        error("%s : INFO: read txPrev failed, tx id prev: %s", __func__, txin.prevout.hash.GetHex());
        return nullptr;
    }

    const CBlockIndex* pindexFrom = nullptr;
    // Find the index of the block of the previous transaction
    if (mapBlockIndex.count(hashBlock)) {
        CBlockIndex* pindex = mapBlockIndex.at(hashBlock);
        if (chainActive.Contains(pindex)) pindexFrom = pindex;
    }
    // Check that the input is in the active chain
    if (!pindexFrom) {
        error("%s : Failed to find the block index for stake origin", __func__);
        return nullptr;
    }

    return new CPivStake(txPrev->vout[txin.prevout.n],
                         txin.prevout,
                         pindexFrom);
}

bool CPivStake::GetTxOutFrom(CTxOut& out) const
{
    out = outputFrom;
    return true;
}

CTxIn CPivStake::GetTxIn() const
{
    return CTxIn(outpointFrom.hash, outpointFrom.n);
}

CAmount CPivStake::GetValue() const
{
    return outputFrom.nValue;
}

bool CPivStake::CreateTxOuts(const CWallet* pwallet, std::vector<CTxOut>& vout, CAmount nTotal) const
{
    std::vector<valtype> vSolutions;
    txnouttype whichType;
    CScript scriptPubKeyKernel = outputFrom.scriptPubKey;
    if (!Solver(scriptPubKeyKernel, whichType, vSolutions))
        return error("%s: failed to parse kernel", __func__);

    if (whichType != TX_PUBKEY && whichType != TX_PUBKEYHASH && whichType != TX_COLDSTAKE)
        return error("%s: type=%d (%s) not supported for scriptPubKeyKernel", __func__, whichType, GetTxnOutputType(whichType));

    CKey key;
    if (whichType == TX_PUBKEYHASH || whichType == TX_COLDSTAKE) {
        // if P2PKH or P2CS check that we have the input private key
        if (!pwallet->GetKey(CKeyID(uint160(vSolutions[0])), key))
            return error("%s: Unable to get staking private key", __func__);
    }

    vout.emplace_back(0, scriptPubKeyKernel);

    // Calculate if we need to split the output
    if (pwallet->nStakeSplitThreshold > 0) {
        int nSplit = static_cast<int>(nTotal / pwallet->nStakeSplitThreshold);
        if (nSplit > 1) {
            // if nTotal is twice or more of the threshold; create more outputs
            int txSizeMax = MAX_STANDARD_TX_SIZE >> 11; // limit splits to <10% of the max TX size (/2048)
            if (nSplit > txSizeMax)
                nSplit = txSizeMax;
            for (int i = nSplit; i > 1; i--) {
                LogPrintf("%s: StakeSplit: nTotal = %d; adding output %d of %d\n", __func__, nTotal, (nSplit-i)+2, nSplit);
                vout.emplace_back(0, scriptPubKeyKernel);
            }
        }
    }

    return true;
}

CDataStream CPivStake::GetUniqueness() const
{
    //The unique identifier for a PIV stake is the outpoint
    CDataStream ss(SER_NETWORK, 0);
    ss << outpointFrom.n << outpointFrom.hash;
    return ss;
}

//The block that the UTXO was added to the chain
const CBlockIndex* CPivStake::GetIndexFrom() const
{
    // Sanity check, pindexFrom is set on the constructor.
    if (!pindexFrom) throw std::runtime_error("CPivStake: uninitialized pindexFrom");
    return pindexFrom;
}

// Verify stake contextual checks
bool CPivStake::ContextCheck(int nHeight, uint32_t nTime)
{
    const Consensus::Params& consensus = Params().GetConsensus();
    // Get Stake input block time/height
    const CBlockIndex* pindexFrom = GetIndexFrom();
    if (!pindexFrom)
        return error("%s: unable to get previous index for stake input", __func__);
    const int nHeightBlockFrom = pindexFrom->nHeight;
    const uint32_t nTimeBlockFrom = pindexFrom->nTime;

    // Check that the stake has the required depth/age
    if (consensus.NetworkUpgradeActive(nHeight + 1, Consensus::UPGRADE_ZC_PUBLIC) &&
            !consensus.HasStakeMinAgeOrDepth(nHeight, nTime, nHeightBlockFrom, nTimeBlockFrom))
        return error("%s : min age violation - height=%d - time=%d, nHeightBlockFrom=%d, nTimeBlockFrom=%d",
                         __func__, nHeight, nTime, nHeightBlockFrom, nTimeBlockFrom);
    // All good
    return true;
}

