#include <transformer/common.h>

namespace transformer {
namespace common {

std::variant<bool, error::eval::unexpected_node> rebuildASTMapsRecursive(
    const std::shared_ptr<parser::parser::Node> node,
    std::shared_ptr<parser::parser::AST> ast) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::Absolute>>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", adding to map"});
    const auto mapResult = ast->absolutes.find(parserNode->token->lexeme);
    if (mapResult == ast->absolutes.end()) {
      ast->absolutes[parserNode->token->lexeme] = node;
    }
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::Atom>>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", adding to map"});
    const auto mapResult = ast->atoms.find(parserNode->token->lexeme);
    if (mapResult == ast->atoms.end()) {
      ast->atoms[parserNode->token->lexeme] = node;
    }
    return true;
  } else if (node->type == parser::parser::NodeType::UNARY) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::UnaryOperator>>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified UNARY node " + parserNode->op->lexeme +
                         ", calling rebuild AST maps on child"});
    return rebuildASTMapsRecursive(parserNode->child, ast);
  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified BINARY node " + parserNode->op->lexeme +
                         ", calling rebuild AST maps on children"});
    logger::Logger::dispatchLog(
        logger::debugLog{"Calling rebuild AST maps on left child"});
    const auto leftRebuildResult =
        rebuildASTMapsRecursive(parserNode->left, ast);
    if (std::holds_alternative<error::eval::unexpected_node>(
            leftRebuildResult)) {
      return std::get<error::eval::unexpected_node>(leftRebuildResult);
    } else if (std::holds_alternative<bool>(leftRebuildResult)) {
      if (!std::get<bool>(leftRebuildResult)) return false;
    }

    logger::Logger::dispatchLog(
        logger::debugLog{"Calling rebuild AST maps on right child"});
    const auto rightRebuildResult =
        rebuildASTMapsRecursive(parserNode->right, ast);
    if (std::holds_alternative<error::eval::unexpected_node>(
            rightRebuildResult)) {
      return std::get<error::eval::unexpected_node>(rightRebuildResult);
    } else if (std::holds_alternative<bool>(rightRebuildResult)) {
      return std::get<bool>(rightRebuildResult);
    }
  }

  logger::Logger::dispatchLog(logger::errorLog{
    error : error::parser::unexpected_token{
        "Encountered unexpected token while rebuilding AST maps"}
  });
  return false;
}

std::variant<bool, error::eval::unexpected_node> rebuildASTMaps(
    std::shared_ptr<parser::parser::AST> ast) {
  logger::Logger::dispatchLog(logger::debugLog{"Clearing old maps"});
  ast->absolutes.clear();
  ast->atoms.clear();
  logger::Logger::dispatchLog(logger::debugLog{
      "Calling recursive function to start rebuilding AST maps"});
  const auto rebuildResult = rebuildASTMapsRecursive(ast->root, ast);
  if (std::holds_alternative<error::eval::unexpected_node>(rebuildResult)) {
    return std::get<error::eval::unexpected_node>(rebuildResult);
  } else if (std::holds_alternative<bool>(rebuildResult)) {
    if (!std::get<bool>(rebuildResult)) return false;
  }
  logger::Logger::dispatchLog(logger::debugLog{"Finished rebuilding AST maps"});
  return true;
}

}  // namespace common
}  // namespace transformer
