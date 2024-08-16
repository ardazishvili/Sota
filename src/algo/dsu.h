#pragma once

#include <map>
#include <vector>

namespace sota::algo {

template <typename T>
class DSU {
 public:
  DSU(int m, int n) : _m(m), _n(n) {
    _parent = std::vector<int>(n * m, -1);
    _size = std::vector<int>(n * m, 0);
    _objects = std::vector<T>(n * m, nullptr);
  }

  void push(int i, T object) {
    _parent[i] = i;
    _size[i] = 1;
    _objects[i] = object;
  }

  int rep(int i) {
    if (_parent[i] == i) {
      return i;
    }
    return _parent[i] = rep(_parent[i]);
  }

  void make_union(int i, int j) {
    if (!(0 <= j && j < _n * _m) || _parent[i] == -1 || _parent[j] == -1) {
      return;
    }
    int a = rep(i);
    int b = rep(j);

    if (a == b) {
      return;
    }
    if (_size[a] < _size[b]) {
      std::swap(a, b);
    }
    _parent[b] = a;
    _size[a] += _size[b];
  }

  std::vector<std::vector<T>> groups() {
    std::map<T, std::vector<T>> m;
    for (int i = 0; i < _n * _m; ++i) {
      if (_parent[i] == -1) {
        continue;
      }
      m[_objects[rep(i)]].push_back(_objects[i]);
    }

    std::vector<std::vector<T>> res;
    for (auto [k, vec] : m) {
      res.push_back(vec);
    }

    return res;
  }

 private:
  std::vector<int> _parent;
  std::vector<T> _objects;
  std::vector<int> _size;
  int _m;
  int _n;
};

}  // namespace sota::algo
