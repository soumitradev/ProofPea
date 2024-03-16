#ifndef CNF_CONVERTOR_ERROR_PARSER
#define CNF_CONVERTOR_ERROR_PARSER

#include <error/error.h>

#include <string>

namespace error {
namespace parser {

struct unexpected_token : error::error {
  unexpected_token(std::string c);
};

}  // namespace parser
}  // namespace error

#endif  // CNF_CONVERTOR_ERROR_PARSER
