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
  tokenizer::Token *token;
};

struct Atom {
  tokenizer::Token *token;
};

struct UnaryOperator {
  tokenizer::Token *op;
  struct Node *child;
};

struct BinaryOperator {
  tokenizer::Token *op;
  struct Node *left;
  struct Node *right;
};

struct Node {
  NodeType type;
  std::variant<UnaryOperator *, BinaryOperator *, Atom *, Absolute *> node;
};

struct AST {
  struct Node *root;
  std::vector<tokenizer::Token *> tokens;
  std::unordered_map<std::string, Node *> atoms;
  std::unordered_map<std::string, Node *> absolutes;
  AST() { this->root = nullptr; }
  AST(const std::vector<tokenizer::Token *> &tokens) {
    this->tokens = std::vector<tokenizer::Token *>(tokens);
  }

  static struct AST *copy(struct AST *ast);

 private:
  static struct Node *copyNode(
      const struct Node *node, std::vector<tokenizer::Token *> &tokens,
      std::unordered_map<std::string, Node *> &atoms,
      std::unordered_map<std::string, Node *> &absolutes);
};

std::variant<std::pair<Node *, std::vector<tokenizer::Token *>::const_iterator>,
             error::parser::unexpected_token>
expression(std::vector<tokenizer::Token *>::const_iterator tokenPtr, AST *ast);
std::variant<AST *, error::parser::unexpected_token> parseAST(
    const std::vector<tokenizer::Token *> &tokens);

void deallocAST(AST *ast);
void rebuildTokens(AST *ast);

}  // namespace parser
}  // namespace parser

#endif  // CNF_CONVERTOR_PARSER
