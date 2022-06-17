#ifndef RWLOCK_HPP
#define RWLOCK_HPP
#include <shared_mutex>
namespace mdc {
  using rw_lock = std::shared_mutex;
  using read_lock = std::shared_lock<rw_lock>;
  using write_lock = std::lock_guard<rw_lock>;
}
#endif // RWLOCK_HPP
