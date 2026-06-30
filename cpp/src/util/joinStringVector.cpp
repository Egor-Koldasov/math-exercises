#include <string>
#include <vector>

std::string joinStringVector(const std::vector<std::string> &stringVector,
                             const std::string &separator) {
  std::string accumString = "";
  for (int i = 0; i < stringVector.size(); i++) {
    if (i > 0) {
      accumString += separator;
    }
    accumString += stringVector.at(i);
  }
  return accumString;
}