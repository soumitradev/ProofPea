#ifndef CNF_CONVERTOR_PARSER
#define CNF_CONVERTOR_PARSER

#include <error/parser.h>
#include <parser/tokenizer.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace proofpea {
namespace parser {
namespace parser {

enum NodeType { UNARY, BINARY, ATOM, ABSOLUTE };

struct Node;

struct Absolute {
  std::shared_ptr<tokenizer::Token> token;
  Absolute(std::shared_ptr<tokenizer::Token> token) { this->token = token; }
};

struct Atom {
  std::shared_ptr<tokenizer::Token> token;
  Atom(std::shared_ptr<tokenizer::Token> token) { this->token = token; }
};

struct UnaryOperator {
  std::shared_ptr<tokenizer::Token> op;
  std::shared_ptr<Node> child;
  UnaryOperator(std::shared_ptr<tokenizer::Token> op,
                std::shared_ptr<Node> child) {
    this->op = op;
    this->child = child;
  }
};

struct BinaryOperator {
  std::shared_ptr<tokenizer::Token> op;
  std::shared_ptr<Node> left;
  std::shared_ptr<Node> right;
  BinaryOperator(std::shared_ptr<tokenizer::Token> op,
                 std::shared_ptr<Node> left, std::shared_ptr<Node> right) {
    this->op = op;
    this->left = left;
    this->right = right;
  }
};

struct Node {
  NodeType type;
  std::weak_ptr<Node> parent;
  std::variant<std::shared_ptr<UnaryOperator>, std::shared_ptr<BinaryOperator>,
               std::shared_ptr<Atom>, std::shared_ptr<Absolute>>
      node;
  Node(NodeType type, std::weak_ptr<Node> parent,
       std::variant<std::shared_ptr<UnaryOperator>,
                    std::shared_ptr<BinaryOperator>, std::shared_ptr<Atom>,
                    std::shared_ptr<Absolute>>
           node) {
    this->type = type;
    this->parent = parent;
    this->node = node;
  }
};

struct AST {
 private:
  std::vector<tokenizer::Token> *constructionTokens;
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  primary(const std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  negation(const std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  conjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  disjunction(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  implication(std::vector<tokenizer::Token>::const_iterator tokenPtr);
  std::variant<std::pair<std::shared_ptr<Node>,
                         std::vector<tokenizer::Token>::const_iterator>,
               error::parser::unexpected_token>
  expression(const std::vector<tokenizer::Token>::const_iterator tokenPtr);

 public:
  std::shared_ptr<Node> root;
  std::vector<std::shared_ptr<tokenizer::Token>> tokens;
  std::unordered_map<std::string, std::shared_ptr<Node>> atoms;
  std::unordered_map<std::string, std::shared_ptr<Node>> absolutes;
  AST() { this->root = nullptr; }

  static std::shared_ptr<AST> copy(const struct AST *ast);
  static std::shared_ptr<Node> copyNode(
      const struct Node *node,
      std::vector<std::shared_ptr<tokenizer::Token>> &tokens,
      std::unordered_map<std::string, std::shared_ptr<Node>> &atoms,
      std::unordered_map<std::string, std::shared_ptr<Node>> &absolutes);

  std::variant<bool, error::parser::unexpected_token> parseAST(
      std::vector<tokenizer::Token> &tokens);
};

}  // namespace parser
}  // namespace parser
}  // namespace proofpea

#endif  // CNF_CONVERTOR_PARSER
