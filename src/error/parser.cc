#include <error/parser.h>

namespace error {
namespace parser {

unexpected_token::unexpected_token(std::string c) {
  error = "UnexpectedToken";
  cause = c;
}

}  // namespace parser
}  // namespace error
