/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "gtest/gtest.h"
#include <iostream>

#include "DualMappedVector.hpp"
using namespace gmlc::containers;

/** test basic operations */
TEST (dual_mapped_vector_tests_stable, definition_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
      M;
    DualMappedVector<std::string, std::string, double,
                     reference_stability::stable>
      S2;
    EXPECT_EQ (M.size (), 0u);
    EXPECT_EQ (S2.size (), 0u);
    DualMappedVector<std::vector<std::string>, double, std::string,
                     reference_stability::stable>
      V2;

    // test move and assignment operators
    decltype (M) TV2;
    TV2 = std::move (M);

    decltype (TV2) TV3;
    TV3 = TV2;
}

TEST (dual_mapped_vector_tests_stable, insertion_tests)
{
    DualMappedVector<std::vector<double>, std::string, int64_t,
                     reference_stability::stable>
      Mvec;
    Mvec.insert ("el1", 41, 3, 1.7);
    EXPECT_EQ (Mvec.size (), 1u);
    Mvec.insert ("a2", 27, std::vector<double> (45));
    EXPECT_EQ (Mvec.size (), 2u);
    auto &V = Mvec[0];
    EXPECT_EQ (V.size (), 3u);
    EXPECT_EQ (V[0], 1.7);
    EXPECT_EQ (V[2], 1.7);

    auto &V2 = Mvec[1];
    EXPECT_EQ (V2.size (), 45u);

    auto V3 = Mvec.find ("el1");
    EXPECT_EQ (V3->size (), 3u);

    auto V4 = Mvec.find ("a2");
    EXPECT_EQ (V4->size (), 45u);

    auto V5 = Mvec.find (41);
    EXPECT_EQ (V5->size (), 3u);
}

TEST (dual_mapped_vector_tests_stable, additional_searchTerm_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
      Mvec;

    Mvec.insert ("s1", 64, 3.2);
    Mvec.insert ("s2", 63, 4.3);
    Mvec.insert ("s3", 47, 9.7);
    Mvec.insert ("s4", 92, 11.4);

    EXPECT_EQ (Mvec.size (), 4u);

    Mvec.addSearchTerm ("s5", "s1");
    EXPECT_EQ (Mvec.size (), 4u);

    EXPECT_EQ (*(Mvec.find ("s5")), 3.2);

    Mvec.addSearchTerm (93, 47);
    EXPECT_EQ (*(Mvec.find (93)), 9.7);

    Mvec.addSearchTerm (143, "s3");
    EXPECT_EQ (*(Mvec.find (143)), 9.7);

    Mvec.addSearchTerm ("extra", 63);
    EXPECT_EQ (*(Mvec.find ("extra")), 4.3);

    Mvec.addSearchTermForIndex ("astring", 3);
    EXPECT_EQ (*(Mvec.find ("astring")), 11.4);

    auto res = Mvec.addSearchTermForIndex (99, 2);
    EXPECT_EQ (*(Mvec.find (99)), 9.7);
    EXPECT_TRUE (res);

    // check for appropriate return values
    EXPECT_TRUE (!Mvec.addSearchTerm ("missing", "none"));
    EXPECT_TRUE (!Mvec.addSearchTerm (1241, 98));
    EXPECT_TRUE (!Mvec.addSearchTerm ("missing", 98));
    EXPECT_TRUE (!Mvec.addSearchTerm (1241, "none"));
}

TEST (dual_mapped_vector_tests_stable, iterator_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
      Mvec;

    Mvec.insert ("s1", 64, 3.2);
    Mvec.insert ("s2", 63, 4.3);
    Mvec.insert ("s3", 47, 9.7);
    Mvec.insert ("s4", 92, 11.4);

    EXPECT_EQ (Mvec.size (), 4u);

    Mvec.transform ([](double val) { return val + 1; });

    EXPECT_EQ (Mvec[0], 3.2 + 1.0);
    EXPECT_EQ (Mvec[1], 4.3 + 1.0);
    EXPECT_EQ (Mvec[2], 9.7 + 1.0);

    double sum = 0.0;
    for (auto &el : Mvec)
    {
        sum += el;
    }
    EXPECT_EQ (sum, 4.2 + 5.3 + 10.7 + 12.4);
}

TEST (dual_mapped_vector_tests_stable, remove_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
      Mvec;

    Mvec.insert ("s1", 64, 3.2);
    Mvec.insert ("s2", 63, 4.3);
    Mvec.insert ("s3", 47, 9.7);
    Mvec.insert ("s4", 92, 11.4);

    Mvec.addSearchTerm ("s5", 64);
    Mvec.addSearchTermForIndex (107, 2);
    EXPECT_EQ (Mvec.size (), 4u);

    Mvec.removeIndex (1);

    EXPECT_EQ (Mvec.size (), 3u);
    EXPECT_TRUE (Mvec.find ("s2") == Mvec.end ());
    EXPECT_EQ (Mvec[1], 9.7);
    EXPECT_EQ (*Mvec.find ("s4"), 11.4);
    EXPECT_EQ (*Mvec.find ("s5"), 3.2);
    Mvec.remove ("s1");
    EXPECT_EQ (Mvec.size (), 2u);
    EXPECT_EQ (*Mvec.find ("s4"), 11.4);
    EXPECT_EQ (Mvec[0], 9.7);
    EXPECT_TRUE (Mvec.find ("s5") == Mvec.end ());
    EXPECT_EQ (*Mvec.find (107), 9.7);

    auto MV2 = std::move (Mvec);
    EXPECT_EQ (MV2.size (), 2u);

    auto MV3 = MV2;
    EXPECT_EQ (MV2.size (), 2u);
    EXPECT_EQ (MV3.size (), 2u);

    MV3.remove (92);
    EXPECT_EQ (MV2.size (), 2u);
    EXPECT_EQ (MV3.size (), 1u);
    MV3.clear ();
    EXPECT_EQ (MV2.size (), 2u);
    EXPECT_EQ (MV3.size (), 0u);
}