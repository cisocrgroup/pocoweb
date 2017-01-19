#ifndef pcw_MockPreparedStatement_h__
#define pcw_MockPreparedStatement_h__

#include <string>
#include <vector>
#include <iomanip>

struct MockPreparedStatement {
  template<class C>
  MockPreparedStatement(const C& c): str_(c.str()), params_() {}
  void _pre_bind();
  void _bind_boolean_parameter(size_t i, const signed char* value, bool is_null) {
    static const char* NAMES[] = {"FALSE", "TRUE"};
    bind_impl(i, NAMES[not not *value], is_null);
    // std::cerr << "_bind_boolean_parameter(" << i << "," << *value << "," << is_null << ")\n";
  }
  void _bind_integral_parameter(size_t i, const int64_t* value, bool is_null) {
    bind_impl(i, *value, is_null);
    // std::cerr << "_bind_integral_parameter(" << i << "," << *value << "," << is_null << ")\n";
  }
  void _bind_floating_point_parameter(size_t i, const double* value, bool is_null) {
    bind_impl(i, *value, is_null);
    // std::cerr << "_bind_floating_point_parameter(" << i << "," << *value << "," << is_null << ")\n";
  }
  void _bind_text_parameter(size_t i, const std::string* value, bool is_null) {
    bind_impl(i, *value, is_null);
    // std::cerr << "_bind_text_parameter(" << i << "," << *value << "," << is_null << ")\n";
  }
  void _bind_date_parameter(size_t i, const ::sqlpp::chrono::day_point* value, bool is_null) {
    // bind_impl(i, *value, is_null);
    // std::cerr << "_bind_date_parameter(" << i << ",date," << is_null << ")\n";
  }
  void _bind_date_time_parameter(size_t i, const ::sqlpp::chrono::microsecond_point* value, bool is_null) {
    // bind_impl(i, *value, is_null);
    // std::cerr << "_bind_date_time_parameter(" << i << ",date-time," << is_null << ")\n";
  }
  template<class T>
  void bind_impl(size_t i, const T& t, bool is_null) {
    std::stringstream s;
    if (is_null)
      s << "NULL";
    else
      s << t;
    bind_impl(i, s);
  }
  void bind_impl(size_t i, const std::string& str, bool is_null) {
    std::stringstream s;
    if (is_null)
      s << "NULL";
    else
      s << std::quoted(str, '\'');
    bind_impl(i, s);
  }
  void bind_impl(size_t i, const std::stringstream& s) {
    if (params_.size() <= i)
      params_.resize(i + 1);
    params_[i] = s.str();
  }
  std::string str() const {
    std::string res;
    size_t i = 0;
    for (auto c: str_) {
      if (c == '?')
        res.append(params_[i++]);
      else
        res.push_back(c);
    }
    return res;
  }

private:
  std::string str_;
  std::vector<std::string> params_;
};

#endif // pcw_MockPreparedStatement_h__
// vim:sw=2
