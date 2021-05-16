// Copyright (c) 2020 The groom developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://www.opensource.org/licenses/mit-license.php.

#ifndef groom_SAPLING_TEST_FIXTURE_H
#define groom_SAPLING_TEST_FIXTURE_H

#include "test/test_groom.h"

/**
 * Testing setup that configures a complete environment for Sapling testing.
 */
struct SaplingTestingSetup : public TestingSetup {
    SaplingTestingSetup();
    ~SaplingTestingSetup();
};


#endif //groom_SAPLING_TEST_FIXTURE_H
