# Fluent iterators for C++ (WIP)

An experimental project which attempts to implement Rust-like fluent iterator adaptors for C++.


```cpp
FluentIterator<std::vector<int>> v = {1, 2, 3, 4, 5, 6};
v = v.map([](auto const &item) { return item * 20; })
     .print() // 20, 40, 60, 80, 100, 120
     .filter([](auto const &item) { return item % 3 != 0; })
     .print() // 20, 40, 80, 100
     .map([](auto const &item) { return item / 4; })
     .print() // 5, 10, 20, 25
     .filter([](auto const &item) { return item % 4 == 0; })
     .print() // 20
     .collect();
```

