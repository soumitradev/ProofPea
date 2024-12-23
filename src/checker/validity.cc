#include <checker/validity.h>

namespace checker {
namespace validity {

std::variant<bool, error::eval::unexpected_node> checkEquality(
    parser::parser::Node* node1, parser::parser::Node* node2) {
  if (node1->type != node2->type) return false;
  if (node1->type == parser::parser::NodeType::ABSOLUTE) {
    const auto abs1 = std::get<parser::parser::Absolute*>(node1->node);
    const auto abs2 = std::get<parser::parser::Absolute*>(node2->node);
    return abs1->token->type == abs2->token->type;
  } else if (node1->type == parser::parser::NodeType::ATOM) {
    const auto atom1 = std::get<parser::parser::Atom*>(node1->node);
    const auto atom2 = std::get<parser::parser::Atom*>(node2->node);
    return atom1->token->lexeme == atom2->token->lexeme;
  } else if (node1->type == parser::parser::NodeType::UNARY) {
    const auto unary1 = std::get<parser::parser::UnaryOperator*>(node1->node);
    const auto unary2 = std::get<parser::parser::UnaryOperator*>(node2->node);
    return unary1->op->type == unary2->op->type;
  } else if (node1->type == parser::parser::NodeType::BINARY) {
    const auto binary1 = std::get<parser::parser::BinaryOperator*>(node1->node);
    const auto binary2 = std::get<parser::parser::BinaryOperator*>(node2->node);
    return binary1->op->type == binary2->op->type;
  }

  return error::eval::unexpected_node{
      "Encountered unknown type of node " +
      util::symbols::SymbolTypeArray[node1->type] + " in checkEquality"};
}

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

std::variant<bool, error::eval::unexpected_node, error::unknown::unknown_error>
checkValidity(parser::parser::AST* ast) {
  const auto copyAST = parser::parser::AST::copy(ast);
  const auto cnfTransformResult = transformer::cnf::transformToCNF(copyAST);
  if (std::holds_alternative<error::eval::unexpected_node>(
          cnfTransformResult)) {
    return std::get<error::eval::unexpected_node>(cnfTransformResult);
  } else if (std::holds_alternative<bool>(cnfTransformResult)) {
    if (!std::get<bool>(cnfTransformResult)) {
      return error::unknown::unknown_error{
          "transformToCNF returned false in checkValidity"};
    }
  }

  debug::ast::printAST(copyAST, false, (char*)"ast.svg");

  std::vector<parser::parser::AST*> conjuncts;
  splitAST(copyAST->root, util::symbols::SymbolType::CONJUNCT, conjuncts);
  parser::parser::deallocAST(copyAST);

  for (size_t i = 0; i < conjuncts.size(); i++) {
    std::vector<parser::parser::AST*> literals;
    splitAST(conjuncts[i]->root, util::symbols::SymbolType::DISJUNCT, literals);
    parser::parser::deallocAST(conjuncts[i]);
    std::vector<parser::parser::AST*> invertedLiterals(literals.size());

    for (size_t i = 0; i < literals.size(); i++) {
      invertedLiterals[i] = parser::parser::AST::copy(literals[i]);
      transformer::invert::invertTree(invertedLiterals[i]);
    }

    bool conjunctValid = false;
    for (size_t j = 0; (j < literals.size()) && (!conjunctValid); j++) {
      for (size_t k = j + 1; (k < invertedLiterals.size()) && (!conjunctValid);
           k++) {
        const auto equalityResult =
            checkEquality(literals[j]->root, invertedLiterals[k]->root);
        if (std::holds_alternative<error::eval::unexpected_node>(
                equalityResult)) {
          return std::get<error::eval::unexpected_node>(equalityResult);
        } else if (std::holds_alternative<bool>(equalityResult)) {
          const auto boolResult = std::get<bool>(equalityResult);
          // We have found one pair of such literals
          if (boolResult) {
            conjunctValid = true;
            break;
          }
        }
      }
    }

    for (size_t j = 0; j < literals.size(); j++) {
      parser::parser::deallocAST(literals[j]);
      parser::parser::deallocAST(invertedLiterals[j]);
    }

    if (!conjunctValid) return false;
  }

  return true;
}

}  // namespace validity
}  // namespace checker
