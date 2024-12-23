#ifndef CNF_CONVERTOR_VALIDITY_CHECKER
#define CNF_CONVERTOR_VALIDITY_CHECKER

#include <debug/ast.h>
#include <error/eval.h>
#include <error/unknown.h>
#include <parser/parser.h>
#include <transformer/cnf.h>
#include <transformer/invert.h>

namespace checker {
namespace validity {

std::variant<bool, error::eval::unexpected_node, error::unknown::unknown_error>
checkValidity(parser::parser::AST* ast);

}  // namespace validity
}  // namespace checker

#endif  // CNF_CONVERTOR_VALIDITY_CHECKER
