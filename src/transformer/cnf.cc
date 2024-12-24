#include <transformer/cnf.h>

namespace transformer {
namespace cnf {

bool distributivityTransform(parser::parser::Node* node,
                             parser::parser::AST* ast) {
  const auto nodeOp = std::get<parser::parser::BinaryOperator*>(node->node);
  if (nodeOp->left->type == parser::parser::NodeType::BINARY) {
    const auto leftNode =
        std::get<parser::parser::BinaryOperator*>(nodeOp->left->node);
    if (leftNode->op->type == util::symbols::CONJUNCT) {
      nodeOp->op->type = util::symbols::CONJUNCT;
      nodeOp->op->lexeme = "*";

      const auto rightCopy = parser::parser::AST::copyNode(
          nodeOp->right, ast->tokens, ast->atoms, ast->absolutes);
      if (rightCopy == nullptr) {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Unknown error while copying right node in "
              "distributivityTransform"}
        });
        return false;
      }
      leftNode->op->type = util::symbols::DISJUNCT;
      leftNode->op->lexeme = "+";
      const auto leftRightChild = leftNode->right;
      rightCopy->parent = nodeOp->left;
      leftNode->right = rightCopy;

      const auto newRightToken = std::make_shared<parser::tokenizer::Token>(
          util::symbols::DISJUNCT, "+", 0);
      const auto newRightOp = new parser::parser::BinaryOperator{
          newRightToken, leftRightChild, nodeOp->right};
      const auto newRightNode =
          new parser::parser::Node{parser::parser::BINARY, node, newRightOp};
      ast->tokens.push_back(newRightToken);
      nodeOp->right->parent = newRightNode;
      nodeOp->right = newRightNode;
      leftRightChild->parent = newRightNode;

      const auto leftTransformResult =
          distributivityTransform(nodeOp->left, ast);
      if (!leftTransformResult) return false;

      const auto rightTransformResult =
          distributivityTransform(newRightNode, ast);
      return rightTransformResult;
    }
  }

  if (nodeOp->right->type == parser::parser::NodeType::BINARY) {
    const auto rightNode =
        std::get<parser::parser::BinaryOperator*>(nodeOp->right->node);
    if (rightNode->op->type == util::symbols::CONJUNCT) {
      nodeOp->op->type = util::symbols::CONJUNCT;
      nodeOp->op->lexeme = "*";

      const auto leftCopy = parser::parser::AST::copyNode(
          nodeOp->left, ast->tokens, ast->atoms, ast->absolutes);
      if (leftCopy == nullptr) {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Unknown error while copying left node in "
              "distributivityTransform"}
        });
        return false;
      }
      rightNode->op->type = util::symbols::DISJUNCT;
      rightNode->op->lexeme = "+";
      const auto rightLeftChild = rightNode->left;
      leftCopy->parent = nodeOp->right;
      rightNode->left = leftCopy;

      const auto newLeftToken = std::make_shared<parser::tokenizer::Token>(
          util::symbols::DISJUNCT, "+", 0);
      ast->tokens.push_back(newLeftToken);
      const auto newLeftOp = new parser::parser::BinaryOperator{
          newLeftToken, rightLeftChild, nodeOp->left};
      const auto newLeftNode =
          new parser::parser::Node{parser::parser::BINARY, node, newLeftOp};
      nodeOp->left->parent = newLeftNode;
      nodeOp->left = newLeftNode;
      rightLeftChild->parent = newLeftNode;

      const auto leftTransformResult =
          distributivityTransform(newLeftNode, ast);
      if (!leftTransformResult) return false;

      const auto rightTransformResult =
          distributivityTransform(nodeOp->right, ast);
      return rightTransformResult;
    }
  }

  return true;
}

