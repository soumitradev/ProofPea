#include <parser/parser.h>

namespace parser {
namespace parser {

struct AST* AST::copy(struct AST* ast) {
  logger::Logger::dispatchLog(logger::infoLog{"Copying AST"});
  auto copy = new AST{};
  copy->absolutes.clear();
  copy->atoms.clear();
  copy->tokens.clear();

  copy->absolutes.reserve(ast->absolutes.size());
  copy->atoms.reserve(ast->atoms.size());
  copy->root = copyNode(ast->root, copy->tokens, copy->atoms, copy->absolutes);
  return copy;
}

struct Node* AST::copyNode(
    struct Node* node, std::vector<std::shared_ptr<tokenizer::Token>>& tokens,
    std::unordered_map<std::string, Node*>& atoms,
    std::unordered_map<std::string, Node*>& absolutes) {
  if (node->type == NodeType::ABSOLUTE) {
    const auto parserNode = std::get<Absolute*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ABSOLUTE node " +
                                                 parserNode->token->lexeme +
                                                 ", checking if node has "
                                                 "already been copied"});
    const auto existing = absolutes.find(parserNode->token->lexeme);
    if (existing == absolutes.end()) {
      logger::Logger::dispatchLog(
          logger::debugLog{"Did not find ABSOLUTE node " +
                           parserNode->token->lexeme + ", copying"});

      tokens.push_back(
          std::make_shared<tokenizer::Token>(*(parserNode->token)));
      const auto newNode = new Absolute{tokens.back()};
      const auto copyNode = new Node{node->type, nullptr, newNode};
      absolutes[parserNode->token->lexeme] = copyNode;
      return copyNode;
    }
    logger::Logger::dispatchLog(logger::debugLog{
        "Found ABSOLUTE node " + parserNode->token->lexeme + ", returning"});
    return existing->second;
  } else if (node->type == NodeType::ATOM) {
    const auto parserNode = std::get<Atom*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{"Identified ATOM node " +
                                                 parserNode->token->lexeme +
                                                 ", checking if node has "
                                                 "already been copied"});
    const auto existing = atoms.find(parserNode->token->lexeme);
    if (existing == atoms.end()) {
      logger::Logger::dispatchLog(logger::debugLog{
          "Did not find ATOM node " + parserNode->token->lexeme + ", copying"});
      tokens.push_back(
          std::make_shared<tokenizer::Token>(*(parserNode->token)));
      const auto newNode = new Atom{tokens.back()};
      const auto copyNode = new Node{node->type, nullptr, newNode};
      atoms[parserNode->token->lexeme] = copyNode;
      return copyNode;
    }
    logger::Logger::dispatchLog(logger::debugLog{
        "Found ATOM node " + parserNode->token->lexeme + ", returning"});
    return existing->second;
  } else if (node->type == NodeType::UNARY) {
    const auto parserNode = std::get<UnaryOperator*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{
        "Identified UNARY node " + parserNode->op->lexeme + ", copying"});
    tokens.push_back(std::make_shared<tokenizer::Token>(*(parserNode->op)));
    const auto newTokenPtr = tokens.back();
    logger::Logger::dispatchLog(logger::debugLog{
        "Copying child node for UNARY node " + parserNode->op->lexeme});
    const auto newChild = copyNode(parserNode->child, tokens, atoms, absolutes);
    const auto newNode = new UnaryOperator{newTokenPtr, newChild};
    newChild->parent = new Node{node->type, nullptr, newNode};
    return newChild->parent;
  } else if (node->type == NodeType::BINARY) {
    const auto parserNode = std::get<BinaryOperator*>(node->node);
    logger::Logger::dispatchLog(logger::debugLog{
        "Identified BINARY node " + parserNode->op->lexeme + ", copying"});
    tokens.push_back(std::make_shared<tokenizer::Token>(*(parserNode->op)));
    const auto newTokenPtr = tokens.back();
    logger::Logger::dispatchLog(logger::debugLog{
        "Copying left child node for BINARY node " + parserNode->op->lexeme});
    const auto newLeftChild =
        copyNode(parserNode->left, tokens, atoms, absolutes);
    logger::Logger::dispatchLog(logger::debugLog{
        "Copying right child node for BINARY node " + parserNode->op->lexeme});
    const auto newRightChild =
        copyNode(parserNode->right, tokens, atoms, absolutes);
    const auto newOp =
        new BinaryOperator{newTokenPtr, newLeftChild, newRightChild};

    const auto newNode = new Node{node->type, nullptr, newOp};
    newLeftChild->parent = newNode;
    newRightChild->parent = newNode;
    return newNode;
  }

  logger::Logger::dispatchLog(logger::errorLog{
    error : error::parser::unexpected_token{
        "Encountered unexpected token while copying AST"}
  });
  return nullptr;
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::primary(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for primary token at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});

  if (tokenPtr->type == util::symbols::ATOM) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Detected atom " + tokenPtr->lexeme + " at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});

    const auto atomNodePtr = this->atoms.find(tokenPtr->lexeme);
    if (atomNodePtr != this->atoms.end()) {
      logger::Logger::dispatchLog(
          logger::debugLog{"Found atom " + tokenPtr->lexeme + " in hash-map"});
      return std::make_pair(atomNodePtr->second, tokenPtr + 1);
    }

    logger::Logger::dispatchLog(
        logger::debugLog{"Did not find atom " + tokenPtr->lexeme +
                         " in hash-map, creating and inserting"});

    const auto atom = new Atom{std::make_shared<tokenizer::Token>(*tokenPtr)};
    const auto atomNode = new Node{parser::ATOM, nullptr, atom};
    this->atoms[tokenPtr->lexeme] = atomNode;

    return std::make_pair(atomNode, tokenPtr + 1);
  } else if (tokenPtr->type == util::symbols::ABSOLUTETRUE ||
             tokenPtr->type == util::symbols::ABSOLUTEFALSE) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Detected absolute " + tokenPtr->lexeme + " at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});

    const auto absolutePtr = this->absolutes.find(tokenPtr->lexeme);
    if (absolutePtr != this->absolutes.end()) {
      logger::Logger::dispatchLog(logger::debugLog{
          "Found absolute " + tokenPtr->lexeme + " in hash-map"});
      return std::make_pair(absolutePtr->second, tokenPtr + 1);
    }

    logger::Logger::dispatchLog(
        logger::debugLog{"Did not find absolute " + tokenPtr->lexeme +
                         " in hash-map, creating and inserting"});

    const auto absolute =
        new Absolute{std::make_shared<tokenizer::Token>(*tokenPtr)};
    const auto absoluteNode = new Node{parser::ABSOLUTE, nullptr, absolute};
    this->absolutes[tokenPtr->lexeme] = absoluteNode;

    return std::make_pair(absoluteNode, tokenPtr + 1);
  }

  if (tokenPtr->type == util::symbols::LBRACE) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Detected grouping at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});

    const auto groupingResult = this->expression(tokenPtr + 1);
    if (std::holds_alternative<error::parser::unexpected_token>(
            groupingResult)) {
      const auto groupingError =
          std::get<error::parser::unexpected_token>(groupingResult);
      return groupingError;
    }

    const auto groupedExpr = std::get<
        std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
        groupingResult);
    if (groupedExpr.second < this->constructionTokens->end()) {
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
      std::to_string(
          std::distance(this->constructionTokens->cbegin(), tokenPtr)) +
      ", found " + tokenPtr->lexeme};
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::negation(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for negation at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});
  if (tokenPtr->type == util::symbols::NEG) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Found negation at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});
    auto negationResult = this->negation(tokenPtr + 1);
    if (std::holds_alternative<error::parser::unexpected_token>(
            negationResult)) {
      const auto negationError =
          std::get<error::parser::unexpected_token>(negationResult);
      return negationError;
    }

    const auto negationExpr = std::get<
        std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
        negationResult);
    const auto operatorStruct = new UnaryOperator{
        std::make_shared<tokenizer::Token>(*tokenPtr), negationExpr.first};
    const auto expr = new Node{parser::UNARY, nullptr, operatorStruct};
    negationExpr.first->parent = expr;
    return std::make_pair(expr, negationExpr.second);
  }

  return primary(tokenPtr);
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::conjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for conjunction at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});
  const auto negationResult = this->negation(tokenPtr);
  if (std::holds_alternative<error::parser::unexpected_token>(negationResult)) {
    const auto negationError =
        std::get<error::parser::unexpected_token>(negationResult);
    return negationError;
  }

  const auto negationExpr =
      std::get<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
          negationResult);
  auto expr = negationExpr.first;
  tokenPtr = negationExpr.second;

  while (tokenPtr < this->constructionTokens->end() &&
         tokenPtr->type == util::symbols::CONJUNCT) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Found conjunction at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});
    const auto rightNegationResult = this->negation(tokenPtr + 1);
    if (std::holds_alternative<error::parser::unexpected_token>(
            rightNegationResult)) {
      const auto rightNegationError =
          std::get<error::parser::unexpected_token>(rightNegationResult);
      return rightNegationResult;
    }

    const auto rightNegationExpr = std::get<
        std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
        rightNegationResult);
    const auto operatorStruct =
        new BinaryOperator{std::make_shared<tokenizer::Token>(*tokenPtr), expr,
                           rightNegationExpr.first};
    const auto newExpr = new Node{parser::BINARY, nullptr, operatorStruct};
    expr->parent = newExpr;
    rightNegationExpr.first->parent = newExpr;
    expr = newExpr;
    tokenPtr = rightNegationExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::disjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for disjunction at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});
  const auto conjunctionResult = this->conjunction(tokenPtr);
  if (std::holds_alternative<error::parser::unexpected_token>(
          conjunctionResult)) {
    const auto conjunctionError =
        std::get<error::parser::unexpected_token>(conjunctionResult);
    return conjunctionError;
  }

  const auto conjunctionExpr =
      std::get<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
          conjunctionResult);
  auto expr = conjunctionExpr.first;
  tokenPtr = conjunctionExpr.second;

  while (tokenPtr < this->constructionTokens->end() &&
         tokenPtr->type == util::symbols::DISJUNCT) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Found disjunction at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});
    const auto rightConjunctionResult = this->conjunction(tokenPtr + 1);
    if (std::holds_alternative<error::parser::unexpected_token>(
            rightConjunctionResult)) {
      const auto rightConjunctionError =
          std::get<error::parser::unexpected_token>(rightConjunctionResult);
      return rightConjunctionError;
    }

    const auto rightConjunctionExpr = std::get<
        std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
        rightConjunctionResult);
    const auto operatorStruct =
        new BinaryOperator{std::make_shared<tokenizer::Token>(*tokenPtr), expr,
                           rightConjunctionExpr.first};
    const auto newExpr = new Node{parser::BINARY, nullptr, operatorStruct};
    expr->parent = newExpr;
    rightConjunctionExpr.first->parent = newExpr;
    expr = newExpr;
    tokenPtr = rightConjunctionExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::implication(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for implication at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});
  const auto disjunctionResult = this->disjunction(tokenPtr);
  if (std::holds_alternative<error::parser::unexpected_token>(
          disjunctionResult)) {
    const auto disjunctionError =
        std::get<error::parser::unexpected_token>(disjunctionResult);
    return disjunctionError;
  }

  const auto disjunctionExpr =
      std::get<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
          disjunctionResult);
  auto expr = disjunctionExpr.first;
  tokenPtr = disjunctionExpr.second;

  if (tokenPtr < this->constructionTokens->end() &&
      tokenPtr->type == util::symbols::IMPL) {
    logger::Logger::dispatchLog(
        logger::debugLog{"Found implication at " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});
    auto op = tokenPtr.base();
    const auto implicationResult = this->implication(tokenPtr + 1);
    if (std::holds_alternative<error::parser::unexpected_token>(
            implicationResult)) {
      const auto implicationError =
          std::get<error::parser::unexpected_token>(implicationResult);
      return implicationError;
    }
    const auto implicationExpr = std::get<
        std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
        implicationResult);
    const auto operatorStruct =
        new BinaryOperator{std::make_shared<tokenizer::Token>(*op),
                           disjunctionExpr.first, implicationExpr.first};
    expr = new Node{parser::BINARY, nullptr, operatorStruct};
    disjunctionExpr.first->parent = expr;
    implicationExpr.first->parent = expr;
    tokenPtr = implicationExpr.second;
  }

  return std::make_pair(expr, tokenPtr);
}

