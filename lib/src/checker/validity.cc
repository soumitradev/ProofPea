#include <checker/validity.h>

namespace proofpea {
namespace checker {
namespace validity {

std::variant<bool, error::eval::unexpected_node> checkEquality(
    const std::shared_ptr<parser::parser::Node> node1,
    const std::shared_ptr<parser::parser::Node> node2) {
  if (node1->type != node2->type) return false;
  if (node1->type == parser::parser::NodeType::ABSOLUTE) {
    const auto abs1 =
        std::get<std::shared_ptr<parser::parser::Absolute>>(node1->node);
    const auto abs2 =
        std::get<std::shared_ptr<parser::parser::Absolute>>(node2->node);
    return abs1->token->type == abs2->token->type;
  } else if (node1->type == parser::parser::NodeType::ATOM) {
    const auto atom1 =
        std::get<std::shared_ptr<parser::parser::Atom>>(node1->node);
    const auto atom2 =
        std::get<std::shared_ptr<parser::parser::Atom>>(node2->node);
    return atom1->token->lexeme == atom2->token->lexeme;
  } else if (node1->type == parser::parser::NodeType::UNARY) {
    const auto unary1 =
        std::get<std::shared_ptr<parser::parser::UnaryOperator>>(node1->node);
    const auto unary2 =
        std::get<std::shared_ptr<parser::parser::UnaryOperator>>(node2->node);
    return unary1->op->type == unary2->op->type;
  } else if (node1->type == parser::parser::NodeType::BINARY) {
    const auto binary1 =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node1->node);
    const auto binary2 =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node2->node);
    return binary1->op->type == binary2->op->type;
  }

  return error::eval::unexpected_node{
      "Encountered unknown type of node " +
      util::symbols::SymbolTypeArray[node1->type] + " in checkEquality"};
}

std::variant<bool, error::eval::unexpected_node, error::unknown::unknown_error>
checkValidity(const parser::parser::AST* ast) {
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

  std::vector<std::shared_ptr<parser::parser::AST>> conjuncts;
  common::splitAST(copyAST->root.get(), util::symbols::SymbolType::CONJUNCT,
                   conjuncts);

  for (size_t i = 0; i < conjuncts.size(); i++) {
    std::vector<std::shared_ptr<parser::parser::AST>> literals;
    common::splitAST(conjuncts[i]->root.get(),
                     util::symbols::SymbolType::DISJUNCT, literals);
    std::vector<std::shared_ptr<parser::parser::AST>> invertedLiterals(
        literals.size());

    for (size_t i = 0; i < literals.size(); i++) {
      invertedLiterals[i] = parser::parser::AST::copy(literals[i].get());
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

    if (!conjunctValid) return false;
  }

  return true;
}

}  // namespace validity
}  // namespace checker
}  // namespace proofpea
