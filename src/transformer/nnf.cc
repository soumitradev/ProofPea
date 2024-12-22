#include <transformer/nnf.h>

namespace transformer {
namespace nnf {

std::variant<bool, error::eval::unexpected_node> transformToNNFRecursive(
    parser::parser::Node* node) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode = std::get<parser::parser::Absolute*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for NNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode = std::get<parser::parser::Atom*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for NNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::UNARY) {
    const auto parserNode =
        std::get<parser::parser::UnaryOperator*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified UNARY node " +
                                                 parserNode->op->lexeme +
                                                 ", checking child"});

    const auto child = parserNode->child;
    if (child->type == parser::parser::NodeType::ABSOLUTE) {
      const auto childNode = std::get<parser::parser::Absolute*>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child ABSOLUTE node " +
                           childNode->token->lexeme + ", ignoring"});
      return true;
    } else if (child->type == parser::parser::NodeType::ATOM) {
      const auto childNode = std::get<parser::parser::Atom*>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child ATOM node " +
                           childNode->token->lexeme + ", ignoring"});
      return true;
    } else if (child->type == parser::parser::NodeType::UNARY) {
      const auto childNode =
          std::get<parser::parser::UnaryOperator*>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child UNARY node " +
                           childNode->op->lexeme + ", reducing"});
      const auto grandchild = childNode->child;
      delete parserNode;
      delete child;
      node->type = grandchild->type;
      node->node = grandchild->node;
      return true;
    } else if (child->type == parser::parser::NodeType::BINARY) {
      auto childNode = std::get<parser::parser::BinaryOperator*>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child BINARY node " +
                           childNode->op->lexeme + ", reducing"});

      const auto leftNegationToken =
          new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
      const auto leftNegationOp =
          new parser::parser::UnaryOperator{leftNegationToken, childNode->left};
      const auto leftNegationNode =
          new parser::parser::Node{parser::parser::UNARY, leftNegationOp};

      const auto rightNegationToken =
          new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
      const auto rightNegationOp = new parser::parser::UnaryOperator{
          rightNegationToken, childNode->right};
      const auto rightNegationNode =
          new parser::parser::Node{parser::parser::UNARY, rightNegationOp};

      childNode->left = leftNegationNode;
      childNode->right = rightNegationNode;

      const auto prevLexeme = childNode->op->lexeme;
      if (childNode->op->type == util::symbols::DISJUNCT) {
        childNode->op->lexeme = "*";
        childNode->op->type = util::symbols::CONJUNCT;
      } else if (childNode->op->type == util::symbols::CONJUNCT) {
        childNode->op->lexeme = "+";
        childNode->op->type = util::symbols::DISJUNCT;
      } else {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Encountered unexpected IMPL in transformToNNFRecursive"}
        });
        return false;
      }
      delete parserNode;
      node->node = child->node;
      node->type = child->type;

      logger::Logger::dispatchLog(logger::debugLog{
          "Finished simplifying BINARY child node from " + prevLexeme + " to " +
          childNode->op->lexeme + ", checking children for NNF"});
      const auto leftTransformResult = transformToNNFRecursive(childNode->left);
      if (std::holds_alternative<error::eval::unexpected_node>(
              leftTransformResult)) {
        return std::get<error::eval::unexpected_node>(leftTransformResult);
      } else if (std::holds_alternative<bool>(leftTransformResult)) {
        if (!std::get<bool>(leftTransformResult)) return false;
      }

      const auto rightTransformResult =
          transformToNNFRecursive(childNode->right);
      if (std::holds_alternative<error::eval::unexpected_node>(
              rightTransformResult)) {
        return std::get<error::eval::unexpected_node>(rightTransformResult);
      } else if (std::holds_alternative<bool>(rightTransformResult)) {
        return std::get<bool>(rightTransformResult);
      }

      logger::Logger::dispatchLog(logger::errorLog{
        error :
            error::unknown::unknown_error{"Encountered unexpected return value "
                                          "from transformToNNFRecursive when "
                                          "checking BINARY children for NNF"}
      });
      return false;
    }

    logger::Logger::dispatchLog(logger::errorLog{
      error : error::parser::unexpected_token{
          "Encountered unexpected token while checking child node of unary in "
          "NNF"}
    });
    return false;

  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<parser::parser::BinaryOperator*>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified BINARY node " + parserNode->op->lexeme +
                         ", checking children for NNF"});
    const auto leftTransformResult = transformToNNFRecursive(parserNode->left);
    if (std::holds_alternative<error::eval::unexpected_node>(
            leftTransformResult)) {
      return std::get<error::eval::unexpected_node>(leftTransformResult);
    } else if (std::holds_alternative<bool>(leftTransformResult)) {
      if (!std::get<bool>(leftTransformResult)) return false;
    }

    const auto rightTransformResult =
        transformToNNFRecursive(parserNode->right);
    if (std::holds_alternative<error::eval::unexpected_node>(
            rightTransformResult)) {
      return std::get<error::eval::unexpected_node>(rightTransformResult);
    } else if (std::holds_alternative<bool>(rightTransformResult)) {
      return std::get<bool>(rightTransformResult);
    }

    logger::Logger::dispatchLog(logger::errorLog{
      error :
          error::unknown::unknown_error{"Encountered unexpected return value "
                                        "from transformToNNFRecursive"}
    });
    return false;
  }

  logger::Logger::dispatchLog(logger::errorLog{
    error : error::parser::unexpected_token{
        "Encountered unexpected token while transforming to NNF"}
  });
  return false;
}

