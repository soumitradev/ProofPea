#include <parser/parser.h>

namespace parser {

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> primary(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for primary token at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});

  if (tokenPtr->type == util::symbols::ATOM) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Detected atom " + tokenPtr->lexeme + " at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});

    const auto atom = new Atom{tokenPtr.base()};
    const auto atomNode = new Node{parser::ATOM, atom};

    return std::make_pair(atomNode, tokenPtr + 1);
  }

  if (tokenPtr->type == util::symbols::LBRACE) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Detected grouping at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});

    const auto groupingResult = expression(tokens, tokenPtr + 1);
    if (groupingResult.second < tokens.end()) {
      if (groupingResult.second->type != util::symbols::RBRACE) {
        logger::Logger::dispatchLog(logger::errorLog{error::error{
            "Expected \")\" after expression with \"(\", but found \"" +
                groupingResult.second->lexeme + "\"",
            "Invalid Grouping syntax"}});
      }
      return std::make_pair(groupingResult.first, groupingResult.second + 1);
    } else {
      logger::Logger::dispatchLog(logger::errorLog{
          error::error{"Expected \")\" after expression with \"(\", but "
                       "expression terminated prematurely",
                       "Invalid Grouping syntax"}});
      return groupingResult;
    }
  }
}

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> negation(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for negation at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});
  if (tokenPtr->type == util::symbols::NEG) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found negation at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});
    auto negationResult = negation(tokens, tokenPtr + 1);
    const auto operatorStruct =
        new UnaryOperator{tokenPtr.base(), negationResult.first};
    const auto expr = new Node{parser::UNARY, operatorStruct};
    return std::make_pair(expr, negationResult.second);
  }

  return primary(tokens, tokenPtr);
}

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> conjunction(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for conjunction at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});
  const auto negationResult = negation(tokens, tokenPtr);
  auto expr = negationResult.first;
  tokenPtr = negationResult.second;

  while (tokenPtr < tokens.end() && tokenPtr->type == util::symbols::CONJUNCT) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found conjunction at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});
    const auto rightNegationResult = negation(tokens, tokenPtr + 1);
    const auto operatorStruct =
        new BinaryOperator{tokenPtr.base(), expr, rightNegationResult.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = rightNegationResult.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> disjunction(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for disjunction at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});
  const auto conjunctionResult = conjunction(tokens, tokenPtr);
  auto expr = conjunctionResult.first;
  tokenPtr = conjunctionResult.second;

  while (tokenPtr < tokens.end() && tokenPtr->type == util::symbols::DISJUNCT) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found disjunction at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});
    const auto rightConjunctionResult = conjunction(tokens, tokenPtr + 1);
    const auto operatorStruct =
        new BinaryOperator{tokenPtr.base(), expr, rightConjunctionResult.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = rightConjunctionResult.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> implication(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for implication at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});
  const auto disjunctionResult = disjunction(tokens, tokenPtr);
  auto expr = disjunctionResult.first;
  tokenPtr = disjunctionResult.second;

  while (tokenPtr < tokens.end() && tokenPtr->type == util::symbols::IMPL) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Found implication at " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});
    tokenizer::Token op = *tokenPtr;
    const auto rightDisjunctionResult = disjunction(tokens, tokenPtr + 1);
    const auto operatorStruct =
        new BinaryOperator{&op, expr, rightDisjunctionResult.first};
    expr = new Node{parser::BINARY, operatorStruct};
    tokenPtr = rightDisjunctionResult.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::pair<Node*, std::vector<tokenizer::Token>::const_iterator> expression(
    const std::vector<tokenizer::Token>& tokens,
    std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(logger::debugLog{
      "Checking for expression at " +
      std::to_string(std::distance(tokens.begin(), tokenPtr))});
  return implication(tokens, tokenPtr);
}

Node* parseAST(const std::vector<tokenizer::Token>& tokens) {
  logger::Logger::dispatchLog(logger::infoLog{"Starting to parse AST"});
  const auto tokenPtr = tokens.cbegin();
  const auto exprResult = expression(tokens, tokenPtr);

  if (exprResult.second != tokens.end()) {
    logger::Logger::dispatchLog(logger::warnLog{warning::warning{
        "tokenPtr in parser not at end of vector after parsing",
        "unknown cause"}});
    logger::Logger::dispatchLog(logger::debugLog{
        "tokenPtr at position " +
        std::to_string(std::distance(tokens.begin(), tokenPtr))});
  }

  logger::Logger::dispatchLog(logger::infoLog{"AST successfully parsed"});
  return exprResult.first;
}

void deallocAST(const Node* root) {
  if (root->type == parser::ATOM) {
    auto ptr = std::get<const parser::Atom*>(root->node);
    delete ptr;
  } else if (root->type == parser::BINARY) {
    auto ptr = std::get<const parser::BinaryOperator*>(root->node);
    deallocAST(ptr->left);
    deallocAST(ptr->right);
    delete ptr->left;
    delete ptr->right;
    delete ptr;
  } else if (root->type == parser::UNARY) {
    auto ptr = std::get<const parser::UnaryOperator*>(root->node);
    deallocAST(ptr->child);
    delete ptr->child;
    delete ptr;
  }
}

}  // namespace parser
