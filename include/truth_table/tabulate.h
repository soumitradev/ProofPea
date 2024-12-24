#ifndef CNF_CONVERTOR_TRUTH_TABLE_TABULATOR
#define CNF_CONVERTOR_TRUTH_TABLE_TABULATOR

#include <eval/eval.h>
#include <parser/parser.h>

#include <algorithm>
#include <map>
#include <set>
#include <vector>

namespace truth_table {
namespace tabulator {

std::variant<std::vector<std::string>, error::eval::unexpected_node,
             error::eval::mismatched_atoms>
constructTruthTable(std::map<std::vector<bool>, bool>& table,
                    const parser::parser::AST* ast);

std::variant<std::vector<std::string>, error::eval::unexpected_node,
             error::eval::mismatched_atoms>
constructPartialTruthTable(
    std::map<std::vector<bool>, bool>& table, const parser::parser::AST* ast,
    const std::set<std::shared_ptr<parser::parser::Node>>& skippedNodes);

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
printTruthTable(const parser::parser::AST* ast);
}  // namespace tabulator
}  // namespace truth_table

#endif  // CNF_CONVERTOR_TRUTH_TABLE_TABULATOR
