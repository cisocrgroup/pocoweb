#ifndef pcw_ConnectionPool_hpp__
#define pcw_ConnectionPool_hpp__

#include <atomic>
#include <boost/optional.hpp>
#include <cassert>
#include <iterator>
#include <memory>
#include <mutex>
#include <tuple>

namespace pcw {
template<class Db>
struct ConnectionPoolConnection
{
  template<class... Args>
  ConnectionPoolConnection(Args&&... args)
    : db(std::forward<Args>(args)...)
    , in_use(false)
  {}

  Db db;
  std::atomic<bool> in_use;
};

template<class Db>
class Connection
{
public:
  Connection(ConnectionPoolConnection<Db>* c)
    : c_(c)
  {}

  ~Connection() noexcept
  {
    if (c_) {
      assert(c_->in_use);
      c_->in_use = false;
    }
  }

  Connection(Connection&& o) noexcept
    : c_(o.c_)
  {
    o.c_ = nullptr;
  }

  Connection& operator=(Connection&& o) noexcept
  {
    c_ = o.c_;
    o.c_ = nullptr;
    return *this;
  }

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  operator bool() const noexcept { return c_; }
  Db& db() const noexcept { return c_->db; }

private:
  using connection_ptr_t = ConnectionPoolConnection<Db>*;
  connection_ptr_t c_;
};

template<class Db, class... A>
class ConnectionPool
{
public:
  ConnectionPool(size_t n, A&&... args)
    : pool_(n)
    , args_(std::forward<A>(args)...)
    , mutex_()
  {}
  Connection<Db> get_connection();

private:
  // to avoid confusion with Connection class
  using connection_t = ConnectionPoolConnection<Db>;
  using connection_uptr_t = std::unique_ptr<connection_t>;
  using Pool = std::vector<connection_uptr_t>;
  using Args = std::tuple<A...>;
  using Mutex = std::mutex;
  using Lock = std::lock_guard<Mutex>;

  inline connection_uptr_t new_connection() const;
  template<size_t... i>
  inline connection_uptr_t new_connection(std::index_sequence<i...>) const;
  Connection<Db> get_connection(connection_uptr_t& connection) const;

  Pool pool_;
  Args args_;
  Mutex mutex_;
};
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class... A>
inline typename pcw::ConnectionPool<Db, A...>::connection_uptr_t
pcw::ConnectionPool<Db, A...>::new_connection() const
{
  constexpr auto n = std::tuple_size<typename std::decay<Args>::type>::value;
  return new_connection(std::make_index_sequence<n>{});
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class... A>
template<size_t... i>
inline typename pcw::ConnectionPool<Db, A...>::connection_uptr_t
pcw::ConnectionPool<Db, A...>::new_connection(std::index_sequence<i...>) const
{
  return std::make_unique<connection_t>(std::get<i>(args_)...);
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class... A>
pcw::Connection<Db>
pcw::ConnectionPool<Db, A...>::get_connection()
{
  using std::begin;
  using std::end;
  Lock lock(mutex_);
  auto i = std::find_if(begin(pool_), end(pool_), [](const auto& c) {
    return not c or not c->in_use;
  });
  if (i != end(pool_))
    return get_connection(*i);
  else
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
template<class Db, class... A>
pcw::Connection<Db>
pcw::ConnectionPool<Db, A...>::get_connection(
  connection_uptr_t& connection) const
{
  if (not connection)
    connection = new_connection();
  assert(connection);
  assert(connection->in_use == false);
  connection->in_use = true;
  return Connection<Db>(connection.get());
}

#endif // pcw_ConnectionPool_hpp__
