#ifndef CNF_CONVERTOR_PARSE_TREE
#define CNF_CONVERTOR_PARSE_TREE

#include <error/parser.h>
#include <tokenizer/tokenizer.h>

#include <iostream>
#include <unordered_map>
#include <variant>

namespace parser {

enum NodeType { UNARY, BINARY, ATOM };

struct Atom {
  const tokenizer::Token *token;
};

struct UnaryOperator {
  const tokenizer::Token *op;
  const struct Node *child;
};

struct BinaryOperator {
  const tokenizer::Token *op;
  const struct Node *left;
  const struct Node *right;
};

struct Node {
  NodeType type;
  const std::variant<const UnaryOperator *, const BinaryOperator *,
                     const Atom *>
      node;
};

std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
             error::parser::unexpected_token>
expression(const std::vector<tokenizer::Token> &tokens,
           std::vector<tokenizer::Token>::const_iterator tokenPtr);

std::variant<Node *, error::parser::unexpected_token> parseAST(
    const std::vector<tokenizer::Token> &tokens);

void deallocAST(const Node *root);

}  // namespace parser

#endif  // CNF_CONVERTOR_PARSE_TREE
