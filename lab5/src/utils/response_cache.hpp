#pragma once

#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace utils {

class ResponseCache {
 public:
  using Clock = std::chrono::steady_clock;

  std::optional<std::string> Get(const std::string& key) {
    const auto now = Clock::now();
    std::lock_guard lock(mutex_);

    const auto it = entries_.find(key);
    if (it == entries_.end()) {
      return std::nullopt;
    }
    if (it->second.expires_at <= now) {
      entries_.erase(it);
      return std::nullopt;
    }

    return it->second.body;
  }

  void Put(const std::string& key,
           std::string body,
           std::chrono::seconds ttl) {
    std::lock_guard lock(mutex_);
    entries_[key] = CacheEntry{std::move(body), Clock::now() + ttl};
  }

  void Invalidate(const std::string& key) {
    std::lock_guard lock(mutex_);
    entries_.erase(key);
  }

  void InvalidatePrefix(std::string_view prefix) {
    std::lock_guard lock(mutex_);
    for (auto it = entries_.begin(); it != entries_.end();) {
      if (it->first.rfind(prefix, 0) == 0) {
        it = entries_.erase(it);
      } else {
        ++it;
      }
    }
  }

 private:
  struct CacheEntry {
    std::string body;
    Clock::time_point expires_at;
  };

  std::mutex mutex_;
  std::unordered_map<std::string, CacheEntry> entries_;
};

inline ResponseCache& GlobalResponseCache() {
  static ResponseCache cache;
  return cache;
}

}  // namespace utils
