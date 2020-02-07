#include "CalamariParserLine.hpp"
#include "core/LineBuilder.hpp"
#include "core/Pix.hpp"
#include "core/util.hpp"
#include "utils/Error.hpp"
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

////////////////////////////////////////////////////////////////////////////////
void CalamariParserLine::insert(size_t i, wchar_t c) {
  const auto ps = prediction_.mutable_positions();
  const auto pos = ps->Add();
  pos->set_local_start(ps->Mutable(i)->local_start());
  pos->set_local_end(ps->Mutable(i)->local_end());
  pos->set_global_start(ps->Mutable(i)->global_start());
  pos->set_global_end(ps->Mutable(i)->global_end());
  const auto chr = pos->mutable_chars()->Add();
  chr->set_char_(utf8(std::wstring(1, c)));
  chr->set_probability(1.0);

  // insert last element to position i
  // 0 i    2 3 last (the new element is at position *last*)
  // 0 last 2 3 i    (swap i last)
  // 0 last i 3 2    (swap i+1 last)
  // 0 last i 2 3    (swap i+2 last)
  const int last = ps->size() - 1;
  for (int j = int(i); j < last; j++) {
    ps->SwapElements(j, last);
  }
}

////////////////////////////////////////////////////////////////////////////////
void CalamariParserLine::erase(size_t i) {
  prediction_.mutable_positions()->erase(
      prediction_.mutable_positions()->begin() + i);
}

////////////////////////////////////////////////////////////////////////////////
void CalamariParserLine::set(size_t i, wchar_t c) {
  prediction_.mutable_positions(int(i))->mutable_chars(0)->set_char_(
      utf8(std::wstring(1, c)));
  prediction_.mutable_positions(int(i))->mutable_chars(0)->set_probability(1.0);
}

////////////////////////////////////////////////////////////////////////////////
void CalamariParserLine::end_wagner_fischer() {
  std::string sentence;
  for (int i = 0; i < prediction_.positions_size(); i++) {
    const auto pos = prediction_.positions(i);
    if (pos.chars_size() == 0) {
      continue;
    }
    sentence.append(pos.chars(0).char_());
  }
  prediction_.set_sentence(sentence);
}

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

  PixPtr pix;
  pix.reset(pixRead(img_.string().data()));
  if (not pix) {
    THROW(Error,
          "(CalamariParserLine) cannot read image file: ", img_.string());
  }
  Box box;
  box.set_left(0);
  box.set_top(0);
  box.set_right(static_cast<int>(pix->w));
  box.set_bottom(static_cast<int>(pix->h));
  builder.set_box(box);

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
