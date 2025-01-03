#include <util/memory.h>

namespace proofpea {
namespace util {
namespace memory {

template <typename T>
bool is_uninitialized(std::weak_ptr<T> const& weak) {
  using wt = std::weak_ptr<T>;
  return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

}  // namespace memory
}  // namespace util
}  // namespace proofpea
