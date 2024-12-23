#include <checker/horn.h>

namespace checker {
namespace horn {

bool checkPFormula(parser::parser::AST* ast) {
  if ((ast->root->type == parser::parser::NodeType::ATOM) ||
      (ast->root->type == parser::parser::NodeType::ABSOLUTE)) {
    return true;
  }

  return false;
}

bool checkHornClause(parser::parser::AST* ast) {
  if (ast->root->type != parser::parser::NodeType::BINARY) return false;
  const auto rootOp =
      std::get<parser::parser::BinaryOperator*>(ast->root->node);
  if (rootOp->op->type != util::symbols::SymbolType::IMPL) return false;

  const auto copyAST = parser::parser::AST::copy(ast);
  std::vector<parser::parser::AST*> impl;
  common::splitAST(copyAST->root, util::symbols::SymbolType::IMPL, impl);
  parser::parser::deallocAST(copyAST);

  if (impl.size() != 2) return false;

  if (!checkPFormula(impl[1])) return false;
  parser::parser::deallocAST(impl[1]);

  std::vector<parser::parser::AST*> pFormulae;
  common::splitAST(impl[0]->root, util::symbols::SymbolType::CONJUNCT,
                   pFormulae);
  parser::parser::deallocAST(impl[0]);

  for (size_t i = 0; i < pFormulae.size(); i++) {
    const auto correct = checkPFormula(pFormulae[i]);
    parser::parser::deallocAST(pFormulae[i]);
    if (!correct) return false;
  }

  return true;
}

bool checkHornFormula(parser::parser::AST* ast) {
  const auto copyAST = parser::parser::AST::copy(ast);

  std::vector<parser::parser::AST*> clauses;
  common::splitAST(copyAST->root, util::symbols::SymbolType::CONJUNCT, clauses);
  parser::parser::deallocAST(copyAST);

  for (size_t i = 0; i < clauses.size(); i++) {
    if (!checkHornClause(clauses[i])) return false;
    parser::parser::deallocAST(clauses[i]);
  }

  return true;
}

}  // namespace horn
}  // namespace checker
