#ifndef CNF_CONVERTOR_DEBUG_AST
#define CNF_CONVERTOR_DEBUG_AST

#include <graphviz/gvc.h>
#include <logger/logger.h>
#include <parser/parser.h>

#include <fstream>
#include <variant>

namespace proofpea {
namespace debug {
namespace ast {
void printAST(const parser::parser::AST *ast, const bool renderParentEdge,
              char *filepath);
}  // namespace ast
}  // namespace debug
}  // namespace proofpea

#endif  // CNF_CONVERTOR_DEBUG_AST
