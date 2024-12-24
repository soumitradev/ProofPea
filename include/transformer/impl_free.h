#ifndef CNF_CONVERTOR_IMPL_FREE_TRANSFORMER
#define CNF_CONVERTOR_IMPL_FREE_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>

namespace transformer {
namespace impl_free {

std::variant<bool, error::eval::unexpected_node> transformToIMPLFREE(
    std::shared_ptr<parser::parser::AST> ast);

}  // namespace impl_free
}  // namespace transformer

#endif  // CNF_CONVERTOR_IMPL_FREE_TRANSFORMER
