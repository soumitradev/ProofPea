#ifndef CNF_CONVERTOR_COMMON_CHECKER
#define CNF_CONVERTOR_COMMON_CHECKER

#include <debug/ast.h>
#include <error/eval.h>
#include <error/unknown.h>
#include <parser/parser.h>
#include <transformer/cnf.h>
#include <transformer/invert.h>

namespace proofpea {
namespace checker {
namespace common {

void splitAST(const parser::parser::Node* node,
              const util::symbols::SymbolType separator,
              std::vector<std::shared_ptr<parser::parser::AST>>& result);

}  // namespace common
}  // namespace checker
}  // namespace proofpea

#endif  // CNF_CONVERTOR_COMMON_CHECKER