std::variant<bool, error::eval::unexpected_node> rebuildASTMapsRecursive(
    parser::parser::Node* node, parser::parser::AST* ast) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode = std::get<parser::parser::Absolute*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", adding to map"});
    const auto mapResult = ast->absolutes.find(parserNode->token->lexeme);
    if (mapResult == ast->absolutes.end()) {
      ast->absolutes[parserNode->token->lexeme] = node;
    }
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode = std::get<parser::parser::Atom*>(node->node);
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
        std::get<parser::parser::UnaryOperator*>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified UNARY node " + parserNode->op->lexeme +
                         ", calling rebuild AST maps on child"});
    return rebuildASTMapsRecursive(parserNode->child, ast);
  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<parser::parser::BinaryOperator*>(node->node);
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
    parser::parser::AST* ast) {
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

std::variant<bool, error::eval::unexpected_node> transformToNNF(
    parser::parser::AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Calling IMPL_FREE transformation before running NNF transformer"});
  const auto implFreeResult = transformer::impl_free::transformToIMPLFREE(ast);
  if (std::holds_alternative<error::eval::unexpected_node>(implFreeResult)) {
    return std::get<error::eval::unexpected_node>(implFreeResult);
  } else if (std::holds_alternative<bool>(implFreeResult)) {
    if (!std::get<bool>(implFreeResult)) {
      logger::Logger::dispatchLog(logger::errorLog{
        error : error::unknown::unknown_error{
            "Encountered unexpected error in transformToIMPLFREE"}
      });
      return false;
    }
  }
  logger::Logger::dispatchLog(logger::debugLog{
      "Finished IMPL_FREE transformation, starting NNF transformer"});
  const auto transformResult = transformToNNFRecursive(ast->root);
  if (std::holds_alternative<error::eval::unexpected_node>(transformResult)) {
    return std::get<error::eval::unexpected_node>(transformResult);
  } else if (std::holds_alternative<bool>(transformResult)) {
    if (!std::get<bool>(transformResult)) {
      logger::Logger::dispatchLog(logger::errorLog{
        error : error::unknown::unknown_error{
            "Encountered unexpected error in transformToNNFRecursive"}
      });
      return false;
    }
  }
  logger::Logger::dispatchLog(logger::debugLog{
      "Finished NNF transformer, rebuilding AST token vector"});
  parser::parser::rebuildTokens(ast);
  logger::Logger::dispatchLog(logger::debugLog{
      "Finished rebuilding AST token vector, rebuilding AST maps"});
  const auto rebuildResult = rebuildASTMaps(ast);
  if (std::holds_alternative<error::eval::unexpected_node>(rebuildResult)) {
    return std::get<error::eval::unexpected_node>(rebuildResult);
  } else if (std::holds_alternative<bool>(rebuildResult)) {
    if (!std::get<bool>(rebuildResult)) return false;
  }
  return true;
}

}  // namespace nnf
}  // namespace transformer
