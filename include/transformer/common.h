#ifndef CNF_CONVERTOR_COMMON_TRANSFORMER
#define CNF_CONVERTOR_COMMON_TRANSFORMER

#include <error/eval.h>
#include <parser/parser.h>

namespace transformer {
namespace common {

std::variant<bool, error::eval::unexpected_node> rebuildASTMaps(
    std::shared_ptr<parser::parser::AST> ast);

}  // namespace common
}  // namespace transformer

#endif  // CNF_CONVERTOR_COMMON_TRANSFORMER
