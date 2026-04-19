/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once
#include "MapTraits.hpp"
#include <optional>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gmlc::containers {
/** class merging a vector of pointer with a map that can be used to lookup
 * specific values
 */
template<class VType, class searchType1, class searchType2>
class DualMappedPointerVector {
    static_assert(
        !std::is_same_v<searchType1, searchType2>,
        "searchType1 and searchType2 cannot be the same type");

  public:
    DualMappedPointerVector() = default;
    DualMappedPointerVector(DualMappedPointerVector&& mp) noexcept = default;
    DualMappedPointerVector&
        operator=(DualMappedPointerVector&& mp) noexcept = default;
    DualMappedPointerVector(const DualMappedPointerVector& mp) = delete;
    DualMappedPointerVector&
        operator=(const DualMappedPointerVector& mp) = delete;
    ~DualMappedPointerVector() = default;
    /** insert a new element into the vector directly from an existing
     * unique ptr*/
    std::optional<size_t> insert(
        const searchType1& searchValue1,
        const searchType2& searchValue2,
        std::unique_ptr<VType>&& ptr)
    {
        auto fnd1 = lookup1.find(searchValue1);
        auto fnd2 = lookup2.find(searchValue2);
        if (fnd1 != lookup1.end() || fnd2 != lookup2.end()) {
            return std::nullopt;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(std::move(ptr));
        lookup1.emplace(searchValue1, index);
        lookup2.emplace(searchValue2, index);
        return index;
    }
    /** insert a new element into the vector*/
    template<typename... Us>
    std::optional<size_t> insert(
        const searchType1& searchValue1,
        const searchType2& searchValue2,
        Us&&... data)
    {
        auto fnd1 = lookup1.find(searchValue1);
        auto fnd2 = lookup2.find(searchValue2);
        if (fnd1 != lookup1.end() || fnd2 != lookup2.end()) {
            return std::nullopt;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup1.emplace(searchValue1, index);
        lookup2.emplace(searchValue2, index);
        return index;
    }

    /** insert a new element into the vector*/
    template<typename... Us>
    std::optional<size_t> insert(
        const searchType1& searchValue1,
        std::nullptr_t /*unused*/,
        Us&&... data)
    {
        auto fnd = lookup1.find(searchValue1);
        if (fnd != lookup1.end()) {
            return std::nullopt;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup1.emplace(searchValue1, index);
        return index;
    }

    /** insert a new element into the vector*/
    template<typename... Us>
    std::optional<size_t> insert(
        no_search_type /*unused*/,
        const searchType2& searchValue2,
        Us&&... data)
    {
        auto fnd = lookup2.find(searchValue2);
        if (fnd != lookup2.end()) {
            return std::nullopt;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup2.emplace(searchValue2, index);
        return index;
    }

    /** insert a new element into the vector directly from an existing
     * unique ptr*/
    size_t insert_or_assign(
        const searchType1& searchValue1,
        const searchType2& searchValue2,
        std::unique_ptr<VType>&& ptr)
    {
        auto fnd1 = lookup1.find(searchValue1);
        auto fnd2 = lookup2.find(searchValue2);
        if (fnd1 != lookup1.end() && fnd2 != lookup2.end()) {
            if (fnd1->second != fnd2->second) {
                throw std::invalid_argument(
                    "search keys already refer to different entries");
            }
            dataStorage[fnd1->second] = std::move(ptr);
            return fnd1->second;
        }
        if (fnd1 != lookup1.end()) {
            dataStorage[fnd1->second] = std::move(ptr);
            removeIndexTerms(lookup2, fnd1->second);
            lookup2[searchValue2] = fnd1->second;
            return fnd1->second;
        }
        if (fnd2 != lookup2.end()) {
            dataStorage[fnd2->second] = std::move(ptr);
            removeIndexTerms(lookup1, fnd2->second);
            lookup1[searchValue1] = fnd2->second;
            return fnd2->second;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(std::move(ptr));
        lookup1[searchValue1] = index;
        lookup2[searchValue2] = index;
        return index;
    }
    /** insert a new element into the vector*/
    template<typename... Us>
    size_t insert_or_assign(
        const searchType1& searchValue1,
        const searchType2& searchValue2,
        Us&&... data)
    {
        auto fnd1 = lookup1.find(searchValue1);
        auto fnd2 = lookup2.find(searchValue2);
        if (fnd1 != lookup1.end() && fnd2 != lookup2.end()) {
            if (fnd1->second != fnd2->second) {
                throw std::invalid_argument(
                    "search keys already refer to different entries");
            }
            dataStorage[fnd1->second] =
                std::make_unique<VType>(std::forward<Us>(data)...);
            return fnd1->second;
        }
        if (fnd1 != lookup1.end()) {
            dataStorage[fnd1->second] =
                std::make_unique<VType>(std::forward<Us>(data)...);
            removeIndexTerms(lookup2, fnd1->second);
            lookup2[searchValue2] = fnd1->second;
            return fnd1->second;
        }
        if (fnd2 != lookup2.end()) {
            dataStorage[fnd2->second] =
                std::make_unique<VType>(std::forward<Us>(data)...);
            removeIndexTerms(lookup1, fnd2->second);
            lookup1[searchValue1] = fnd2->second;
            return fnd2->second;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup1[searchValue1] = index;
        lookup2[searchValue2] = index;
        return index;
    }

    /** insert a new element into the vector*/
    template<typename... Us>
    size_t insert_or_assign(
        const searchType1& searchValue1,
        std::nullptr_t /*unused*/,
        Us&&... data)
    {
        auto fnd = lookup1.find(searchValue1);
        if (fnd != lookup1.end()) {
            dataStorage[fnd->second] =
                std::make_unique<VType>(std::forward<Us>(data)...);
            return fnd->second;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup1.emplace(searchValue1, index);
        return index;
    }

    /** insert a new element into the vector*/
    template<typename... Us>
    size_t insert_or_assign(
        std::nullptr_t /*unused*/,
        const searchType2& searchValue2,
        Us&&... data)
    {
        auto fnd = lookup2.find(searchValue2);
        if (fnd != lookup2.end()) {
            dataStorage[fnd->second] =
                std::make_unique<VType>(std::forward<Us>(data)...);
            return fnd->second;
        }
        auto index = dataStorage.size();
        dataStorage.emplace_back(
            std::make_unique<VType>(std::forward<Us>(data)...));
        lookup2.emplace(searchValue2, index);
        return index;
    }

    /** find an element based on the search value
@return nullptr if the element is not found
*/
    VType* find(const searchType1& searchValue1) const
    {
        auto fnd = lookup1.find(searchValue1);
        if (fnd != lookup1.end()) {
            return dataStorage[fnd->second].get();
        }
        return nullptr;
    }

    /** find an element based on the search value
@return nullptr if the element is not found
*/
    VType* find(const searchType2& searchValue2) const
    {
        auto fnd = lookup2.find(searchValue2);
        if (fnd != lookup2.end()) {
            return dataStorage[fnd->second].get();
        }
        return nullptr;
    }

    VType* operator[](size_t index) const
    {
        return (index < dataStorage.size()) ? (dataStorage[index].get()) :
                                              nullptr;
    }

    VType* at(size_t index) const { return dataStorage.at(index).get(); }

    /** get a pointer to the last element inserted*/
    VType* back() { return dataStorage.back().get(); }
    /** remove an element at a specific index
@param index the index of the element to remove*/
    void removeIndex(size_t index)
    {
        if (index >= dataStorage.size()) {
            return;
        }
        dataStorage.erase(dataStorage.begin() + index);
        updateLookupAfterErase(lookup1, index);
        updateLookupAfterErase(lookup2, index);
    }

    void remove(const searchType1& search)
    {
        auto el = lookup1.find(search);
        if (el == lookup1.end()) {
            return;
        }
        auto index = el->second;
        dataStorage.erase(dataStorage.begin() + index);
        updateLookupAfterErase(lookup1, index);
        updateLookupAfterErase(lookup2, index);
    }

    void remove(const searchType2& search)
    {
        auto el = lookup2.find(search);
        if (el == lookup2.end()) {
            return;
        }
        auto index = el->second;
        dataStorage.erase(dataStorage.begin() + index);
        updateLookupAfterErase(lookup1, index);
        updateLookupAfterErase(lookup2, index);
    }

    /** apply a function to all the values
@param F must be a function with signature like void fun(VType *a);*/
    template<class UnaryFunction>
    void apply(UnaryFunction F)
    {
        for (auto& vp : dataStorage) {
            F(vp.get());
        }
    }

    /** apply a function to all the values
@param F must be a function with signature like void fun(const VType *a);*/
    template<class UnaryFunction>
    void apply(UnaryFunction F) const
    {
        for (auto& vp : dataStorage) {
            F(vp.get());
        }
    }
    /** get a const iterator to the start of the data*/
    auto begin() const { return dataStorage.cbegin(); }
    /** get a constant iterator to the end of the data*/
    auto end() const { return dataStorage.cend(); }
    /** get the number of elements in the data*/
    auto size() const { return dataStorage.size(); }
    /** remove all elements from the data*/
    void clear()
    {
        dataStorage.clear();
        lookup1.clear();
        lookup2.clear();
    }

  private:
    template<class SearchMap>
    static void updateLookupAfterErase(SearchMap& searchMap, size_t index)
    {
        for (auto it = searchMap.begin(); it != searchMap.end();) {
            if (it->second == index) {
                it = searchMap.erase(it);
            } else {
                if (it->second > index) {
                    --(it->second);
                }
                ++it;
            }
        }
    }

    template<class SearchMap>
    static void removeIndexTerms(SearchMap& searchMap, size_t index)
    {
        for (auto it = searchMap.begin(); it != searchMap.end();) {
            if (it->second == index) {
                it = searchMap.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<std::unique_ptr<VType>> dataStorage;  //!< storage for the
                                                      //!< pointers
    std::conditional_t<
        is_easily_hashable_v<searchType1>,
        std::unordered_map<searchType1, size_t>,
        std::map<searchType1, size_t>>
        lookup1;  //!< map to lookup the index
    std::conditional_t<
        is_easily_hashable_v<searchType2>,
        std::unordered_map<searchType2, size_t>,
        std::map<searchType2, size_t>>
        lookup2;  //!< map to lookup the index
};

}  // namespace gmlc::containers
