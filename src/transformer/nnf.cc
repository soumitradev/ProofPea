#include <transformer/nnf.h>

namespace transformer {
namespace nnf {

std::variant<bool, error::eval::unexpected_node> transformToNNFRecursive(
    parser::parser::Node* node, parser::parser::AST* ast) {
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
                           childNode->token->lexeme + ", inverting"});
      parser::parser::Node* invertedNode;
      if (childNode->token->type == util::symbols::SymbolType::ABSOLUTETRUE) {
        const auto existing = ast->absolutes.find("0");
        if (existing != ast->absolutes.end()) {
          invertedNode = existing->second;
        } else {
          const auto invertedToken = new parser::tokenizer::Token{
              util::symbols::SymbolType::ABSOLUTEFALSE, "0", 0};
          ast->tokens.push_back(invertedToken);
          const auto invertedAbsolute =
              new parser::parser::Absolute{invertedToken};
          invertedNode = new parser::parser::Node{
              parser::parser::ABSOLUTE, node->parent, invertedAbsolute};
          ast->absolutes["0"] = invertedNode;
        }
      } else if (childNode->token->type ==
                 util::symbols::SymbolType::ABSOLUTEFALSE) {
        const auto existing = ast->absolutes.find("1");
        if (existing != ast->absolutes.end()) {
          invertedNode = existing->second;
        } else {
          const auto invertedToken = new parser::tokenizer::Token{
              util::symbols::SymbolType::ABSOLUTETRUE, "1", 0};
          ast->tokens.push_back(invertedToken);
          const auto invertedAbsolute =
              new parser::parser::Absolute{invertedToken};
          invertedNode = new parser::parser::Node{
              parser::parser::ABSOLUTE, node->parent, invertedAbsolute};
          ast->absolutes["1"] = invertedNode;
        }
      } else {
        return error::eval::unexpected_node{
            "Encountered unexpected ABSOLUTE child node" +
            childNode->token->lexeme + " in transformToNNFRecursive"};
      }

      if (node->parent == nullptr) {
        ast->root = invertedNode;
        ast->root->parent = nullptr;
      } else {
        if (node->parent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<parser::parser::BinaryOperator*>(node->parent->node);
          if (parent->left == node) parent->left = invertedNode;
          if (parent->right == node) parent->right = invertedNode;
        } else if (node->parent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<parser::parser::UnaryOperator*>(node->parent->node);
          if (parent->child == node) parent->child = invertedNode;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }
      delete parserNode;
      delete node;
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
      if (node->parent == nullptr) {
        ast->root = grandchild;
        ast->root->parent = nullptr;
      } else {
        if (node->parent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<parser::parser::BinaryOperator*>(node->parent->node);
          if (parent->left == node) parent->left = grandchild;
          if (parent->right == node) parent->right = grandchild;
        } else if (node->parent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<parser::parser::UnaryOperator*>(node->parent->node);
          if (parent->child == node) parent->child = grandchild;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }
      delete parserNode;
      delete node;
      delete childNode;
      delete child;
      return transformToNNFRecursive(grandchild, ast);
    } else if (child->type == parser::parser::NodeType::BINARY) {
      auto childNode = std::get<parser::parser::BinaryOperator*>(child->node);
      logger::Logger::dispatchLog(
          logger::debugLog{"Identified child BINARY node " +
                           childNode->op->lexeme + ", reducing"});

      const auto leftNegationToken =
          new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
      ast->tokens.push_back(leftNegationToken);
      const auto leftNegationOp =
          new parser::parser::UnaryOperator{leftNegationToken, childNode->left};
      const auto leftNegationNode = new parser::parser::Node{
          parser::parser::UNARY, child, leftNegationOp};
      childNode->left->parent = leftNegationNode;

      const auto rightNegationToken =
          new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
      ast->tokens.push_back(rightNegationToken);
      const auto rightNegationOp = new parser::parser::UnaryOperator{
          rightNegationToken, childNode->right};
      const auto rightNegationNode = new parser::parser::Node{
          parser::parser::UNARY, child, rightNegationOp};
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

      if (node->parent == nullptr) {
        ast->root = child;
        ast->root->parent = nullptr;
      } else {
        if (node->parent->type == parser::parser::NodeType::BINARY) {
          const auto parent =
              std::get<parser::parser::BinaryOperator*>(node->parent->node);
          if (parent->left == node) parent->left = child;
          if (parent->right == node) parent->right = child;
        } else if (node->parent->type == parser::parser::NodeType::UNARY) {
          const auto parent =
              std::get<parser::parser::UnaryOperator*>(node->parent->node);
          if (parent->child == node) parent->child = child;
        } else {
          return error::eval::unexpected_node{
              "Encountered unexpected parent node of " +
              parserNode->op->lexeme + " in transformToNNFRecursive"};
        }
      }
      delete parserNode;
      delete node;

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
        std::get<parser::parser::BinaryOperator*>(node->node);
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
  logger::Logger::dispatchLog(logger::debugLog{
      "Finished NNF transformer, rebuilding AST token vector"});
  parser::parser::rebuildTokens(ast);
  logger::Logger::dispatchLog(logger::debugLog{
      "Finished rebuilding AST token vector, rebuilding AST maps"});
  const auto rebuildResult = common::rebuildASTMaps(ast);
  if (std::holds_alternative<error::eval::unexpected_node>(rebuildResult)) {
    return std::get<error::eval::unexpected_node>(rebuildResult);
  } else if (std::holds_alternative<bool>(rebuildResult)) {
    if (!std::get<bool>(rebuildResult)) return false;
  }
  return true;
}

}  // namespace nnf
}  // namespace transformer