std::variant<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
AST::expression(std::vector<tokenizer::Token>::const_iterator tokenPtr) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Checking for expression at " +
                       std::to_string(std::distance(
                           this->constructionTokens->cbegin(), tokenPtr))});
  return this->implication(tokenPtr);
}

std::variant<bool, error::parser::unexpected_token> AST::parseAST(
    std::vector<tokenizer::Token>& tokens) {
  this->constructionTokens = &tokens;
  logger::Logger::dispatchLog(logger::infoLog{"Starting to parse AST"});
  const auto tokenPtr = this->constructionTokens->cbegin();
  this->atoms.clear();

  const auto exprResult = this->expression(tokenPtr);
  if (std::holds_alternative<error::parser::unexpected_token>(exprResult)) {
    const auto exprError =
        std::get<error::parser::unexpected_token>(exprResult);
    return exprError;
  }

  const auto expr =
      std::get<std::pair<Node*, std::vector<tokenizer::Token>::const_iterator>>(
          exprResult);

  if (expr.second != this->constructionTokens->end()) {
    logger::Logger::dispatchLog(logger::warnLog{warning::warning{
        "tokenPtr in parser not at end of vector after parsing",
        "unknown cause"}});
    logger::Logger::dispatchLog(
        logger::debugLog{"tokenPtr at position " +
                         std::to_string(std::distance(
                             this->constructionTokens->cbegin(), tokenPtr))});
  }

  // Null out the construction pointer since we don't need it anymore
  this->constructionTokens = nullptr;

  logger::Logger::dispatchLog(logger::infoLog{"AST successfully parsed"});
  this->root = expr.first;
  return true;
}

void deallocNodeRecursive(const Node* root,
                          std::unordered_set<const Node*>& deletedAtoms) {
  if (root->type == parser::ATOM) {
    auto ptr = std::get<parser::Atom*>(root->node);
    delete ptr;
  } else if (root->type == parser::ABSOLUTE) {
    auto ptr = std::get<parser::Absolute*>(root->node);
    delete ptr;
  } else if (root->type == parser::BINARY) {
    auto ptr = std::get<parser::BinaryOperator*>(root->node);
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
    auto ptr = std::get<parser::UnaryOperator*>(root->node);
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
  delete ast;
}
}  // namespace parser
}  // namespace parser
