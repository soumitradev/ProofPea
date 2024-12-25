#ifndef CNF_CONVERTOR_ERROR_HORN
#define CNF_CONVERTOR_ERROR_HORN

#include <error/error.h>

#include <string>

namespace proofpea {
namespace error {
namespace horn {

struct invalid_horn_formula : error::error {
  invalid_horn_formula(std::string c);
};

}  // namespace horn
}  // namespace error
}  // namespace proofpea

#endif  // CNF_CONVERTOR_ERROR_HORN
