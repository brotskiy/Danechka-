#pragma once

#include <algorithm>
#include <memory>           // provides shared_ptr.
#include <cstdint>          // provides size_t.
#include <iterator>
#include <stdexcept>
#include <utility>

#include <optional>
#include <set>
#include <vector>
#include <unordered_map>    // is a hash-table.
#include <deque>            // is a queue with constant fron/back and log(N) random access [] operations.


// -------------------------------------------------

template <class Data>
class LinkDataStorage
{
private:
    std::optional<Data> data_ = std::nullopt;

public:
    void putData(Data incoming)
    {
        data_ = incoming;
    }

    std::optional<Data> extractData()
    {
        const std::optional<Data> tmp = data_;
        data_ = std::nullopt;

        return tmp;
    }
};

// -------------------------------------------------

template <class Data>
class Link
{
private:
    std::shared_ptr<LinkDataStorage<Data>> storage_ = std::make_shared<LinkDataStorage<Data>>();

public:
    void putData(Data incoming)
    {
        storage_->putData(incoming);
    }

    std::optional<Data> extractData()
    {
        return storage_->extractData();
    }

    // for explicitness. same work is done by copy-constructor.
    void adoptStorageFrom(const Link &other)
    {
        storage_ = other.storage_;
    }
};

// -------------------------------------------------

template <class Data>
class LinkedChain
{
public:
    struct Settings
    {
        std::size_t maxLength_;
        std::set<std::size_t> linkIndices_;
    };

private:
    const Settings set_;

    std::deque<Data> dataQueue_;
    std::unordered_map<std::size_t, Link<Data>> index_to_link_;

public:
    explicit LinkedChain (Settings settings) :
        set_(std::move(settings))
    {
        if (set_.maxLength_ == 0)
            throw std::runtime_error("length == 0!");

        if (set_.linkIndices_.size() > set_.maxLength_)
            throw std::runtime_error("link indices count > length!");

        if (not set_.linkIndices_.empty())
            if(const std::size_t maxIndex = *std::max_element(set_.linkIndices_.cbegin(), set_.linkIndices_.cend()); maxIndex >= set_.maxLength_)
                throw std::runtime_error("max link index must be < length!");

        for (const std::size_t index : set_.linkIndices_)
            index_to_link_.emplace(index, Link<Data>());
    }

    void linkTo(LinkedChain &other)
    {
        const auto extractLinks = [](const LinkedChain &chain) -> std::vector<std::size_t>
        {
            std::vector<std::size_t> indices;
            for (const auto& [index, link] : chain.index_to_link_)
                indices.push_back(index);
            std::sort(indices.begin(), indices.end());

            return indices;    // named return value optimization may occur. otherwise use std::move().
        };

        if (index_to_link_.size() != other.index_to_link_.size())
            throw std::runtime_error("links count missmatch!");

        // first:  ... - 2 - 3 - 4 - 6 - ...
        //               |   |   |   |
        // second: ... - 3   5   8   9 - ...

        const std::vector<std::size_t> firstLinks = extractLinks(*this);
        const std::vector<std::size_t> secondLinks = extractLinks(other);

        for (std::size_t i = 0; i < firstLinks.size(); i++)
        {
            const std::size_t indexFirst  = firstLinks[i];
            const std::size_t indexSecond = secondLinks[i];

            // rewrite local state with the OTHER chain state!
            index_to_link_[indexFirst].adoptStorageFrom(other.index_to_link_[indexSecond]);
        }
    }

    bool tryPushData(Data incoming)
    {
        if (dataQueue_.size() == set_.maxLength_)
            return false;

        // data elements tend to be stored at the imaginary end of the queue.
        // so we update data elements in the links, counting links from the imaginary end of the queue.
        // length:        9;
        // element count: 5
        // links at:      1, 4, 8;
        // marks:         element X, link [], imaginary vacant place _;

        // _ [] _ _ [] X X X [X] X
        // 0 1  2 3 4  5 6 7  8  9

        // 1. try to update local elements with it's state inside links.
        updateState(true);

        // 2. add new element into the queue.
        dataQueue_.push_back(incoming);

        // 3. local elements indices are changed, so we update values inside links.
        updateState(false);

        return true;
    }

    bool tryPopData(Data &outcoming)
    {
        if (dataQueue_.empty())
            return false;

        // 1. try to update local elements with it's state inside links.
        updateState(true);

        // 2. pop the head (the oldest) element.
        const Data head = dataQueue_.front();
        dataQueue_.pop_front();

        // 3. local elements indices are changed, so we update values inside links.
        updateState(false);

        // 4. assign function output parameter.
        outcoming = head;
        return true;
    }

    void clearData()
    {
        dataQueue_.clear();
        for (auto& [index, link] : index_to_link_)
            const std::optional<Data> discarded = link.extractData();
    }

private:
    void updateState(bool local)
    {
        // data elements tend to be stored at the imaginary end of the queue.
        // so we update data elements in the links, counting links from the imaginary end of the queue.
        // length:        9;
        // element count: 5
        // links at:      1, 4, 8;
        // marks:         element X, link [], imaginary vacant place _;

        // _ [] _ _ [] X X X [X] X
        // 0 1  2 3 4  5 6 7  8  9

        // nothing to update, if queue is empty!
        if (dataQueue_.empty())
            return;

        const std::size_t storedElementsCount     = dataQueue_.size();
        const std::size_t firstStoredElementIndex = set_.maxLength_ - storedElementsCount;

        for (auto& [linkIndex, link] : index_to_link_)
            if (linkIndex >= firstStoredElementIndex)
            {
                const std::size_t realStoredElementIndex = linkIndex - firstStoredElementIndex;

                if (local)
                {
                    // if we update local state, then extract element from the link.
                    const std::optional<Data> extractedDataFromLink = link.extractData();

                    // if data is stored inside link, then update appropriate local queue element.
                    if (extractedDataFromLink != std::nullopt)
                        dataQueue_.at(realStoredElementIndex) = extractedDataFromLink.value();
                }
                else
                {
                    // if we update data inside link, then put local element into it.
                    const Data locallyStoredData = dataQueue_.at(realStoredElementIndex);
                    link.putData(locallyStoredData);
                }
            }
    }
};


