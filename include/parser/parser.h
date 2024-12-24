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
  std::shared_ptr<tokenizer::Token> token;
};

struct Atom {
  std::shared_ptr<tokenizer::Token> token;
};

struct UnaryOperator {
  std::shared_ptr<tokenizer::Token> op;
  struct Node *child;
};

struct BinaryOperator {
  std::shared_ptr<tokenizer::Token> op;
  struct Node *left;
  struct Node *right;
};

struct Node {
  NodeType type;
  struct Node *parent;
  std::variant<UnaryOperator *, BinaryOperator *, Atom *, Absolute *> node;
};

struct AST {
 private:
  std::vector<tokenizer::Token> *constructionTokens;

 public:
  struct Node *root;
  std::vector<std::shared_ptr<tokenizer::Token>> tokens;
  std::unordered_map<std::string, Node *> atoms;
  std::unordered_map<std::string, Node *> absolutes;
  AST() { this->root = nullptr; }

  static struct AST *copy(struct AST *ast);
  static struct Node *copyNode(
      struct Node *node, std::vector<std::shared_ptr<tokenizer::Token>> &tokens,
      std::unordered_map<std::string, Node *> &atoms,
      std::unordered_map<std::string, Node *> &absolutes);

  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  primary(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  negation(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  conjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  disjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  implication(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<Node *, std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  expression(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<bool, error::parser::unexpected_token> parseAST(
      std::vector<tokenizer::Token> &tokens);
};

void deallocAST(AST *ast);
void rebuildTokens(AST *ast);

}  // namespace parser
}  // namespace parser

#endif  // CNF_CONVERTOR_PARSER
