#include "Parser.hpp"

#include "ogm/common/error.hpp"

#include <iostream>

using namespace std;

Production::~Production() {
}

string Production::to_string() {
  return "?";
}

void Production::flattenPostfixes() {
  auto it = infixes.end() - postfix_n;
  while (it != infixes.end()) {
    auto ws = *it;
    if (ws) {
      ws->postfix_n = 0;
      if (!ws->infixes.empty()) {
        auto inserted_first = infixes.insert(it + 1, std::move_iterator(ws->infixes.begin()), std::move_iterator(ws->infixes.end()));
        postfix_n += ws->infixes.size();
        ws->infixes.clear();
        it = inserted_first - 1;
      }
    }
    ++it;
  }
}

PrDecor::PrDecor(Token t, LineColumn lc): rawToken(t), Production(lc) {}

string PrDecor::to_string() {
  return *rawToken.value;
}

string PrExprParen::to_string() {
  return "(" + content->to_string() + ")";
}

PrExprArithmetic::PrExprArithmetic(PrExpression* lhs, Token op, PrExpression* rhs, LineColumn lc): lhs(lhs), op(op), rhs(rhs) {m_start = lc;}

string PrExprArithmetic::to_string() {
  return ((lhs)?(lhs->to_string() + " "):"") + op.value + ((rhs)?(" " + rhs->to_string()):"");
}
