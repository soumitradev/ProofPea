#ifndef CNF_CONVERTOR_PARSE_TREE
#define CNF_CONVERTOR_PARSE_TREE

#include <error/parser.h>
#include <tokenizer/tokenizer.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace parser {

enum NodeType { UNARY, BINARY, ATOM, ABSOLUTE };

struct Absolute {
  const tokenizer::Token *token;
};

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
                     const Atom *, const Absolute *>
      node;
};

struct AST {
  const struct Node *root;
  std::unordered_map<std::string, const Node *> atoms;
  std::unordered_map<std::string, const Node *> absolutes;
};

std::variant<
    std::pair<const Node *, std::vector<tokenizer::Token *>::const_iterator>,
    error::parser::unexpected_token>
expression(const std::vector<tokenizer::Token *> &tokens,
           std::vector<tokenizer::Token *>::const_iterator tokenPtr, AST *ast);
std::variant<AST *, error::parser::unexpected_token> parseAST(
    const std::vector<tokenizer::Token *> &tokens);

void deallocAST(AST *ast);

}  // namespace parser

#endif  // CNF_CONVERTOR_PARSE_TREE
