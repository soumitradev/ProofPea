#ifndef CNF_CONVERTOR_ERROR_EVAL
#define CNF_CONVERTOR_ERROR_EVAL

#include <error/error.h>

#include <string>

namespace proofpea {
namespace error {
namespace eval {

struct unexpected_node : error::error {
  unexpected_node(std::string c);
};
struct mismatched_atoms : error::error {
  mismatched_atoms(std::string c);
};

}  // namespace eval
}  // namespace error
}  // namespace proofpea

#endif  // CNF_CONVERTOR_ERROR_EVAL
