#ifndef CNF_CONVERTOR_ERROR_ERROR
#define CNF_CONVERTOR_ERROR_ERROR

#include <string>

namespace error {

struct error {
  std::string error;
  std::string cause;
};

}  // namespace error

#endif  // CNF_CONVERTOR_ERROR_ERROR
