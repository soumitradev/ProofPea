#ifndef CNF_CONVERTOR_ERROR_UNKNOWN
#define CNF_CONVERTOR_ERROR_UNKNOWN

#include <error/error.h>

#include <string>

namespace proofpea {
namespace error {
namespace unknown {

struct unknown_error : error::error {
  unknown_error(std::string c);
};

}  // namespace unknown
}  // namespace error
}  // namespace proofpea

#endif  // CNF_CONVERTOR_ERROR_UNKNOWN
