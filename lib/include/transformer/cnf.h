#ifndef CNF_CONVERTOR_CNF_TRANSFORMER
#define CNF_CONVERTOR_CNF_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>
#include <transformer/common.h>
#include <transformer/nnf.h>

namespace proofpea {
namespace transformer {
namespace cnf {

std::variant<bool, error::eval::unexpected_node> transformToCNF(
    std::shared_ptr<parser::parser::AST> ast);

}  // namespace cnf
}  // namespace transformer
}  // namespace proofpea

#endif  // CNF_CONVERTOR_CNF_TRANSFORMER
