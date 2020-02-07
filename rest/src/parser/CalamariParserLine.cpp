#include "CalamariParserLine.hpp"
#include "core/util.hpp"
#include <fstream>
#include <iostream>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
CalamariParserLine::CalamariParserLine(Path path) : path_(path), prediction_() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::fstream is(path.string(), std::ios::in | std::ios::binary);
  if (not is.good()) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: cannot open: " +
        path.string());
  }
  Predictions predictions;
  if (not predictions.ParseFromIstream(&is)) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: error parsing " +
        path.string() + ": error");
  }
  if (predictions.predictions_size() < 1) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: error parsing " +
        path.string() + ": empty predictions");
  }
  prediction_ = predictions.predictions(0);
  for (int i = 0; i < predictions.predictions_size(); i++) {
    std::cout << i << ": " << predictions.predictions(i).id() << "\n";
    std::cout << i << ": " << predictions.predictions(i).sentence() << "\n";
  }
}

void CalamariParserLine::insert(size_t i, wchar_t c) {}

void CalamariParserLine::erase(size_t i) {}
void CalamariParserLine::set(size_t i, wchar_t c) {}

////////////////////////////////////////////////////////////////////////////////
std::wstring CalamariParserLine::wstring() const { return utf8(string()); }

////////////////////////////////////////////////////////////////////////////////
std::string CalamariParserLine::string() const {
  return prediction_.sentence();
}

LinePtr CalamariParserLine::line(int id) const { return nullptr; }
