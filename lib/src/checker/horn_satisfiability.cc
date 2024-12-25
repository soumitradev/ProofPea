#include <checker/horn_satisfiability.h>

namespace proofpea {
namespace checker {
namespace horn_satisfiability {

void populateHornClauses(
    std::shared_ptr<parser::parser::Node> node,
    std::vector<std::shared_ptr<parser::parser::BinaryOperator>>& result) {
  if (node->type == parser::parser::NodeType::BINARY) {
    const auto binaryOp =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(node->node);
    if (binaryOp->op->type == util::symbols::CONJUNCT) {
      populateHornClauses(binaryOp->left, result);
      populateHornClauses(binaryOp->right, result);
    } else if (binaryOp->op->type == util::symbols::IMPL) {
      result.push_back(binaryOp);
    }
  }
}

std::variant<bool, error::horn::invalid_horn_formula> allLeftMarkedRecursive(
    std::shared_ptr<parser::parser::BinaryOperator> op,
    std::unordered_set<std::shared_ptr<parser::parser::Node>>& marked) {
  if ((op->right->type == parser::parser::NodeType::ABSOLUTE) ||
      (op->right->type == parser::parser::NodeType::ATOM)) {
    // Have we reached the end?
    if ((op->left->type == parser::parser::NodeType::ABSOLUTE) ||
        (op->left->type == parser::parser::NodeType::ATOM)) {
      return (marked.find(op->right) != marked.end()) &&
             (marked.find(op->left) != marked.end());
    }
    // If not, continue
    if (op->left->type != parser::parser::NodeType::BINARY) {
      return error::horn::invalid_horn_formula{
          "Invalid Horn formula passed to allLeftMarkedRecursive"};
    }
    const auto leftOp =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
            op->left->node);
    if (marked.find(op->right) == marked.end()) return false;
    return allLeftMarkedRecursive(leftOp, marked);
  }

  if ((op->left->type == parser::parser::NodeType::ABSOLUTE) ||
      (op->left->type == parser::parser::NodeType::ATOM)) {
    if (marked.find(op->left) == marked.end()) return false;
    // We know right side is not absolute or atom
    if (op->right->type != parser::parser::NodeType::BINARY) {
      return error::horn::invalid_horn_formula{
          "Invalid Horn formula passed to allLeftMarkedRecursive"};
    }
    const auto rightOp =
        std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
            op->right->node);
    return allLeftMarkedRecursive(rightOp, marked);
  }

  return error::horn::invalid_horn_formula{
      "Invalid Horn formula passed to allLeftMarkedRecursive"};
}

std::variant<bool, error::horn::invalid_horn_formula> allLeftMarked(
    std::shared_ptr<parser::parser::BinaryOperator> impl,
    std::unordered_set<std::shared_ptr<parser::parser::Node>>& marked) {
  if (((impl->right->type != parser::parser::NodeType::ABSOLUTE) &&
       (impl->right->type != parser::parser::NodeType::ATOM)) ||
      ((impl->left->type != parser::parser::NodeType::BINARY) &&
       (impl->left->type != parser::parser::NodeType::ABSOLUTE) &&
       (impl->left->type != parser::parser::NodeType::ATOM)))
    return error::horn::invalid_horn_formula{
        "Invalid Horn formula passed to allLeftMarked"};

  if ((impl->left->type == parser::parser::NodeType::ABSOLUTE) ||
      (impl->left->type == parser::parser::NodeType::ATOM))
    return (marked.find(impl->left) != marked.end());

  const auto left = std::get<std::shared_ptr<parser::parser::BinaryOperator>>(
      impl->left->node);
  return allLeftMarkedRecursive(left, marked);
}

std::variant<bool, error::horn::invalid_horn_formula> checkHornSatisfiability(
    parser::parser::AST* ast) {
  if (!horn::checkHornFormula(ast))
    return error::horn::invalid_horn_formula{
        "Invalid Horn formula passed to checkHornSatisfiability"};

  std::unordered_set<std::shared_ptr<parser::parser::Node>> marked;
  std::vector<std::shared_ptr<parser::parser::BinaryOperator>> clauses;
  populateHornClauses(ast->root, clauses);

  // Implicitly assumes that the ast maps are correct
  const auto trueResult = ast->absolutes.find("1");
  const auto falseResult = ast->absolutes.find("0");

  // If there is no T then nothing gets marked, so F can't be marked
  // If there is no F then F can't be marked
  if (trueResult == ast->absolutes.end() || falseResult == ast->absolutes.end())
    return true;

  const auto trueNode = trueResult->second;
  const auto falseNode = falseResult->second;

  // Mark T
  marked.insert(trueNode);

  int round = 1;
  while (true) {
    // Keep looping till either nothing changed, or absolute F is marked
    bool changed = false;
    for (size_t i = 0; i < clauses.size(); i++) {
      // If RHS already marked, continue
      if (marked.find(clauses[i]->right) != marked.end()) continue;

      const auto leftMarked = allLeftMarked(clauses[i], marked);
      if (std::holds_alternative<error::horn::invalid_horn_formula>(
              leftMarked)) {
        return std::get<error::horn::invalid_horn_formula>(leftMarked);
      } else if (std::holds_alternative<bool>(leftMarked)) {
        if (std::get<bool>(leftMarked)) {
          marked.insert(clauses[i]->right);
          changed = true;
        }
      }
    }

    if (marked.find(falseNode) != marked.end()) return false;
    if (!changed) break;
    round++;
  }

  return true;
}

}  // namespace horn_satisfiability
}  // namespace checker
}  // namespace proofpea
