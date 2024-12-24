#include <checker/equivalence.h>

namespace checker {
namespace equivalence {

std::vector<bool> encodeState(unsigned long long data,
                              const unsigned long long size) {
  std::vector<bool> state(size, false);
  int i = size - 1;
  while (data > 0) {
    if (data % 2) state[i] = true;
    i--;
    data /= 2;
  }
  return state;
}

std::variant<bool, error::equivalence::incomplete_truth_table> checkDependence(
    std::map<std::vector<bool>, bool>& table,
    const std::vector<std::string>& atoms,
    const std::vector<std::string>::iterator atomPtr) {
  // Implicitly assumes that atom does indeed exist in table, because atomPtr
  // points to a location inside atoms
  const auto order = atoms.end() - atomPtr - 1L;
  const auto groupSize = 1ULL << order;

  for (unsigned long long i = 0; i < (1ULL << (atoms.size() - order - 1));
       i++) {
    const auto groupStart = i << (order + 1);
    const auto state1 = encodeState(groupStart, atoms.size());
    const auto state2 = encodeState(groupStart + groupSize, atoms.size());
    auto mapIterator1 = table.find(state1);
    auto mapIterator2 = table.find(state2);

    if (mapIterator1 == table.end() || mapIterator2 == table.end()) {
      return error::equivalence::incomplete_truth_table{
          "Incomplete truth table passed to checkDependence"};
    }

    for (unsigned long long j = 0; j < groupSize; j++) {
      if (mapIterator1->second != mapIterator2->second) return true;
      mapIterator1++;
      mapIterator2++;
    }
  }

  return false;
}

bool checkTautology(const std::map<std::vector<bool>, bool>& table) {
  for (auto&& entry : table) {
    if (entry.second != true) return false;
  }
  return true;
}

bool checkFallacy(const std::map<std::vector<bool>, bool>& table) {
  for (auto&& entry : table) {
    if (entry.second != false) return false;
  }
  return true;
}

std::variant<bool, error::eval::unexpected_node, error::eval::mismatched_atoms,
             error::equivalence::equivalence_chack_unsupported,
             error::equivalence::incomplete_truth_table>
checkEquivalence(const parser::parser::AST* ast1,
                 const parser::parser::AST* ast2) {
  if (ast1->atoms.size() > 32 || ast2->atoms.size() > 32) {
    return error::equivalence::equivalence_chack_unsupported{
        "Equivalence check unsupported for formulae with more than 32 unique "
        "atoms"};
  }

  std::map<std::vector<bool>, bool> table1;
  std::map<std::vector<bool>, bool> table2;
  const auto truthTableResult1 =
      truth_table::tabulator::constructTruthTable(table1, ast1);
  if (std::holds_alternative<error::eval::unexpected_node>(truthTableResult1)) {
    return std::get<error::eval::unexpected_node>(truthTableResult1);
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 truthTableResult1)) {
    return std::get<error::eval::mismatched_atoms>(truthTableResult1);
  }

  const auto truthTableResult2 =
      truth_table::tabulator::constructTruthTable(table2, ast2);
  if (std::holds_alternative<error::eval::unexpected_node>(truthTableResult2)) {
    return std::get<error::eval::unexpected_node>(truthTableResult2);
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 truthTableResult2)) {
    return std::get<error::eval::mismatched_atoms>(truthTableResult2);
  }

  std::vector<std::string> atoms1 =
      std::get<std::vector<std::string>>(truthTableResult1);
  std::vector<std::string> atoms2 =
      std::get<std::vector<std::string>>(truthTableResult2);

  std::set<std::string> commonAtoms;
  std::set<std::string> differingAtoms1;
  std::set<std::string> differingAtoms2;
  for (auto&& i : ast1->atoms) {
    if (ast2->atoms.find(i.first) == ast2->atoms.end()) {
      differingAtoms1.insert(i.first);
    } else {
      commonAtoms.insert(i.first);
    }
  }
  for (auto&& i : ast2->atoms) {
    if (ast1->atoms.find(i.first) == ast1->atoms.end()) {
      differingAtoms2.insert(i.first);
    } else {
      commonAtoms.insert(i.first);
    }
  }

