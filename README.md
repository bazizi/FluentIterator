# Fluent iterators for C++ (WIP)

An experimental project which attempts to implement Rust-like fluent iterator adaptors for C++.


```cpp
FluentIterator<std::vector<int>> v = {1, 2, 3, 4, 5, 6};

auto const result = v.filter([](auto const &item)
                                { return item % 2 == 0; })
                        .filter([](auto const &item)
                                { return item % 3 != 0; })
                        .map([](auto const &item)
                                { return item * 20; })
                        .map([](auto const &item)
                                { return item / 20; })
                        .filter([](auto const &item)
                                { return item % 4 == 0; });
std::cout << result.to_string() << std::endl;

// 4\n
```

