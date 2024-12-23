#include <transformer/impl_free.h>

namespace transformer {
namespace impl_free {

std::variant<bool, error::eval::unexpected_node> transformToIMPLFREERecursive(
    parser::parser::Node* node, parser::parser::AST* ast) {
  if (node->type == parser::parser::NodeType::ABSOLUTE) {
    const auto parserNode = std::get<parser::parser::Absolute*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for IMPL_FREE"});
    return true;
  } else if (node->type == parser::parser::NodeType::ATOM) {
    const auto parserNode = std::get<parser::parser::Atom*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", ignoring for IMPL_FREE"});
    return true;
  } else if (node->type == parser::parser::NodeType::UNARY) {
    const auto parserNode =
        std::get<parser::parser::UnaryOperator*>(node->node);
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified UNARY node " + parserNode->op->lexeme +
                         ", checking children for IMPL_FREE"});
    return transformToIMPLFREERecursive(parserNode->child, ast);
  } else if (node->type == parser::parser::NodeType::BINARY) {
    const auto parserNode =
        std::get<parser::parser::BinaryOperator*>(node->node);
    // If the binary operator is IMPL, swap out the token and add a negation
    // node on the left child
    if (parserNode->op->type == util::symbols::IMPL) {
      logger::Logger::dispatchLog(
          logger::debugLog{"BINARY node " + parserNode->op->lexeme +
                           " is IMPL node, replacing for IMPL_FREE"});
      parserNode->op->type = util::symbols::DISJUNCT;
      parserNode->op->lexeme = "+";

      const auto negationToken =
          new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
      ast->tokens.push_back(negationToken);
      const auto negationOp =
          new parser::parser::UnaryOperator{negationToken, parserNode->left};
      const auto negationNode =
          new parser::parser::Node{parser::parser::UNARY, node, negationOp};
      parserNode->left->parent = negationNode;
      parserNode->left = negationNode;
    }
    logger::Logger::dispatchLog(
        logger::debugLog{"Identified BINARY node " + parserNode->op->lexeme +
                         ", checking children for IMPL_FREE"});
    const auto leftTransformResult =
        transformToIMPLFREERecursive(parserNode->left, ast);
    if (std::holds_alternative<error::eval::unexpected_node>(
            leftTransformResult)) {
      return std::get<error::eval::unexpected_node>(leftTransformResult);
    } else if (std::holds_alternative<bool>(leftTransformResult)) {
      if (!std::get<bool>(leftTransformResult)) return false;
    }

    const auto rightTransformResult =
        transformToIMPLFREERecursive(parserNode->right, ast);
    if (std::holds_alternative<error::eval::unexpected_node>(
            rightTransformResult)) {
      return std::get<error::eval::unexpected_node>(rightTransformResult);
    } else if (std::holds_alternative<bool>(rightTransformResult)) {
      return std::get<bool>(rightTransformResult);
    }

    logger::Logger::dispatchLog(logger::errorLog{
      error :
          error::unknown::unknown_error{"Encountered unexpected return value "
                                        "from transformToIMPLFREERecursive"}
    });
    return false;
  }

  logger::Logger::dispatchLog(logger::errorLog{
    error : error::parser::unexpected_token{
        "Encountered unexpected token while transforming to IMPL_FREE"}
  });
  return false;
}

std::variant<bool, error::eval::unexpected_node> transformToIMPLFREE(
    parser::parser::AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Calling recursive function to start IMPL_FREE transformation"});
  const auto transformResult = transformToIMPLFREERecursive(ast->root, ast);

  if (std::holds_alternative<error::eval::unexpected_node>(transformResult)) {
    return std::get<error::eval::unexpected_node>(transformResult);
  } else if (std::holds_alternative<bool>(transformResult)) {
    if (!std::get<bool>(transformResult)) return false;
  } else {
    logger::Logger::dispatchLog(logger::errorLog{
      error :
          error::unknown::unknown_error{"Encountered unexpected return value "
                                        "from transformToIMPLFREERecursive"}
    });
    return false;
  }

  return true;
}

}  // namespace impl_free
}  // namespace transformer
