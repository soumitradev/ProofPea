#include <eval/eval.h>

namespace eval {

std::variant<bool, error::eval::unexpected_node> evaluateSubtree(
    const parser::Node* root,
    std::unordered_map<const parser::Node*, bool>& state) {
  if (root->type == parser::ABSOLUTE) {
    const auto absoluteToken = std::get<const parser::Absolute*>(root->node);
    if (absoluteToken->token->type == util::symbols::ABSOLUTETRUE) {
      return true;
    } else if (absoluteToken->token->type == util::symbols::ABSOLUTEFALSE) {
      return false;
    } else {
      return error::eval::unexpected_node{
          "Found unexpected node of type ABSOLUTE with lexeme " +
          absoluteToken->token->lexeme + " at position " +
          std::to_string(absoluteToken->token->position)};
    }
  } else if (root->type == parser::ATOM) {
    const auto atomValue = state.find(root);

    if (atomValue == state.end()) {
      const auto atomToken = std::get<const parser::Atom*>(root->node);
      return error::eval::unexpected_node{
          "Found unexpected node of type ATOM with lexeme " +
          atomToken->token->lexeme + " at position " +
          std::to_string(atomToken->token->position)};
    };

    return atomValue->second;
  } else if (root->type == parser::UNARY) {
    const auto unaryOperator =
        std::get<const parser::UnaryOperator*>(root->node);
    if (unaryOperator->op->type == util::symbols::NEG) {
      const auto childResult = evaluateSubtree(unaryOperator->child, state);

      if (std::holds_alternative<error::eval::unexpected_node>(childResult)) {
        return std::get<error::eval::unexpected_node>(childResult);
      }

      const auto childValue = std::get<bool>(childResult);
      return !childValue;
    } else {
      return error::eval::unexpected_node{
          "Found unexpected node of type UNARY with lexeme " +
          unaryOperator->op->lexeme + " at position " +
          std::to_string(unaryOperator->op->position)};
    }
  } else if (root->type == parser::BINARY) {
    const auto binaryOperator =
        std::get<const parser::BinaryOperator*>(root->node);

    const auto leftResult = evaluateSubtree(binaryOperator->left, state);
    if (std::holds_alternative<error::eval::unexpected_node>(leftResult)) {
      return std::get<error::eval::unexpected_node>(leftResult);
    }

    const auto rightResult = evaluateSubtree(binaryOperator->right, state);
    if (std::holds_alternative<error::eval::unexpected_node>(rightResult)) {
      return std::get<error::eval::unexpected_node>(rightResult);
    }
    const auto leftValue = std::get<bool>(leftResult);
    const auto rightValue = std::get<bool>(rightResult);
    if (binaryOperator->op->type == util::symbols::CONJUNCT) {
      return leftValue && rightValue;
    } else if (binaryOperator->op->type == util::symbols::DISJUNCT) {
      return leftValue || rightValue;
    } else if (binaryOperator->op->type == util::symbols::IMPL) {
      if (leftValue) return rightValue;
      return true;
    } else {
      return error::eval::unexpected_node{
          "Found unexpected node of type BINARY with lexeme " +
          binaryOperator->op->lexeme + " at position " +
          std::to_string(binaryOperator->op->position)};
    }
  }
  return error::eval::unexpected_node{
      "Found unexpected node of type UNKNOWN. Type: " + root->type};
}

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
evaluateState(parser::AST* ast,
              std::unordered_map<const parser::Node*, bool> state) {
  std::unordered_set<const parser::Node*> stateNodes;
  for (auto& x : state) {
    stateNodes.insert(x.first);
  }
  std::unordered_set<const parser::Node*> astAtoms;
  for (auto& x : ast->atoms) {
    astAtoms.insert(x.second);
  }
  if (astAtoms != stateNodes) {
    return error::eval::mismatched_atoms{
        "AST and state atoms do not match. Cannot continue with eval."};
  }

  const auto evalResult = evaluateSubtree(ast->root, state);
  if (std::holds_alternative<error::eval::unexpected_node>(evalResult)) {
    return std::get<error::eval::unexpected_node>(evalResult);
  }
  return std::get<bool>(evalResult);
}

}  // namespace eval
