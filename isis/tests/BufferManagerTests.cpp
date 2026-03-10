/* SPDX-License-Identifier: CC0-1.0 */
#include <gtest/gtest.h>

#include "BufferManager.h"
#include "Preference.h"

#include <tuple>
#include <vector>

using namespace Isis;

namespace {
  using Position = std::tuple<int, int, int>;

  class BufferManagerTest : public ::testing::Test {
   protected:
    static void SetUpTestSuite() {
      Preference::Preferences(true);
    }

    static std::vector<Position> CollectWithIncrement(BufferManager &bm) {
      std::vector<Position> positions;
      for (bm.begin(); !bm.end(); bm++) {
        positions.emplace_back(bm.Sample(), bm.Line(), bm.Band());
      }
      return positions;
    }

    static std::vector<Position> CollectWithNext(BufferManager &bm) {
      std::vector<Position> positions;
      bm.begin();
      do {
        positions.emplace_back(bm.Sample(), bm.Line(), bm.Band());
      } while (bm.next());
      return positions;
    }
  };

  std::vector<Position> ExpectedTileTraversal() {
    return {
      {1, 1, 1}, {4, 1, 1},
      {1, 3, 1}, {4, 3, 1},
      {1, 1, 2}, {4, 1, 2},
      {1, 3, 2}, {4, 3, 2}
    };
  }

  std::vector<Position> ExpectedPixelByPixelTraversal() {
    return {
      {1, 1, 1}, {2, 1, 1}, {3, 1, 1}, {4, 1, 1},
      {1, 2, 1}, {2, 2, 1}, {3, 2, 1}, {4, 2, 1},
      {1, 3, 1}, {2, 3, 1}, {3, 3, 1}, {4, 3, 1},
      {1, 1, 2}, {2, 1, 2}, {3, 1, 2}, {4, 1, 2},
      {1, 2, 2}, {2, 2, 2}, {3, 2, 2}, {4, 2, 2},
      {1, 3, 2}, {2, 3, 2}, {3, 3, 2}, {4, 3, 2}
    };
  }

  std::vector<Position> ExpectedStridedSampleTraversal() {
    return {
      {1, 1, 1}, {3, 1, 1}, {5, 1, 1}
    };
  }
}  // namespace

TEST_F(BufferManagerTest, TileTraversalMatchesLegacySequence) {
  BufferManager bm(6, 4, 2, 3, 2, 1, Isis::Real);

  EXPECT_EQ(CollectWithIncrement(bm), ExpectedTileTraversal());
}

TEST_F(BufferManagerTest, PixelByPixelTraversalMatchesLegacySequence) {
  BufferManager bm(4, 3, 2, 1, 1, 1, Isis::Real);

  EXPECT_EQ(CollectWithNext(bm), ExpectedPixelByPixelTraversal());
}

TEST_F(BufferManagerTest, StridedSampleTraversalMatchesLegacySequence) {
  BufferManager bm(5, 1, 1, 2, 1, 1, Isis::Real);

  EXPECT_EQ(CollectWithNext(bm), ExpectedStridedSampleTraversal());
}

// The legacy BufferManager unit test exercised two traversal APIs:
//
//   1) for (begin(); !end(); bm++)
//   2) begin(); do { ... } while (next());
//
// Both are intended to iterate over the same logical buffer positions.
// The original test printed both sequences but did not explicitly verify
// they were identical. This test upgrades that behavior by confirming
// the two traversal methods produce the exact same sequence of
// (Sample, Line, Band) positions.
TEST_F(BufferManagerTest, IncrementAndNextProduceSameSequence) {
  BufferManager bmIncrement(4, 3, 2, 1, 1, 1, Isis::Real);
  BufferManager bmNext(4, 3, 2, 1, 1, 1, Isis::Real);

  EXPECT_EQ(CollectWithIncrement(bmIncrement), CollectWithNext(bmNext));
}
