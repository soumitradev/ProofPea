#ifndef CNF_CONVERTOR_NNF_TRANSFORMER
#define CNF_CONVERTOR_NNF_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>
#include <transformer/common.h>
#include <transformer/impl_free.h>
#include <util/memory.h>

namespace transformer {
namespace nnf {

std::variant<bool, error::eval::unexpected_node> transformToNNF(
    std::shared_ptr<parser::parser::AST> ast);

}  // namespace nnf
}  // namespace transformer

#endif  // CNF_CONVERTOR_NNF_TRANSFORMER
