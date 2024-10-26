#include <error/parser.h>

namespace error {
namespace parser {

unexpected_token::unexpected_token(std::string c) {
  error = "UnexpectedToken";
  cause = c;
}

}  // namespace parser

namespace tokenizer {

invalid_symbol::invalid_symbol(std::string c) {
  error = "InvalidSymbol";
  cause = c;
}

}  // namespace tokenizer

}  // namespace error
