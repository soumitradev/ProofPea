#include <transformer/invert.h>

namespace proofpea {
namespace transformer {
namespace invert {

std::variant<bool, error::eval::unexpected_node> invertTree(
    std::shared_ptr<parser::parser::AST> ast) {
  logger::Logger::dispatchLog(logger::debugLog{"Adding negation to top of "});

  const auto negationToken =
      std::make_shared<parser::tokenizer::Token>(util::symbols::NEG, "~", 0);
  ast->tokens.push_back(negationToken);
  const auto negationNode = std::make_shared<parser::parser::Node>(
      parser::parser::UNARY, std::weak_ptr<parser::parser::Node>(),
      std::make_shared<parser::parser::UnaryOperator>(negationToken,
                                                      ast->root));
  ast->root->parent = negationNode;
  ast->root = negationNode;
  ast->root->parent.reset();

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
}  // namespace proofpea
