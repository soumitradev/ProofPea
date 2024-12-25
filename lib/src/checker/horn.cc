#include <checker/horn.h>

namespace proofpea {
namespace checker {
namespace horn {

bool checkPFormula(const std::shared_ptr<parser::parser::AST> ast) {
  if ((ast->root->type == parser::parser::NodeType::ATOM) ||
      (ast->root->type == parser::parser::NodeType::ABSOLUTE)) {
    return true;
  }

  return false;
}

bool checkHornClause(const std::shared_ptr<parser::parser::AST> ast) {
  if (ast->root->type != parser::parser::NodeType::BINARY) return false;
  const auto rootOp = std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
      ast->root->node);
  if (rootOp->op->type != util::symbols::SymbolType::IMPL) return false;

  const auto copyAST = parser::parser::AST::copy(ast.get());
  std::vector<std::shared_ptr<parser::parser::AST>> impl;
  common::splitAST(copyAST->root.get(), util::symbols::SymbolType::IMPL, impl);

  if (impl.size() != 2) return false;

  if (!checkPFormula(impl[1])) return false;

  std::vector<std::shared_ptr<parser::parser::AST>> pFormulae;
  common::splitAST(impl[0]->root.get(), util::symbols::SymbolType::CONJUNCT,
                   pFormulae);

  for (size_t i = 0; i < pFormulae.size(); i++) {
    const auto correct = checkPFormula(pFormulae[i]);
    if (!correct) return false;
  }

  return true;
}

bool checkHornFormula(const parser::parser::AST* ast) {
  const auto copyAST = parser::parser::AST::copy(ast);

  std::vector<std::shared_ptr<parser::parser::AST>> clauses;
  common::splitAST(copyAST->root.get(), util::symbols::SymbolType::CONJUNCT,
                   clauses);

  for (size_t i = 0; i < clauses.size(); i++) {
    if (!checkHornClause(clauses[i])) return false;
  }

  return true;
}

}  // namespace horn
}  // namespace checker
}  // namespace proofpea
