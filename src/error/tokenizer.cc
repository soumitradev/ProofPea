#include <error/tokenizer.h>

namespace error {
namespace tokenizer {

invalid_symbol::invalid_symbol(std::string c) {
  error = "InvalidSymbol";
  cause = c;
}

}  // namespace tokenizer
}  // namespace error
