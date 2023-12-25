#ifndef CNF_CONVERTOR_ERROR_TOKENIZER
#define CNF_CONVERTOR_ERROR_TOKENIZER

#include <error/error.h>

#include <string>

namespace error {
namespace tokenizer {

struct invalid_symbol : error::error {
  invalid_symbol(std::string c);
};

}  // namespace tokenizer
}  // namespace error

#endif  // CNF_CONVERTOR_ERROR_TOKENIZER
