
#include <checker/common.h>

namespace proofpea {
namespace checker {
namespace common {

void splitAST(const parser::parser::Node* node,
              const util::symbols::SymbolType separator,
              std::vector<std::shared_ptr<parser::parser::AST>>& result) {
  if (node->type == parser::parser::NodeType::BINARY) {
    const auto binaryOp =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node->node);
    if (binaryOp->op->type == separator) {
      splitAST(binaryOp->left.get(), separator, result);
      splitAST(binaryOp->right.get(), separator, result);
      return;
    }
  }

  const auto newAST = std::make_shared<parser::parser::AST>();
  newAST->root = parser::parser::AST::copyNode(
      node, newAST->tokens, newAST->atoms, newAST->absolutes);

  result.push_back(newAST);
  return;
}

}  // namespace common
}  // namespace checker
}  // namespace proofpea
