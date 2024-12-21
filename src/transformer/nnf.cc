#include <transformer/nnf.h>

namespace transformer {
namespace nnf {

std::variant<bool, error::eval::unexpected_node> transformToNNF(
    parser::parser::AST* ast) {
  return transformer::impl_free::transformToIMPLFREE(ast);
}

}  // namespace nnf
}  // namespace transformer
