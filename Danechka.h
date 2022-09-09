#pragma once

#include <vector>
#include <set>
#include <memory>
#include <utility>
#include <iterator>
#include <iostream>
#include <ios>        // for boolalpha.

#include "LinkedChain.h"

namespace danechka
{

using Data = int;

class _2Chainz
{
private:
    std::vector<std::shared_ptr<LinkedChain<Data>>> chains_;

    inline static const int A = 0;
    inline static const int B = 1;

public:
    _2Chainz(std::size_t length,
             Data initial,
             std::set<std::size_t> linksA,
             std::set<std::size_t> linksB) :
        chains_(2)
    {
        const auto makeChain = [](std::size_t maxLength, std::set<std::size_t> links)
        {
            typename LinkedChain<Data>::Settings set;
            set.maxLength_   = maxLength;
            set.linkIndices_ = std::move(links);

            return std::make_shared<LinkedChain<Data>>(std::move(set));
        };

        chains_[A] = makeChain(length, linksA);
        chains_[B] = makeChain(length, linksB);


        for (auto& chain : chains_)
            for (std::size_t index = 0; index < length; index++)
            {
                const bool isPushed = chain->tryPushData(initial);

                std::cout << std::boolalpha << "pushing initial value: " << initial << ", status: " << isPushed << std::endl;
            }

        if (chains_.size() > 1)
        {
            std::cout << "linking chains!" << std::endl;

            for (std::size_t index = 1; index < chains_.size(); index++)
            {
                LinkedChain<Data>& prev = *chains_[index-1];
                chains_[index]->linkTo(prev);
            }
        }
    }

    int pushA(int value)
    {
        int head = -1;
        if (chains_[A]->tryPopData(head))
        {
            std::cout << std::boolalpha << "A - popped contained value: " << head << std::endl;
            std::cout << std::boolalpha << "A - pushing new value: " << value << ", status: " << chains_[A]->tryPushData(value) << std::endl;

            return head;
        }
        else
        {
            std::cout << "A - can't pop contained value!" << std::endl;
            const bool isPushed = chains_[A]->tryPushData(value);
            std::cout << std::boolalpha << "A - pushing new value: " << value << ", status: " << isPushed << std::endl;

            if (isPushed)
            {
                std::cout << "A - can't push new value!" << std::endl;
                return -1;
            }
            else
            {
                const bool isPopped = chains_[A]->tryPopData(head);
                std::cout << std::boolalpha << "A - popping new value back: " << ", status: " << isPopped << std::endl;

                if (isPopped)
                {
                    std::cout << std::boolalpha << "A - new value popped back: " << head << std::endl;
                    return head;
                }
                else
                {
                    std::cout << "A - can't pop new value back!" << std::endl;
                    return -1;
                }
            }
        }
    }

    int pushB(int value)
    {
        int head = -1;
        if (chains_[B]->tryPopData(head))
        {
            std::cout << "B - popped contained value: " << head << std::endl;
            std::cout << std::boolalpha << "B - pushing new value: " << value << ", status: " << chains_[A]->tryPushData(value) << std::endl;

            return head;
        }
        else
        {
            std::cout << "B - can't pop contained value!" << std::endl;
            const bool isPushed = chains_[B]->tryPushData(value);
            std::cout << std::boolalpha << "B - pushing new value: " << value << ", status: " << isPushed << std::endl;

            if (isPushed)
            {
                std::cout << "B - can't push new value!" << std::endl;
                return -1;
            }
            else
            {
                const bool isPopped = chains_[B]->tryPopData(head);
                std::cout << std::boolalpha << "B - popping new value back: " << ", status: " << isPopped << std::endl;

                if (isPopped)
                {
                    std::cout << "B - new value popped back: " << head << std::endl;
                    return head;
                }
                else
                {
                    std::cout << "B - can't pop new value back!" << std::endl;
                    return -1;
                }
            }
        }
    }
};

}
