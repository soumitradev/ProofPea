#include <parser/parser.h>

namespace parser {
namespace parser {

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
primary(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for primary token at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});

  if (tokenPtr->type == util::symbols::ATOM) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Detected atom " + tokenPtr->lexeme + " at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});

    const auto atomNodePtr = ast->atoms.find(tokenPtr->lexeme);
    if (atomNodePtr != ast->atoms.end()) {
      logger::Logger::dispatchLog(
          logger::debugLog{"Found atom " + tokenPtr->lexeme + " in hash-map"});
      return std::make_pair(atomNodePtr->second, tokenPtr + 1);
    }

    logger::Logger::dispatchLog(
        logger::debugLog{"Did not find atom " + tokenPtr->lexeme +
                         " in hash-map, creating and inserting"});

    const auto atom = new Atom{tokenPtr.base()};
    const auto atomNode = new Node{parser::ATOM, atom};
    ast->atoms[tokenPtr->lexeme] = atomNode;

    return std::make_pair(atomNode, tokenPtr + 1);
  } else if (tokenPtr->type == util::symbols::ABSOLUTETRUE ||
             tokenPtr->type == util::symbols::ABSOLUTEFALSE) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Detected absolute " + tokenPtr->lexeme + " at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});

    const auto absolutePtr = ast->absolutes.find(tokenPtr->lexeme);
    if (absolutePtr != ast->absolutes.end()) {
      logger::Logger::dispatchLog(logger::debugLog{
          "Found absolute " + tokenPtr->lexeme + " in hash-map"});
      return std::make_pair(absolutePtr->second, tokenPtr + 1);
    }

    logger::Logger::dispatchLog(
        logger::debugLog{"Did not find absolute " + tokenPtr->lexeme +
                         " in hash-map, creating and inserting"});

    const auto absolute = new Absolute{tokenPtr.base()};
    const auto absoluteNode = new Node{parser::ABSOLUTE, absolute};
    ast->absolutes[tokenPtr->lexeme] = absoluteNode;

    return std::make_pair(absoluteNode, tokenPtr + 1);
  }

  if (tokenPtr->type == util::symbols::LBRACE) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Detected grouping at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});

    const auto groupingResult = expression(tokenPtr + 1, ast);
    if (std::holds_alternative<error::parser::unexpected_token>(
            groupingResult)) {
      const auto groupingError =
          std::get<error::parser::unexpected_token>(groupingResult);
      return groupingError;
    }

    const auto groupedExpr = std::get<
        std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
        groupingResult);
    if (groupedExpr.second < ast->tokens.end()) {
      if (groupedExpr.second->type != util::symbols::RBRACE) {
        logger::Logger::dispatchLog(logger::errorLog{error::error{
            "Expected \")\" after expression with \"(\", but found \"" +
                groupedExpr.second->lexeme + "\"",
            "Invalid Grouping syntax"}});
      }
      return std::make_pair(groupedExpr.first, groupedExpr.second + 1);
    } else {
      logger::Logger::dispatchLog(logger::errorLog{
          error::error{"Expected \")\" after expression with \"(\", but "
                       "expression terminated prematurely",
                       "Invalid Grouping syntax"}});
      return groupedExpr;
    }
  }
  return error::parser::unexpected_token{
      "Expected primary token at position " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr)) +
      ", found " + tokenPtr->lexeme};
}

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
negation(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for negation at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  if (tokenPtr->type == util::symbols::NEG) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found negation at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
    auto negationResult = negation(tokenPtr + 1, ast);
    if (std::holds_alternative<error::parser::unexpected_token>(
            negationResult)) {
      const auto negationError =
          std::get<error::parser::unexpected_token>(negationResult);
      return negationError;
    }

    const auto negationExpr = std::get<
        std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
        negationResult);
    const auto operatorStruct =
        new UnaryOperator{tokenPtr.base(), negationExpr.first};
    const auto expr = new Node{parser::UNARY, operatorStruct};
    return std::make_pair(expr, negationExpr.second);
  }

  return primary(tokenPtr, ast);
}

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
conjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for conjunction at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  const auto negationResult = negation(tokenPtr, ast);
  if (std::holds_alternative<error::parser::unexpected_token>(negationResult)) {
    const auto negationError =
        std::get<error::parser::unexpected_token>(negationResult);
    return negationError;
  }

  const auto negationExpr = std::get<
      std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
      negationResult);
  auto expr = negationExpr.first;
  tokenPtr = negationExpr.second;

  while (tokenPtr < ast->tokens.end() &&
         tokenPtr->type == util::symbols::CONJUNCT) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found conjunction at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
    const auto rightNegationResult = negation(tokenPtr + 1, ast);
    if (std::holds_alternative<error::parser::unexpected_token>(
            rightNegationResult)) {
      const auto rightNegationError =
          std::get<error::parser::unexpected_token>(rightNegationResult);
      return rightNegationResult;
    }

    const auto rightNegationExpr = std::get<
        std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
        rightNegationResult);
    const auto operatorStruct =
        new BinaryOperator{tokenPtr.base(), expr, rightNegationExpr.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = rightNegationExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
disjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for disjunction at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  const auto conjunctionResult = conjunction(tokenPtr, ast);
  if (std::holds_alternative<error::parser::unexpected_token>(
          conjunctionResult)) {
    const auto conjunctionError =
        std::get<error::parser::unexpected_token>(conjunctionResult);
    return conjunctionError;
  }

  const auto conjunctionExpr = std::get<
      std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
      conjunctionResult);
  auto expr = conjunctionExpr.first;
  tokenPtr = conjunctionExpr.second;

  while (tokenPtr < ast->tokens.end() &&
         tokenPtr->type == util::symbols::DISJUNCT) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found disjunction at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
    const auto rightConjunctionResult = conjunction(tokenPtr + 1, ast);
    if (std::holds_alternative<error::parser::unexpected_token>(
            rightConjunctionResult)) {
      const auto rightConjunctionError =
          std::get<error::parser::unexpected_token>(rightConjunctionResult);
      return rightConjunctionError;
    }

    const auto rightConjunctionExpr = std::get<
        std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
        rightConjunctionResult);
    const auto operatorStruct =
        new BinaryOperator{tokenPtr.base(), expr, rightConjunctionExpr.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = rightConjunctionExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
implication(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for implication at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  const auto disjunctionResult = disjunction(tokenPtr, ast);
  if (std::holds_alternative<error::parser::unexpected_token>(
          disjunctionResult)) {
    const auto disjunctionError =
        std::get<error::parser::unexpected_token>(disjunctionResult);
    return disjunctionError;
  }

  const auto disjunctionExpr = std::get<
      std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
      disjunctionResult);
  auto expr = disjunctionExpr.first;
  tokenPtr = disjunctionExpr.second;

  if (tokenPtr < ast->tokens.end() && tokenPtr->type == util::symbols::IMPL) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found implication at " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
    auto op = tokenPtr.base();
    const auto implicationResult = implication(tokenPtr + 1, ast);
    if (std::holds_alternative<error::parser::unexpected_token>(
            implicationResult)) {
      const auto implicationError =
          std::get<error::parser::unexpected_token>(implicationResult);
      return implicationError;
    }
    const auto implicationExpr = std::get<
        std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
        implicationResult);
    const auto operatorStruct =
        new BinaryOperator{op, disjunctionExpr.first, implicationExpr.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = implicationExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<
    std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
expression(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for expression at " +
      std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  return implication(tokenPtr, ast);
}

std::variant<AST*, error::parser::unexpected_token> parseAST(
    const std::vector<tokenizer::Token>& tokens) {
  struct AST* ast = new AST(tokens);
  logger::Logger::dispatchLog(logger::infoLog{"Starting to parse AST"});
  const auto tokenPtr = ast->tokens.cbegin();
  ast->atoms.clear();

  const auto exprResult = expression(tokenPtr, ast);
  if (std::holds_alternative<error::parser::unexpected_token>(exprResult)) {
    const auto exprError =
        std::get<error::parser::unexpected_token>(exprResult);
    return exprError;
  }

  const auto expr = std::get<
      std::pair<const Node*, std::vector<tokenizer::Token>::const_iterator>>(
      exprResult);

  if (expr.second != ast->tokens.end()) {
    logger::Logger::dispatchLog(logger::warnLog{warning::warning{
        "tokenPtr in parser not at end of vector after parsing",
        "unknown cause"}});
    logger::Logger::dispatchLog(logger::debugLog{
        "tokenPtr at position " +
        std::to_string(std::distance(ast->tokens.cbegin(), tokenPtr))});
  }

  logger::Logger::dispatchLog(logger::infoLog{"AST successfully parsed"});
  ast->root = expr.first;
  return ast;
}

void deallocNodeRecursive(const Node* root,
                          std::unordered_set<const Node*>& deletedAtoms) {
  if (root->type == parser::ATOM) {
    auto ptr = std::get<const parser::Atom*>(root->node);
    delete ptr;
  } else if (root->type == parser::ABSOLUTE) {
    auto ptr = std::get<const parser::Absolute*>(root->node);
    delete ptr;
  } else if (root->type == parser::BINARY) {
    auto ptr = std::get<const parser::BinaryOperator*>(root->node);
    if (deletedAtoms.find(ptr->left) == deletedAtoms.end()) {
      deallocNodeRecursive(ptr->left, deletedAtoms);
      deletedAtoms.insert(ptr->left);
    }
    if (deletedAtoms.find(ptr->right) == deletedAtoms.end()) {
      deallocNodeRecursive(ptr->right, deletedAtoms);
      deletedAtoms.insert(ptr->right);
    }
    delete ptr;
  } else if (root->type == parser::UNARY) {
    auto ptr = std::get<const parser::UnaryOperator*>(root->node);
    if (deletedAtoms.find(ptr->child) == deletedAtoms.end()) {
      deallocNodeRecursive(ptr->child, deletedAtoms);
      deletedAtoms.insert(ptr->child);
    }
    delete ptr;
  }
  delete root;
}

void deallocAST(AST* ast) {
  std::unordered_set<const Node*> deletedAtoms;
  deletedAtoms.clear();
  deallocNodeRecursive(ast->root, deletedAtoms);
  deletedAtoms.clear();
  ast->atoms.clear();
  ast->absolutes.clear();
  ast->tokens.clear();
  delete ast;
}
}  // namespace parser
}  // namespace parser
