#ifndef CNF_CONVERTOR_DEBUG_AST
#define CNF_CONVERTOR_DEBUG_AST

#include <graphviz/gvc.h>
#include <logger/logger.h>
#include <parser/parser.h>

#include <fstream>
#include <variant>

namespace debug {
namespace ast {
void printAST(const parser::AST *ast);
}  // namespace ast
}  // namespace debug

#endif  // CNF_CONVERTOR_DEBUG_AST