std::variant<bool, error::eval::unexpected_node> transformToCNFRecursive(
    parser::parser::Node* node, parser::parser::AST* ast) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode = std::get<parser::parser::Absolute*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for CNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode = std::get<parser::parser::Atom*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for CNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::UNARY) {
    const auto parserNode =
        std::get<parser::parser::UnaryOperator*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified UNARY node " +
                                                 parserNode->op->lexeme +
                                                 ", ignoring for CNF"});
    return true;
  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<parser::parser::BinaryOperator*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified BINARY node " +
                                                 parserNode->op->lexeme +
                                                 " while checking for CNF"});
    if (parserNode->op->type == util::symbols::CONJUNCT) {
      const auto leftTransformResult =
          transformToCNFRecursive(parserNode->left, ast);
      if (std::holds_alternative<error::eval::unexpected_node>(
              leftTransformResult)) {
        return std::get<error::eval::unexpected_node>(leftTransformResult);
      } else if (std::holds_alternative<bool>(leftTransformResult)) {
        if (!std::get<bool>(leftTransformResult)) return false;
      }

      const auto rightTransformResult =
          transformToCNFRecursive(parserNode->right, ast);
      if (std::holds_alternative<error::eval::unexpected_node>(
              rightTransformResult)) {
        return std::get<error::eval::unexpected_node>(rightTransformResult);
      } else if (std::holds_alternative<bool>(rightTransformResult)) {
        return std::get<bool>(rightTransformResult);
      }

      logger::Logger::dispatchLog(logger::errorLog{
        error :
            error::unknown::unknown_error{"Encountered unexpected return value "
                                          "from transformToCNFRecursive"}
      });
      return false;
    } else if (parserNode->op->type == util::symbols::DISJUNCT) {
      const auto leftTransformResult =
          transformToCNFRecursive(parserNode->left, ast);
      if (std::holds_alternative<error::eval::unexpected_node>(
              leftTransformResult)) {
        return std::get<error::eval::unexpected_node>(leftTransformResult);
      } else if (std::holds_alternative<bool>(leftTransformResult)) {
        if (!std::get<bool>(leftTransformResult)) return false;
      } else {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Encountered unexpected return value "
              "from transformToCNFRecursive"}
        });
        return false;
      }

      const auto rightTransformResult =
          transformToCNFRecursive(parserNode->right, ast);
      if (std::holds_alternative<error::eval::unexpected_node>(
              rightTransformResult)) {
        return std::get<error::eval::unexpected_node>(rightTransformResult);
      } else if (std::holds_alternative<bool>(rightTransformResult)) {
        if (!std::get<bool>(rightTransformResult)) return false;
      } else {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Encountered unexpected return value "
              "from transformToCNFRecursive"}
        });
        return false;
      }

      const auto distrTransformResult = distributivityTransform(node, ast);
      if (!distrTransformResult) {
        logger::Logger::dispatchLog(logger::errorLog{
          error : error::unknown::unknown_error{
              "Encountered unexpected return value "
              "from distributivityTransform"}
        });
        return false;
      }
      return distrTransformResult;
    }

    return error::eval::unexpected_node{"Encountered unexpected node " +
                                        parserNode->op->lexeme +
                                        " in transformToCNFRecursive"};
  }

  return error::eval::unexpected_node{
      "Encountered unknown type of node " +
      util::symbols::SymbolTypeArray[node->type] +
      " in transformToCNFRecursive"};
}

std::variant<bool, error::eval::unexpected_node> transformToCNF(
    parser::parser::AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Calling NNF transformation before running CNF transformer"});
  const auto nnfResult = transformer::nnf::transformToNNF(ast);
  if (std::holds_alternative<error::eval::unexpected_node>(nnfResult)) {
    return std::get<error::eval::unexpected_node>(nnfResult);
  } else if (std::holds_alternative<bool>(nnfResult)) {
    if (!std::get<bool>(nnfResult)) {
      logger::Logger::dispatchLog(logger::errorLog{
        error : error::unknown::unknown_error{
            "Encountered unexpected error in transformToNNF"}
      });
      return false;
    }
  }

  logger::Logger::dispatchLog(logger::debugLog{
      "Finished NNF transformation, starting CNF transformer"});
  const auto transformResult = transformToCNFRecursive(ast->root, ast);
  if (std::holds_alternative<error::eval::unexpected_node>(transformResult)) {
    return std::get<error::eval::unexpected_node>(transformResult);
  } else if (std::holds_alternative<bool>(transformResult)) {
    if (!std::get<bool>(transformResult)) {
      logger::Logger::dispatchLog(logger::errorLog{
        error : error::unknown::unknown_error{
            "Encountered unexpected error in transformToCNFRecursive"}
      });
      return false;
    }
  }
  return true;
}

}  // namespace cnf
}  // namespace transformer
