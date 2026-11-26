# Standalone C++ smoke test

On an Ubuntu or Debian runner, install the compiler and doctest headers.

```bash
sudo apt install g++ doctest-dev
```

From the repository root, compile and run the test.

```bash
g++ -std=c++20 tests/standalone/arithmetic_test.cpp -o arithmetic-test
./arithmetic-test
```
