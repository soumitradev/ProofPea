#include <error/parser.h>

namespace error {
namespace parser {

unexpected_token::unexpected_token(std::string c) {
  error = "UnexpectedToken";
  cause = c;
}

unexpected_node::unexpected_node(std::string c) {
  error = "UnexpectedNode";
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
