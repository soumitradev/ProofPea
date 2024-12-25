#include <transformer/nnf.h>

namespace proofpea {
namespace transformer {
namespace nnf {

std::variant<bool, error::eval::unexpected_node> transformToNNFRecursive(
    std::shared_ptr<parser::parser::Node> node,
    std::shared_ptr<parser::parser::AST> ast) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::Absolute>>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for NNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::Atom>>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for NNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::UNARY) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::UnaryOperator>>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified UNARY node " +
                                                 parserNode->op->lexeme +
                                                 ", checking child"});

    const auto child = parserNode->child;
    if (child->type == parser::parser::NodeType::ABSOLUTE) {
      const auto childNode =
          std::get<std::shared_ptr<parser::parser::Absolute>>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child ABSOLUTE node " +
                           childNode->token->lexeme + ", inverting"});
      std::shared_ptr<parser::parser::Node> invertedNode;
      if (childNode->token->type == util::symbols::SymbolType::ABSOLUTETRUE) {
        const auto existing = ast->absolutes.find("0");
        if (existing != ast->absolutes.end()) {
          invertedNode = existing->second;
        } else {
          const auto invertedToken = std::make_shared<parser::tokenizer::Token>(
              util::symbols::SymbolType::ABSOLUTEFALSE, "0", 0);
          ast->tokens.push_back(invertedToken);
          const auto invertedNode = std::make_shared<parser::parser::Node>(
              parser::parser::ABSOLUTE, node->parent,
              std::make_shared<parser::parser::Absolute>(invertedToken));
          ast->absolutes["0"] = invertedNode;
        }
      } else if (childNode->token->type ==
                 util::symbols::SymbolType::ABSOLUTEFALSE) {
        const auto existing = ast->absolutes.find("1");
        if (existing != ast->absolutes.end()) {
          invertedNode = existing->second;
        } else {
          const auto invertedToken = std::make_shared<parser::tokenizer::Token>(
              util::symbols::SymbolType::ABSOLUTETRUE, "1", 0);
          ast->tokens.push_back(invertedToken);
          const auto invertedNode = std::make_shared<parser::parser::Node>(
              parser::parser::ABSOLUTE, node->parent,
              std::make_shared<parser::parser::Absolute>(invertedToken));
          ast->absolutes["1"] = invertedNode;
        }
      } else {
        return error::eval::unexpected_node{
            "Encountered unexpected ABSOLUTE child node" +
            childNode->token->lexeme + " in transformToNNFRecursive"};
      }

      const auto nodeParent = node->parent.lock();
      if (!nodeParent) {
        ast->root = invertedNode;
        ast->root->parent.reset();
      } else {
        if (nodeParent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
                  nodeParent->node);
          if (parent->left == node) parent->left = invertedNode;
          if (parent->right == node) parent->right = invertedNode;
        } else if (nodeParent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::UnaryOperator>>(
                  nodeParent->node);
          if (parent->child == node) parent->child = invertedNode;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }
      return true;
    } else if (child->type == parser::parser::NodeType::ATOM) {
      const auto childNode =
          std::get<std::shared_ptr<parser::parser::Atom>>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child ATOM node " +
                           childNode->token->lexeme + ", ignoring"});
      return true;
    } else if (child->type == parser::parser::NodeType::UNARY) {
      const auto childNode =
          std::get<std::shared_ptr<parser::parser::UnaryOperator>>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child UNARY node " +
                           childNode->op->lexeme + ", reducing"});

      const auto grandchild = childNode->child;
      const auto nodeParent = node->parent.lock();
      if (!nodeParent) {
        ast->root = grandchild;
        ast->root->parent.reset();
      } else {
        if (nodeParent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
                  nodeParent->node);
          if (parent->left == node) parent->left = grandchild;
          if (parent->right == node) parent->right = grandchild;
        } else if (nodeParent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::UnaryOperator>>(
                  nodeParent->node);
          if (parent->child == node) parent->child = grandchild;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }
      grandchild->parent = node->parent;
      return transformToNNFRecursive(grandchild, ast);
    } else if (child->type == parser::parser::NodeType::BINARY) {
      auto childNode =
          std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
              child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child BINARY node " +
                           childNode->op->lexeme + ", reducing"});

      const auto leftNegationToken = std::make_shared<parser::tokenizer::Token>(
          util::symbols::NEG, "~", 0);
      ast->tokens.push_back(leftNegationToken);
      const auto leftNegationNode = std::make_shared<parser::parser::Node>(
          parser::parser::UNARY, child,
          std::make_shared<parser::parser::UnaryOperator>(leftNegationToken,
                                                          childNode->left));
      childNode->left->parent = leftNegationNode;

      const auto rightNegationToken =
          std::make_shared<parser::tokenizer::Token>(util::symbols::NEG, "~",
                                                     0);
      ast->tokens.push_back(rightNegationToken);
      const auto rightNegationNode = std::make_shared<parser::parser::Node>(
          parser::parser::UNARY, child,
          std::make_shared<parser::parser::UnaryOperator>(rightNegationToken,
                                                          childNode->right));
      childNode->right->parent = rightNegationNode;

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
        return error::eval::unexpected_node{"Encountered unexpected node " +
                                            childNode->op->lexeme +
                                            " in transformToNNFRecursive"};
      }

      const auto nodeParent = node->parent.lock();
      if (!nodeParent) {
        ast->root = child;
        ast->root->parent.reset();
      } else {
        if (nodeParent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
                  nodeParent->node);
          if (parent->left == node) parent->left = child;
          if (parent->right == node) parent->right = child;
        } else if (nodeParent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<std::shared_ptr<parser::parser::UnaryOperator>>(
                  nodeParent->node);
          if (parent->child == node) parent->child = child;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }

      logger::Logger::dispatchLog(logger::debugLog{
          "Finished simplifying BINARY child node from " + prevLexeme + " to " +
          childNode->op->lexeme + ", checking children for NNF"});
      const auto leftTransformResult =
          transformToNNFRecursive(childNode->left, ast);
      if (std::holds_alternative<error::eval::unexpected_node>(
              leftTransformResult)) {
        return std::get<error::eval::unexpected_node>(leftTransformResult);
      } else if (std::holds_alternative<bool>(leftTransformResult)) {
        if (!std::get<bool>(leftTransformResult)) return false;
      }

      const auto rightTransformResult =
          transformToNNFRecursive(childNode->right, ast);
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
    return error::eval::unexpected_node{
        "Encountered unknown type of child node in transformToNNFRecursive"};

  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified BINARY node " + parserNode->op->lexeme +
                         ", checking children for NNF"});
    const auto leftTransformResult =
        transformToNNFRecursive(parserNode->left, ast);
    if (std::holds_alternative<error::eval::unexpected_node>(
            leftTransformResult)) {
      return std::get<error::eval::unexpected_node>(leftTransformResult);
    } else if (std::holds_alternative<bool>(leftTransformResult)) {
      if (!std::get<bool>(leftTransformResult)) return false;
    }

    const auto rightTransformResult =
        transformToNNFRecursive(parserNode->right, ast);
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

  return error::eval::unexpected_node{
      "Encountered unknown type of node " +
      util::symbols::SymbolTypeArray[node->type] +
      " in transformToCNFRecursive"};
}

std::variant<bool, error::eval::unexpected_node> transformToNNF(
    std::shared_ptr<parser::parser::AST> ast) {
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
  const auto transformResult = transformToNNFRecursive(ast->root, ast);
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
  return true;
}

}  // namespace nnf
}  // namespace transformer
}  // namespace proofpea
