#ifndef CNF_CONVERTOR_EVAL
#define CNF_CONVERTOR_EVAL

#include <error/eval.h>
#include <parser/parser.h>

#include <unordered_set>

namespace eval {
std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
evaluateState(
    const parser::parser::AST* ast,
    const std::unordered_map<std::shared_ptr<parser::parser::Node>, bool>
        state);
}  // namespace eval

#endif  // CNF_CONVERTOR_EVAL
