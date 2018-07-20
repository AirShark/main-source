// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2018 Hav0k, Renesis Group
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 800;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (   0,     uint256("0x000009ad2e5d9ec106c7dac2ab5c9e02e880b350e176d3ad1483a9c2c11803f6") ) // genblock
        (   1,     uint256("0x6f86d1371c931f095336f16e3c5a081e6c3ccbdb442489445676a4259fcbee18") ) // first pow
        (  10,     uint256("0x113920ca373a8179facadb54b15ea9a19962166b2f4bd36bf4e456263e9eb828") ) // tenth
        (  50,     uint256("0xf349a0e30d18a399c8234d13b9be7cfe90adb7b61fa8f85307fbd5c7e84c629a") ) // midway premine
        ( 100,     uint256("0x74e20d29c46638f57e930e3a02b78a2839768cf8294e3fc6380775080a34adcf") ) // last premine
        ;

    // TestNet checkpoints
    static MapCheckpoints mapCheckpointsTestnet =
        boost::assign::map_list_of
        (  0,     uint256("0x0000039dcdca6e1e20bc4b889a37821c4869c536d904a6fc532f00aa25f1e7ac") ) // genblock
        ;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
