#ifndef CNF_CONVERTOR_UTIL_MEMORY
#define CNF_CONVERTOR_UTIL_MEMORY

#include <memory>

namespace proofpea {
namespace util {
namespace memory {

template <typename T>
bool is_uninitialized(std::weak_ptr<T> const& weak);

}  // namespace memory
}  // namespace util
}  // namespace proofpea

#endif  // CNF_CONVERTOR_UTIL_MEMORY
