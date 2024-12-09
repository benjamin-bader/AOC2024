#pragma once

#include "base.h"

#include <map>
#include <memory>
#include <string>
#include <utility>

extern std::map<std::pair<int, int>, std::shared_ptr<Problem>> solutions;
extern std::map<std::pair<int, int>, std::string> expected_answers;

void register_solutions();
