#ifndef CNF_CONVERTOR_EVAL
#define CNF_CONVERTOR_EVAL

#include <error/eval.h>
#include <parser/parser.h>

#include <unordered_set>

namespace eval {
std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
evaluateState(parser::parser::AST* ast,
              std::unordered_map<const parser::parser::Node*, bool> state);
}  // namespace eval

#endif  // CNF_CONVERTOR_EVAL
