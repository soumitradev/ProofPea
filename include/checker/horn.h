#ifndef CNF_CONVERTOR_HORN_CHECKER
#define CNF_CONVERTOR_HORN_CHECKER

#include <checker/common.h>
#include <error/eval.h>
#include <error/unknown.h>
#include <parser/parser.h>

namespace checker {
namespace horn {

bool checkHornFormula(parser::parser::AST* ast);

}  // namespace horn
}  // namespace checker

#endif  // CNF_CONVERTOR_HORN_CHECKER
