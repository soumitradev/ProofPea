
#include <checker/common.h>

namespace checker {
namespace common {

void splitAST(parser::parser::Node* node, util::symbols::SymbolType separator,
              std::vector<parser::parser::AST*>& result) {
  if (node->type == parser::parser::NodeType::BINARY) {
    const auto binaryOp = std::get<parser::parser::BinaryOperator*>(node->node);
    if (binaryOp->op->type == separator) {
      splitAST(binaryOp->left, separator, result);
      splitAST(binaryOp->right, separator, result);
      return;
    }
  }

  const auto tempAST = new parser::parser::AST{};
  tempAST->root = node;
  const auto parent = tempAST->root->parent;
  tempAST->root->parent = nullptr;

  const auto newAST = parser::parser::AST::copy(tempAST);
  // No need to use the deallocAST function because this AST was not created
  // like a normal AST. It is only a shell, and we only need to get rid of the
  // shell. The insides are the original nodes passed to this function.
  delete tempAST;

  newAST->root->parent = nullptr;
  node->parent = parent;

  result.push_back(newAST);
  return;
}

}  // namespace common
}  // namespace checker
