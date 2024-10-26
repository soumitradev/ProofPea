#ifndef CNF_CONVERTOR_ERROR_PARSER
#define CNF_CONVERTOR_ERROR_PARSER

#include <error/error.h>

#include <string>

namespace error {
namespace parser {

struct unexpected_token : error::error {
  unexpected_token(std::string c);
};
struct unexpected_node : error::error {
  unexpected_node(std::string c);
};

}  // namespace parser

namespace tokenizer {

struct invalid_symbol : error::error {
  invalid_symbol(std::string c);
};

}  // namespace tokenizer

}  // namespace error

#endif  // CNF_CONVERTOR_ERROR_PARSER
