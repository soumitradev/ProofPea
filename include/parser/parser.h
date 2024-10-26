#ifndef CNF_CONVERTOR_PARSER
#define CNF_CONVERTOR_PARSER

#include <error/parser.h>
#include <parser/tokenizer.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace parser {
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
  std::vector<tokenizer::Token> tokens;
  std::unordered_map<std::string, const Node *> atoms;
  std::unordered_map<std::string, const Node *> absolutes;
  AST() { this->root = nullptr; }
  AST(const std::vector<tokenizer::Token> &tokens) {
    this->tokens = std::vector<tokenizer::Token>(tokens);
  }

  static struct AST *copy(struct AST *ast);

 private:
  static const struct Node *copyNode(
      const struct Node *node, std::vector<tokenizer::Token> &tokens,
      std::unordered_map<std::string, const Node *> &atoms,
      std::unordered_map<std::string, const Node *> &absolutes);
};

std::variant<
    std::pair<const Node *, std::vector<tokenizer::Token>::const_iterator>,
    error::parser::unexpected_token>
expression(std::vector<tokenizer::Token>::const_iterator tokenPtr, AST *ast);
std::variant<AST *, error::parser::unexpected_token> parseAST(
    const std::vector<tokenizer::Token> &tokens);

void deallocAST(AST *ast);
}  // namespace parser
}  // namespace parser

#endif  // CNF_CONVERTOR_PARSER
