/* Copyright (c) 2019 The Brave Software Team. Distributed under the MPL2
 * license. This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "etld/parser.h"
#include "etld/public_suffix_rule.h"
#include "etld/types.h"

namespace Brave {
namespace eTLD {

void PublicSuffixParseResult::AddParseResult(
    const PublicSuffixTextLineParseResult &result) {
  switch (result.type) {
    case PublicSuffixTextLineTypeRule:
      rules_.push_back(result.rule);
      break;

    case PublicSuffixTextLineTypeWhitespace:
      num_whitespace_lines_ += 1;
      break;

    case PublicSuffixTextLineTypeComment:
      num_comment_lines_ += 1;
      break;

    case PublicSuffixTextLineTypeInvalidRule:
      num_invalid_rules_ += 1;
      break;

    case PublicSuffixTextLineTypeNone:
      break;
  }
}

PublicSuffixParseResult parse_rule_file(std::ifstream &rule_file) {
  std::vector<PublicSuffixRule> rules;
  std::string line;
  PublicSuffixParseResult results;
  PublicSuffixTextLineParseResult line_result;
  while (std::getline(rule_file, line)) {
    line_result = parse_rule_line(line);
    results.AddParseResult(line_result);
  }

  return results;
}

PublicSuffixParseResult parse_rule_text(const std::string &text) {
  std::istringstream stream(text);
  std::string line;
  PublicSuffixParseResult results;
  PublicSuffixTextLineParseResult line_result;
  while (std::getline(stream, line)) {
    line_result = parse_rule_line(line);
    results.AddParseResult(line_result);
  }

  return results;
}

PublicSuffixTextLineParseResult parse_rule_line(const std::string &line) {
  PublicSuffixTextLineParseResult result;

  // Check to see if this is a comment line.  If so, process no further.
  if (line.find("//") == 0) {
    result.type = PublicSuffixTextLineTypeComment;
    return result;
  }

  std::size_t first_non_white_space_char = line.find_first_not_of(" ");
  // Next, check to see if the line is only whitespace.
  if (first_non_white_space_char == std::string::npos) {
    result.type = PublicSuffixTextLineTypeWhitespace;
    return result;
  }

  try {
    result.rule = PublicSuffixRule(line);
    result.type = PublicSuffixTextLineTypeRule;
  } catch (PublicSuffixRuleInputException error) {
    result.type = PublicSuffixTextLineTypeInvalidRule;
  }

  return result;
}

}  // namespace eTLD
}  // namespace Brave
