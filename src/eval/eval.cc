#include <eval/eval.h>

namespace eval {

std::variant<bool, error::eval::unexpected_node> evaluateSubtree(
    const parser::parser::Node* root,
    std::unordered_map<const parser::parser::Node*, bool>& state) {
  if (root->type == parser::parser::ABSOLUTE) {
    logger::Logger::dispatchLog(logger::debugLog{"Found ABSOLUTE node"});
    const auto absoluteToken =
        std::get<const parser::parser::Absolute*>(root->node);
    if (absoluteToken->token->type == util::symbols::ABSOLUTETRUE) {
      logger::Logger::dispatchLog(
          logger::debugLog{"Evaluating ABSOLUTE node \"" +
                           absoluteToken->token->lexeme + "\" as true"});
      return true;
    } else if (absoluteToken->token->type == util::symbols::ABSOLUTEFALSE) {
      logger::Logger::dispatchLog(
          logger::debugLog{"Evaluating ABSOLUTE node \"" +
                           absoluteToken->token->lexeme + "\" as false"});
      return false;
    } else {
      return error::eval::unexpected_node{
          "Found unexpected node of type ABSOLUTE with lexeme " +
          absoluteToken->token->lexeme + " at position " +
          std::to_string(absoluteToken->token->position)};
    }
  } else if (root->type == parser::parser::ATOM) {
    logger::Logger::dispatchLog(logger::debugLog{"Found ATOM node"});
    const auto atomToken = std::get<const parser::parser::Atom*>(root->node);
    logger::Logger::dispatchLog(logger::debugLog{
        "Finding atom node \"" + atomToken->token->lexeme + "\" in state map"});
    const auto atomValue = state.find(root);

    if (atomValue == state.end()) {
      return error::eval::unexpected_node{
          "Found unexpected node of type ATOM with lexeme " +
          atomToken->token->lexeme + " at position " +
          std::to_string(atomToken->token->position)};
    };

    logger::Logger::dispatchLog(logger::debugLog{
        "Evaluated atom node \"" + atomToken->token->lexeme +
        "\" in state map as " + (atomValue->second ? "true" : "false")});
    return atomValue->second;
  } else if (root->type == parser::parser::UNARY) {
    logger::Logger::dispatchLog(logger::debugLog{"Found UNARY node"});
    const auto unaryOperator =
        std::get<const parser::parser::UnaryOperator*>(root->node);
    if (unaryOperator->op->type == util::symbols::NEG) {
      logger::Logger::dispatchLog(logger::debugLog{"Identified UNARY node \"" +
                                                   unaryOperator->op->lexeme +
                                                   "\" as NEGATION"});
      const auto childResult = evaluateSubtree(unaryOperator->child, state);

      if (std::holds_alternative<error::eval::unexpected_node>(childResult)) {
        return std::get<error::eval::unexpected_node>(childResult);
      }

      const auto childValue = std::get<bool>(childResult);
      logger::Logger::dispatchLog(logger::debugLog{
          "Evaluated UNARY node \"" + unaryOperator->op->lexeme + "\" to " +
          (!childValue ? "true" : "false")});
      return !childValue;
    }
    return error::eval::unexpected_node{
        "Found unexpected node of type UNARY with lexeme " +
        unaryOperator->op->lexeme + " at position " +
        std::to_string(unaryOperator->op->position)};
  } else if (root->type == parser::parser::BINARY) {
    logger::Logger::dispatchLog(logger::debugLog{"Found BINARY node"});
    const auto binaryOperator =
        std::get<const parser::parser::BinaryOperator*>(root->node);

    logger::Logger::dispatchLog(
        logger::debugLog{"Evaluating left subtree of operator \"" +
                         binaryOperator->op->lexeme + "\""});
    const auto leftResult = evaluateSubtree(binaryOperator->left, state);
    if (std::holds_alternative<error::eval::unexpected_node>(leftResult)) {
      return std::get<error::eval::unexpected_node>(leftResult);
    }
    logger::Logger::dispatchLog(
        logger::debugLog{"Evaluating right subtree of operator \"" +
                         binaryOperator->op->lexeme + "\""});

    const auto rightResult = evaluateSubtree(binaryOperator->right, state);
    if (std::holds_alternative<error::eval::unexpected_node>(rightResult)) {
      return std::get<error::eval::unexpected_node>(rightResult);
    }

    const auto leftValue = std::get<bool>(leftResult);
    logger::Logger::dispatchLog(logger::debugLog{
        "Left subtree of operator \"" + binaryOperator->op->lexeme +
        "\" evaluated to: " + (leftValue ? "true" : "false")});
    const auto rightValue = std::get<bool>(rightResult);
    logger::Logger::dispatchLog(logger::debugLog{
        "Right subtree of operator \"" + binaryOperator->op->lexeme +
        "\" evaluated to: " + (rightValue ? "true" : "false")});
    if (binaryOperator->op->type == util::symbols::CONJUNCT) {
      logger::Logger::dispatchLog(logger::debugLog{"Identified BINARY node \"" +
                                                   binaryOperator->op->lexeme +
                                                   "\" as CONJUNCTION"});
      logger::Logger::dispatchLog(logger::debugLog{
          "Evaluated operator \"" + binaryOperator->op->lexeme +
          "\" to: " + ((leftValue && rightValue) ? "true" : "false")});
      return leftValue && rightValue;
    } else if (binaryOperator->op->type == util::symbols::DISJUNCT) {
      logger::Logger::dispatchLog(logger::debugLog{"Identified BINARY node \"" +
                                                   binaryOperator->op->lexeme +
                                                   "\" as DISJUNCTION"});
      logger::Logger::dispatchLog(logger::debugLog{
          "Evaluated operator \"" + binaryOperator->op->lexeme +
          "\" to: " + ((leftValue || rightValue) ? "true" : "false")});
      return leftValue || rightValue;
    } else if (binaryOperator->op->type == util::symbols::IMPL) {
      logger::Logger::dispatchLog(logger::debugLog{"Identified BINARY node \"" +
                                                   binaryOperator->op->lexeme +
                                                   "\" as IMPLICATION"});
      logger::Logger::dispatchLog(logger::debugLog{
          "Evaluated operator \"" + binaryOperator->op->lexeme +
          "\" to: " + ((leftValue ? rightValue : true) ? "true" : "false")});
      if (leftValue) return rightValue;
      return true;
    }
    return error::eval::unexpected_node{
        "Found unexpected node of type BINARY with lexeme " +
        binaryOperator->op->lexeme + " at position " +
        std::to_string(binaryOperator->op->position)};
  }
  return error::eval::unexpected_node{
      "Found unexpected node of type UNKNOWN. Type: " + root->type};
}

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
evaluateState(parser::parser::AST* ast,
              std::unordered_map<const parser::parser::Node*, bool> state) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking if AST atoms match state atoms"});
  std::unordered_set<const parser::parser::Node*> stateNodes;
  for (auto& x : state) {
    stateNodes.insert(x.first);
  }
  std::unordered_set<const parser::parser::Node*> astAtoms;
  for (auto& x : ast->atoms) {
    astAtoms.insert(x.second);
  }
  if (astAtoms != stateNodes) {
    return error::eval::mismatched_atoms{
        "AST and state atoms do not match. Cannot continue with eval."};
  }

  logger::Logger::dispatchLog(logger::debugLog{
      "AST atoms match state atoms, continuing with evaluation"});
  const auto evalResult = evaluateSubtree(ast->root, state);
  if (std::holds_alternative<error::eval::unexpected_node>(evalResult)) {
    return std::get<error::eval::unexpected_node>(evalResult);
  }
  return std::get<bool>(evalResult);
}

}  // namespace eval
