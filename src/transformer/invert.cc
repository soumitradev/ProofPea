#include <transformer/invert.h>

namespace transformer {
namespace invert {

std::variant<bool, error::eval::unexpected_node> invertTree(
    parser::parser::AST* ast) {
  logger::Logger::dispatchLog(logger::debugLog{"Adding negation to top of "});

  const auto negationToken =
      new parser::tokenizer::Token{util::symbols::NEG, "~", 0};
  ast->tokens.push_back(negationToken);
  const auto negationOp =
      new parser::parser::UnaryOperator{negationToken, ast->root};
  const auto negationNode =
      new parser::parser::Node{parser::parser::UNARY, nullptr, negationOp};
  ast->root->parent = negationNode;
  ast->root = negationNode;
  ast->root->parent = nullptr;

  const auto transformResult = transformer::cnf::transformToCNF(ast);
  if (std::holds_alternative<error::eval::unexpected_node>(transformResult)) {
    return std::get<error::eval::unexpected_node>(transformResult);
  } else if (std::holds_alternative<bool>(transformResult)) {
    if (!std::get<bool>(transformResult)) return false;
  } else {
    logger::Logger::dispatchLog(logger::errorLog{
      error :
          error::unknown::unknown_error{"Encountered unexpected return value "
                                        "from transformToCNF"}
    });
    return false;
  }
  return true;
}

}  // namespace invert
}  // namespace transformer
