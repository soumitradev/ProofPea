#ifndef CNF_CONVERTOR_HORN_SATISFIABILITY_CHECKER
#define CNF_CONVERTOR_HORN_SATISFIABILITY_CHECKER

#include <checker/common.h>
#include <checker/horn.h>
#include <error/eval.h>
#include <error/horn.h>
#include <error/unknown.h>
#include <parser/parser.h>

namespace checker {
namespace horn_satisfiability {

std::variant<bool, error::horn::invalid_horn_formula> checkHornSatisfiability(
    const parser::parser::AST* ast);

}  // namespace horn_satisfiability
}  // namespace checker

#endif  // CNF_CONVERTOR_HORN_SATISFIABILITY_CHECKER