  if (commonAtoms.size() == 0) {
    const auto tautology1 = checkTautology(table1);
    const auto tautology2 = checkTautology(table2);
    if (tautology1 && tautology2) return true;
    const auto fallacy1 = checkFallacy(table1);
    const auto fallacy2 = checkFallacy(table2);
    if (fallacy1 && fallacy2) return true;

    // If both are neither tautologies or fallacies, they cannot be equivalent
    return false;
  }

  // If either one of them is dependent on the differing atom, then they
  // cannot be equivalent
  for (auto&& i : differingAtoms1) {
    const auto atomPtr1 = std::find(atoms1.begin(), atoms1.end(), i);
    if (atomPtr1 != atoms1.end()) {
      const auto dependence1 = checkDependence(table1, atoms1, atomPtr1);
      if (std::holds_alternative<error::equivalence::incomplete_truth_table>(
              dependence1)) {
        return std::get<error::equivalence::incomplete_truth_table>(
            dependence1);
      } else if (std::holds_alternative<bool>(dependence1)) {
        if (std::get<bool>(dependence1)) return false;
      }
    } else {
      return error::equivalence::incomplete_truth_table{
          "AST 1 atoms and truth table atoms don't match in checkEquivalence."};
    }
  }
  for (auto&& i : differingAtoms2) {
    const auto atomPtr2 = std::find(atoms2.begin(), atoms2.end(), i);
    if (atomPtr2 != atoms2.end()) {
      const auto dependence2 = checkDependence(table2, atoms2, atomPtr2);
      if (std::holds_alternative<error::equivalence::incomplete_truth_table>(
              dependence2)) {
        return std::get<error::equivalence::incomplete_truth_table>(
            dependence2);
      } else if (std::holds_alternative<bool>(dependence2)) {
        if (std::get<bool>(dependence2)) return false;
      }
    } else {
      return error::equivalence::incomplete_truth_table{
          "AST 2 atoms and truth table atoms don't match in checkEquivalence."};
    }
  }

  std::set<std::shared_ptr<parser::parser::Node>> skippedNodes1;
  std::set<std::shared_ptr<parser::parser::Node>> skippedNodes2;
  for (auto&& i : differingAtoms1) {
    const auto node = ast1->atoms.find(i);
    if (node == ast1->atoms.end()) {
      return error::equivalence::incomplete_truth_table{
          "AST 1 atoms and truth table atoms don't match in checkEquivalence."};
    }
    skippedNodes1.insert(node->second);
  }
  for (auto&& i : differingAtoms2) {
    const auto node = ast2->atoms.find(i);
    if (node == ast2->atoms.end()) {
      return error::equivalence::incomplete_truth_table{
          "AST 2 atoms and truth table atoms don't match in checkEquivalence."};
    }
    skippedNodes2.insert(node->second);
  }

  // Check if for the common atoms they both return the same values
  std::map<std::vector<bool>, bool> partialTable1;
  std::map<std::vector<bool>, bool> partialTable2;
  const auto partialTableResult1 =
      truth_table::tabulator::constructPartialTruthTable(partialTable1, ast1,
                                                         skippedNodes1);
  if (std::holds_alternative<error::eval::unexpected_node>(
          partialTableResult1)) {
    return std::get<error::eval::unexpected_node>(partialTableResult1);
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 partialTableResult1)) {
    return std::get<error::eval::mismatched_atoms>(partialTableResult1);
  }

  const auto partialTableResult2 =
      truth_table::tabulator::constructPartialTruthTable(partialTable2, ast2,
                                                         skippedNodes2);
  if (std::holds_alternative<error::eval::unexpected_node>(
          partialTableResult2)) {
    return std::get<error::eval::unexpected_node>(partialTableResult2);
  } else if (std::holds_alternative<error::eval::mismatched_atoms>(
                 partialTableResult2)) {
    return std::get<error::eval::mismatched_atoms>(partialTableResult2);
  }

  if (partialTable1.size() != partialTable2.size()) {
    return error::equivalence::incomplete_truth_table{
        "AST 1 and AST 2 generated different sized partial truth tables in "
        "checkEquivalence."};
  }

  for (auto&& entry1 : partialTable1) {
    const auto entry2 = partialTable2.find(entry1.first);
    if (entry2 == partialTable2.end()) {
      return error::equivalence::incomplete_truth_table{
          "AST 1 and AST 2 generated different sized partial truth tables in "
          "checkEquivalence."};
    }
    if (entry1.second != entry2->second) return false;
  }

  return true;
}

}  // namespace equivalence
}  // namespace checker
