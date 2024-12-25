#include <error/unknown.h>

namespace proofpea {
namespace error {
namespace unknown {

unknown_error::unknown_error(std::string c) {
  error = "UnknownError";
  cause = c;
}

}  // namespace unknown
}  // namespace error
}  // namespace proofpea
