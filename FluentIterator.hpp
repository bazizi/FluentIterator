#include <functional>
#include <vector>
#include <sstream>

template <typename StlContainerT>
class FluentIteratorBase
{
    using ContainerBaseT = FluentIteratorBase<StlContainerT>;
    using StlContainerItemT = typename StlContainerT::value_type;

protected:
    // Avoids copying objects by using iterators
    std::vector<typename StlContainerT::iterator> mMatchingIterators;

    //  Only used when map is called at least once, and we can't re-use iterators to the original container because new objects have to be created
    StlContainerT mMatchingObjects;

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

    void refreshIterators()
    {
        FluentIteratorBaseT::mMatchingIterators.clear();
        for (auto iter = FluentIteratorBaseT::mMatchingObjects.begin(); iter != FluentIteratorBaseT::mMatchingObjects.end(); ++iter)
        {
            FluentIteratorBaseT::mMatchingIterators.emplace_back(iter);
        }
    }

    FluentIterator &filter(std::function<bool(StlContainerItemT const &)> &&predicate) override
    {
        if (FluentIteratorBaseT::mMatchingObjects.empty())
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

        // Some objects were previously mapped, so we have to filter those
        StlContainerT matchingObjects;
        for (auto iter = FluentIteratorBaseT::mMatchingObjects.begin(); iter != FluentIteratorBaseT::mMatchingObjects.end(); ++iter)
        {
            if (predicate(*iter))
            {
                matchingObjects.emplace_back(std::move(*iter));
            }
        }

        FluentIteratorBaseT::mMatchingObjects = std::move(matchingObjects);
        refreshIterators();
        return *this;
    }

    FluentIterator &map(std::function<StlContainerItemT(StlContainerItemT const &)> &&predicate) override
    {
        StlContainerT matchingObjects;
        for (auto iter = FluentIteratorBaseT::mMatchingIterators.begin(); iter != FluentIteratorBaseT::mMatchingIterators.end(); ++iter)
        {
            matchingObjects.emplace_back(predicate(**iter));
        }
        FluentIteratorBaseT::mMatchingObjects = std::move(matchingObjects);
        refreshIterators();
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
