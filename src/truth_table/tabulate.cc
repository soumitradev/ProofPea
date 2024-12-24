#include <truth_table/tabulate.h>

namespace truth_table {
namespace tabulator {

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
populatePartialTruthTable(
    std::map<std::vector<bool>, bool>& table, const parser::parser::AST* ast,
    const std::vector<std::shared_ptr<parser::parser::Node>>& atomNodes,
    const std::set<std::shared_ptr<parser::parser::Node>>& skippedNodes,
    const std::vector<std::shared_ptr<parser::parser::Node>>::const_iterator
        atomPtr,
    std::unordered_map<std::shared_ptr<parser::parser::Node>, bool>& state) {
  if (atomPtr == atomNodes.end()) {
    logger::Logger::dispatchLog(logger::debugLog{"All atoms assigned values"});
    std::vector<bool> atomState(atomNodes.size() - skippedNodes.size());
    std::ostringstream stateString;
    stateString << "[";
    int atomIndex = 0;
    for (size_t i = 0; i < atomState.size(); i++) {
      if (skippedNodes.find(atomNodes[i]) != skippedNodes.end()) continue;
      atomState[atomIndex] = state[atomNodes[i]];
      const auto node =
          std::get<std::shared_ptr<parser::parser::Atom>>(atomNodes[i]->node);
      stateString << node->token->lexeme << ": "
                  << (atomState[atomIndex] ? "1" : "0")
                  << ((atomIndex == atomState.size() - 1) ? "" : ", ");
      atomIndex++;
    }
    stateString << "]";

    logger::Logger::dispatchLog(logger::debugLog{
        "Evaluating expression for state " + stateString.str()});
    const auto evalResult = eval::evaluateState(ast, state);
    if (std::holds_alternative<error::eval::unexpected_node>(evalResult)) {
      return std::get<error::eval::unexpected_node>(evalResult);
    }
    if (std::holds_alternative<error::eval::mismatched_atoms>(evalResult)) {
      return std::get<error::eval::mismatched_atoms>(evalResult);
    }
    const auto evalValue = std::get<bool>(evalResult);

    logger::Logger::dispatchLog(logger::debugLog{
        "State evaluated to: " +
        (evalValue ? std::string("true") : std::string("false"))});
    table[atomState] = evalValue;
    return true;
  }

  const auto node =
      std::get<std::shared_ptr<parser::parser::Atom>>((*atomPtr)->node);
  if (skippedNodes.find(*atomPtr) != skippedNodes.end()) {
    logger::Logger::dispatchLog(logger::debugLog{
        "Forcing skipped atom \"" + node->token->lexeme + "\" to false value"});
    state[*atomPtr] = false;
    const auto skippedStateResult = populatePartialTruthTable(
        table, ast, atomNodes, skippedNodes, atomPtr + 1, state);
    if (std::holds_alternative<error::eval::unexpected_node>(
            skippedStateResult)) {
      return std::get<error::eval::unexpected_node>(skippedStateResult);
    }
    if (std::holds_alternative<error::eval::mismatched_atoms>(
            skippedStateResult)) {
      return std::get<error::eval::mismatched_atoms>(skippedStateResult);
    }
    return true;
  }

  logger::Logger::dispatchLog(logger::debugLog{
      "Setting atom \"" + node->token->lexeme + "\" to false"});
  state[*atomPtr] = false;
  const auto falseStateResult = populatePartialTruthTable(
      table, ast, atomNodes, skippedNodes, atomPtr + 1, state);
  if (std::holds_alternative<error::eval::unexpected_node>(falseStateResult)) {
    return std::get<error::eval::unexpected_node>(falseStateResult);
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(falseStateResult)) {
    return std::get<error::eval::mismatched_atoms>(falseStateResult);
  }

  logger::Logger::dispatchLog(
      logger::debugLog{"Setting atom \"" + node->token->lexeme + "\" to true"});
  state[*atomPtr] = true;
  const auto trueStateResult = populatePartialTruthTable(
      table, ast, atomNodes, skippedNodes, atomPtr + 1, state);
  if (std::holds_alternative<error::eval::unexpected_node>(trueStateResult)) {
    return std::get<error::eval::unexpected_node>(trueStateResult);
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(trueStateResult)) {
    return std::get<error::eval::mismatched_atoms>(trueStateResult);
  }

  return true;
}

std::variant<std::vector<std::string>, error::eval::unexpected_node,
             error::eval::mismatched_atoms>
constructPartialTruthTable(
    std::map<std::vector<bool>, bool>& table, const parser::parser::AST* ast,
    const std::set<std::shared_ptr<parser::parser::Node>>& skippedNodes) {
  logger::Logger::dispatchLog(
      logger::debugLog{"Constructing list of sorted atoms"});
  std::vector<std::pair<std::string, std::shared_ptr<parser::parser::Node>>>
      atoms;
  for (auto& x : ast->atoms) {
    atoms.push_back(x);
  }
  std::sort(atoms.begin(), atoms.end());

  logger::Logger::dispatchLog(
      logger::debugLog{"Constructing initial states for atoms"});
  std::vector<std::string> atomStrings(atoms.size());
  std::vector<std::shared_ptr<parser::parser::Node>> atomNodes(atoms.size());
  std::unordered_map<std::shared_ptr<parser::parser::Node>, bool> state;
  for (size_t i = 0; i < atoms.size(); i++) {
    atomStrings[i] = atoms[i].first;
    atomNodes[i] = atoms[i].second;
    state[atoms[i].second] = false;
  }

  logger::Logger::dispatchLog(
      logger::debugLog{"Generating all possible atom states"});
  const auto status = populatePartialTruthTable(
      table, ast, atomNodes, skippedNodes, atomNodes.begin(), state);
  if (std::holds_alternative<error::eval::unexpected_node>(status)) {
    return std::get<error::eval::unexpected_node>(status);
  }
  if (std::holds_alternative<error::eval::mismatched_atoms>(status)) {
    return std::get<error::eval::mismatched_atoms>(status);
  }
  return atomStrings;
}

std::variant<std::vector<std::string>, error::eval::unexpected_node,
             error::eval::mismatched_atoms>
constructTruthTable(std::map<std::vector<bool>, bool>& table,
                    const parser::parser::AST* ast) {
  const auto skipped = std::set<std::shared_ptr<parser::parser::Node>>();
  return constructPartialTruthTable(table, ast, skipped);
}

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms>
printTruthTable(const parser::parser::AST* ast) {
  std::map<std::vector<bool>, bool> table;
  logger::Logger::dispatchLog(logger::infoLog{"Constructing truth table"});
  const auto truthTableResult =
      truth_table::tabulator::constructTruthTable(table, ast);
  if (std::holds_alternative<error::eval::unexpected_node>(truthTableResult)) {
    return std::get<error::eval::unexpected_node>(truthTableResult);
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 truthTableResult)) {
    return std::get<error::eval::mismatched_atoms>(truthTableResult);
  }
  const auto atomList = std::get<std::vector<std::string>>(truthTableResult);
  logger::Logger::dispatchLog(logger::infoLog{"Printing truth table"});
  for (auto& atomString : atomList) {
    std::cout << atomString << "\t";
  }
  std::cout << "=" << std::endl;
  for (auto x : table) {
    for (auto val : x.first) {
      std::cout << (val ? "T" : "F") << "\t";
    }
    std::cout << (x.second ? "T" : "F") << std::endl;
  }
  return true;
}
}  // namespace tabulator
}  // namespace truth_table
