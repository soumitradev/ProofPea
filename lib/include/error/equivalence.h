#ifndef CNF_CONVERTOR_ERROR_EQUIVALENCE
#define CNF_CONVERTOR_ERROR_EQUIVALENCE

#include <error/error.h>

#include <string>

namespace proofpea {
namespace error {
namespace equivalence {

struct equivalence_chack_unsupported : error::error {
  equivalence_chack_unsupported(std::string c);
};

struct incomplete_truth_table : error::error {
  incomplete_truth_table(std::string c);
};

}  // namespace equivalence
}  // namespace error
}  // namespace proofpea

#endif  // CNF_CONVERTOR_ERROR_EQUIVALENCE
