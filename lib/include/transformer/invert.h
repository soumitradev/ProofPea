#ifndef CNF_CONVERTOR_INVERT_TRANSFORMER
#define CNF_CONVERTOR_INVERT_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>
#include <transformer/cnf.h>
#include <transformer/common.h>

namespace proofpea {
namespace transformer {
namespace invert {

std::variant<bool, error::eval::unexpected_node> invertTree(
    std::shared_ptr<parser::parser::AST> ast);

}  // namespace invert
}  // namespace transformer
}  // namespace proofpea

#endif  // CNF_CONVERTOR_INVERT_TRANSFORMER
