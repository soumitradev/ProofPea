#ifndef CNF_CONVERTOR_EQUIVALENCE_CHECKER
#define CNF_CONVERTOR_EQUIVALENCE_CHECKER

#include <error/equivalence.h>
#include <error/eval.h>
#include <error/unknown.h>
#include <parser/parser.h>
#include <truth_table/tabulate.h>

#include <map>
#include <set>

namespace checker {
namespace equivalence {

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms,
             error::equivalence::equivalence_chack_unsupported,
             error::equivalence::incomplete_truth_table>
checkEquivalence(const parser::parser::AST* ast1,
                 const parser::parser::AST* ast2);

}  // namespace equivalence
}  // namespace checker

#endif  // CNF_CONVERTOR_EQUIVALENCE_CHECKER
