#ifndef CNF_CONVERTOR_TRUTH_TABLE_TABULATOR
#define CNF_CONVERTOR_TRUTH_TABLE_TABULATOR

#include <eval/eval.h>
#include <parser/parser.h>

#include <algorithm>
#include <map>
#include <vector>

namespace truth_table {
namespace tabulator {

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
printTruthTable(parser::parser::AST* ast);
}
}  // namespace truth_table

#endif  // CNF_CONVERTOR_TRUTH_TABLE_TABULATOR
