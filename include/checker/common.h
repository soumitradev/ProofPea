#ifndef CNF_CONVERTOR_COMMON_CHECKER
#define CNF_CONVERTOR_COMMON_CHECKER

#include <debug/ast.h>
#include <error/eval.h>
#include <error/unknown.h>
#include <parser/parser.h>
#include <transformer/cnf.h>
#include <transformer/invert.h>

namespace checker {
namespace common {

void splitAST(parser::parser::Node* node, util::symbols::SymbolType separator,
              std::vector<parser::parser::AST*>& result);

}  // namespace common
}  // namespace checker

#endif  // CNF_CONVERTOR_COMMON_CHECKER
