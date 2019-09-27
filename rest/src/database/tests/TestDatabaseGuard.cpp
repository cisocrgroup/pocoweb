#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DatabaseGuardTest

#include "database/ConnectionPool.hpp"
#include "database/DatabaseGuard.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace pcw;

struct Mock {
  Mock() : undo_was_called(false), dismiss_was_called(false) {}
  bool undo_was_called, dismiss_was_called;
};

class MockDatabaseGuard : public DatabaseGuardBase<Mock, MockDatabaseGuard> {
public:
  using Base = DatabaseGuardBase<Mock, MockDatabaseGuard>;
  MockDatabaseGuard(connection_t &c) : Base(c) {}
  void undo_impl(Base::connection_t &c) noexcept {
    c.db().undo_was_called = true;
  }
  void dismiss_impl(Base::connection_t &c) noexcept {
    c.db().dismiss_was_called = true;
  }
};

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UndoFunctionIsCalled) {
  ConnectionPoolConnection<Mock> m;
  m.in_use = true;
  Connection<Mock> c(&m);
  { MockDatabaseGuard guard(c); }
  BOOST_CHECK(c.db().undo_was_called);
  BOOST_CHECK(not c.db().dismiss_was_called);
}

////////////////////////////////////////////////////////////////////////////////
BOOST_AUTO_TEST_CASE(UndoFunctionIsNotCalled) {
  ConnectionPoolConnection<Mock> m;
  m.in_use = true;
  Connection<Mock> c(&m);
  MockDatabaseGuard guard(c);
  guard.dismiss();
  BOOST_CHECK(not c.db().undo_was_called);
  BOOST_CHECK(c.db().dismiss_was_called);
}
