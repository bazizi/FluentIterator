#include <functional>
#include <iostream>
#include <vector>
#include <sstream>

template <typename StlContainerT>
class FluentIteratorBase
{
    using FluentIteratorBaseT = FluentIteratorBase<StlContainerT>;
    using StlContainerItemT = typename StlContainerT::value_type;

protected:
    std::vector<typename StlContainerT::iterator> mMatchingIterators;

public:
    virtual FluentIteratorBaseT &filter(std::function<bool(StlContainerItemT const &)> &&) = 0;
    virtual FluentIteratorBaseT &map(std::function<StlContainerItemT(StlContainerItemT const &)> &&) = 0;

    virtual StlContainerT collect(bool consumeIter = true) = 0;
    virtual std::string to_string(bool consumeIter = true) = 0;
    virtual FluentIteratorBaseT &print() = 0;
};

template <typename StlContainerT>
class FluentIterator : StlContainerT, FluentIteratorBase<StlContainerT>
{
    using StlContainerItemT = typename StlContainerT::value_type;
    using FluentIteratorBaseT = FluentIteratorBase<StlContainerT>;

public:
    template <typename... Args>
    FluentIterator(Args... args) : StlContainerT{std::forward<Args>(args)...}
    {
        for (auto iter = this->begin(); iter != this->end(); ++iter)
        {
            FluentIteratorBaseT::mMatchingIterators.emplace_back(iter);
        }
    }

    FluentIterator &filter(std::function<bool(StlContainerItemT const &)> &&predicate) override
    {
        decltype(FluentIteratorBaseT::mMatchingIterators) matchingIterators;
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            if (predicate(**iter))
            {
                matchingIterators.emplace_back(std::move(*iter));
            }
        }

        FluentIteratorBaseT::mMatchingIterators = std::move(matchingIterators);
        return *this;
    }

    FluentIterator &map(std::function<StlContainerItemT(StlContainerItemT const &)> &&predicate) override
    {
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            **iter = std::move(predicate(**iter));
        }

        return *this;
    }

    StlContainerT collect(bool consumeIter = true) override
    {
        std::vector<StlContainerItemT> result;
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            if (consumeIter)
            {
                result.emplace_back(std::move(**iter));
            }
            else
            {
                result.emplace_back(**iter);
            }
        }
        return StlContainerT(result.begin(), result.end());
    }

    std::string to_string(bool consumeIter = true) override
    {
        std::stringstream ss;
        bool firstIter = true;
        for (auto const &item : collect(consumeIter))
        {
            if (!firstIter)
            {
                ss << ", ";
            }
            ss << item;
            firstIter = false;
        }
        ss << std::endl;
        return ss.str();
    }

    FluentIterator &print() override
    {
        std::cout << this->to_string(false /* consumeIter */);
        return *this;
    }
};
