#include "CalamariParserLine.hpp"
#include "core/LineBuilder.hpp"
#include "core/util.hpp"
#include <fstream>
#include <iostream>

using namespace pcw;

////////////////////////////////////////////////////////////////////////////////
CalamariParserLine::CalamariParserLine(Path proto, Path img)
    : proto_(proto), img_(img), prediction_() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::fstream is(proto_.string(), std::ios::in | std::ios::binary);
  if (not is.good()) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: cannot open: " +
        proto_.string());
  }
  Predictions predictions;
  if (not predictions.ParseFromIstream(&is)) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: error parsing " +
        proto_.string() + ": error");
  }
  if (predictions.predictions_size() == 0) {
    throw std::runtime_error(
        "cannot parse predictions from protobuf: error parsing " +
        proto_.string() + ": empty predictions");
  }
  prediction_ = predictions.predictions(0);
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

////////////////////////////////////////////////////////////////////////////////
LinePtr CalamariParserLine::line(int id) const {
  LineBuilder builder;
  builder.set_image_path(img_);
  builder.set_id(id);
  builder.set_box(Box{});
  for (int i = 0; i < prediction_.positions_size(); i++) {
    const auto pos = prediction_.positions(i);
    if (pos.chars_size() == 0) {
      continue;
    }
    const auto c = pos.chars(0);
    builder.append(c.char_(), pos.global_end(), c.probability());
  }
  return builder.build();
}
