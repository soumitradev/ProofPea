#ifndef CNF_CONVERTOR_INVERT_TRANSFORMER
#define CNF_CONVERTOR_INVERT_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>
#include <transformer/cnf.h>
#include <transformer/common.h>

namespace transformer {
namespace invert {

std::variant<bool, error::eval::unexpected_node> invertTree(
    parser::parser::AST* ast);

}  // namespace invert
}  // namespace transformer

#endif  // CNF_CONVERTOR_INVERT_TRANSFORMER