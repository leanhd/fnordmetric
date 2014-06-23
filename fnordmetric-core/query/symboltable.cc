/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "symboltable.h"


namespace fnordmetric {
namespace query {

static std::unordered_map<std::string, const SymbolTableEntry*> global_symbols_;

const SymbolTableEntry* lookupSymbol(const std::string& symbol) {
  auto iter = global_symbols_.find(symbol);

  if (iter == global_symbols_.end()) {
    return nullptr;
  } else {
    return iter->second;
  }
}

int SymbolTable::lookup(const std::string& symbol, bool allow_aggregate) {
  auto entry = lookupSymbol(symbol);

  if (!entry->isAggregate()) {
    for (int i=0; i < table_.size(); ++i) {
      if (table_[i].first == entry) {
        return i;
      }
    }
  }

  table_.emplace_back(entry, nullptr);
  return table_.size() - 1;
}

SymbolTableEntry::SymbolTableEntry(
    const std::string& symbol,
    SValue* (*method)(void**, int, SValue**),
    bool is_aggregate) :
    call_(method),
    is_aggregate_(is_aggregate) {
  assert(global_symbols_.find(symbol) == global_symbols_.end());
  global_symbols_[symbol] = this;
}

bool SymbolTableEntry::isAggregate() const {
  return is_aggregate_;
}

}
}