#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <set>

#include "algo/dsu.h"

TEST_CASE("Merging two disjoint-set pairs leaves the fifth element separate") {
  int elements[] = {0, 1, 2, 3, 4};
  sota::algo::DSU<int *> sets(1, 5);
  for (int i = 0; i < 5; ++i) {
    sets.push(i, &elements[i]);
  }

  sets.make_union(0, 1);
  sets.make_union(2, 3);
  CHECK(sets.rep(0) == sets.rep(1));
  CHECK(sets.rep(2) == sets.rep(3));
  CHECK(sets.rep(0) != sets.rep(2));

  sets.make_union(1, 3);
  const int representative = sets.rep(0);
  CHECK(sets.rep(1) == representative);
  CHECK(sets.rep(2) == representative);
  CHECK(sets.rep(3) == representative);
  CHECK(sets.rep(4) != representative);
}

TEST_CASE("Inserted disjoint-set elements start as singleton sets") {
  int elements[] = {0, 1, 2};
  sota::algo::DSU<int *> sets(2, 2);
  for (int i = 0; i < 3; ++i) {
    sets.push(i, &elements[i]);
    CHECK(sets.rep(i) == i);
  }
  CHECK(sets.groups().size() == 3);
}

TEST_CASE("Repeated unions preserve exact disjoint-set membership") {
  int elements[] = {0, 1, 2, 3, 4};
  sota::algo::DSU<int *> sets(2, 3);
  for (int i = 0; i < 5; ++i) {
    sets.push(i, &elements[i]);
  }

  sets.make_union(0, 1);
  sets.make_union(1, 2);
  sets.make_union(3, 4);
  sets.make_union(2, 0);
  sets.make_union(1, 1);

  const auto groups = sets.groups();
  REQUIRE(groups.size() == 2);
  std::set<std::set<int>> memberships;
  for (const auto& group : groups) {
    std::set<int> members;
    for (const int* element : group) {
      REQUIRE(element != nullptr);
      members.insert(*element);
    }
    CHECK(members.size() == group.size());
    memberships.insert(members);
  }
  const std::set<std::set<int>> expected = {{0, 1, 2}, {3, 4}};
  CHECK(memberships == expected);
}
