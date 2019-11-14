#ifndef pcw_iterators_hpp__
#define pcw_iterators_hpp__

namespace pcw {
template <class It> class const_contents_cor_iterator {
public:
  const_contents_cor_iterator(It b, It e) : b_(b), e_(e) { skip(); }
  const auto &operator++() { // prefix ++it
    b_++;
    skip();
    return *this;
  }
  auto operator++(int) { // postfix it++
    const_contents_cor_iterator<It> ret(*this);
    ++(*this);
    return ret;
  }
  auto operator-> () { return b_.operator->(); }

  bool equals(const const_contents_cor_iterator<It> &other) const noexcept {
    return this->b_ == other.b_;
  }

private:
  // Skip deletions in ocr (cor < 0) but not
  // insertions (ocr < 0).
  void skip() {
    for (; b_ != e_; ++b_) {
      if (b_->cor > 0) {
        return;
      }
    }
  }
  It b_, e_;
};

////////////////////////////////////////////////////////////////////////////////
template <class It>
bool operator==(const const_contents_cor_iterator<It> &lhs,
                const const_contents_cor_iterator<It> &rhs) {
  return lhs.equals(rhs);
}

////////////////////////////////////////////////////////////////////////////////
template <class It>
bool operator!=(const const_contents_cor_iterator<It> &lhs,
                const const_contents_cor_iterator<It> &rhs) {
  return not operator==(lhs, rhs);
}

////////////////////////////////////////////////////////////////////////////////
template <class Rows> auto make_const_contents_cor_iterator(Rows &rows) {
  return const_contents_cor_iterator<typename Rows::iterator>(rows.begin(),
                                                              rows.end());
}

////////////////////////////////////////////////////////////////////////////////
template <class Rows> auto make_const_contents_cor_end_iterator(Rows &rows) {
  return const_contents_cor_iterator<typename Rows::iterator>(rows.end(),
                                                              rows.end());
}

} // namespace pcw

#endif // pcw_iterators_hpp__
