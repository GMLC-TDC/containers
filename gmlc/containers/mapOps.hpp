/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2022, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#pragma once

#include <optional>

#include <map>
#include <unordered_map>

template<class X1, class X2>
inline X2 mapFind(const std::map<X1, X2>& mapS, const X1& val, const X2& defVal)
{
    auto map_it = mapS.find(val);
    return (map_it != mapS.end()) ? map_it->second : defVal;
}

template<class X1, class X2>
std::optional<X2> mapFind(const std::map<X1, X2>& mapS, const X1& val)
{
    auto map_it = mapS.find(val);
    if (map_it != mapS.end()) {
        return map_it->second;
    }
    return std::nullopt;
}

template<class X1, class X2>
inline X2 mapFind(
    const std::unordered_map<X1, X2>& mapS,
    const X1& val,
    const X2& defVal)
{
    auto map_it = mapS.find(val);
    return (map_it != mapS.end()) ? map_it->second : defVal;
}

template<class X1, class X2>
std::optional<X2> mapFind(const std::unordered_map<X1, X2>& mapS, const X1& val)
{
    auto map_it = mapS.find(val);
    if (map_it != mapS.end()) {
        return map_it->second;
    }
    return std::nullopt;
}
