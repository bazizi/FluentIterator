#include <functional>
#include <vector>
#include <sstream>

template <typename StlContainerT>
class FluentIteratorBase
{
    using ContainerBaseT = FluentIteratorBase<StlContainerT>;
    using StlContainerItemT = typename StlContainerT::value_type;

protected:
    std::vector<typename StlContainerT::iterator> mMatchingIterators;

public:
    virtual ContainerBaseT &filter(std::function<bool(StlContainerItemT const &)> &&) = 0;
    virtual ContainerBaseT &map(std::function<StlContainerItemT(StlContainerItemT const &)> &&) = 0;

    virtual StlContainerT collect() const = 0;
    virtual std::string to_string() const = 0;
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
        // No objects were previously mapped, so we have to filter the iterators
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
        StlContainerT matchingObjects;
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            **iter = std::move(predicate(**iter));
        }

        return *this;
    }

    StlContainerT collect() const override
    {
        std::vector<StlContainerItemT> result;
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            result.emplace_back(std::move(**iter));
        }
        return StlContainerT(result.begin(), result.end());
    }

    std::string to_string() const override
    {
        std::stringstream ss;
        for (auto const &item : collect())
        {
            ss << item << std::endl;
        }
        return ss.str();
    }
};
