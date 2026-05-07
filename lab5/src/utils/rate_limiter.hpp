#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <mutex>
#include <string>
#include <unordered_map>

namespace utils {

struct RateLimitDecision {
  bool allowed;
  int limit;
  int remaining;
  int reset_after_seconds;
};

class TokenBucketRateLimiter {
 public:
  using Clock = std::chrono::steady_clock;

  RateLimitDecision Check(const std::string& key,
                          int capacity,
                          double refill_per_second) {
    const auto now = Clock::now();
    std::lock_guard lock(mutex_);

    auto& bucket = buckets_[key];
    if (bucket.last_refill == Clock::time_point{}) {
      bucket.tokens = capacity;
      bucket.last_refill = now;
    }

    const auto elapsed =
        std::chrono::duration<double>(now - bucket.last_refill).count();
    bucket.tokens = std::min<double>(
        capacity, bucket.tokens + elapsed * refill_per_second);
    bucket.last_refill = now;

    const bool allowed = bucket.tokens >= 1.0;
    if (allowed) {
      bucket.tokens -= 1.0;
    }

    const double tokens_to_reset = allowed ? capacity - bucket.tokens
                                           : 1.0 - bucket.tokens;
    const auto reset_after_seconds = std::max(
        1, static_cast<int>(std::ceil(tokens_to_reset / refill_per_second)));

    return RateLimitDecision{
        allowed,
        capacity,
        std::max(0, static_cast<int>(std::floor(bucket.tokens))),
        reset_after_seconds};
  }

 private:
  struct Bucket {
    double tokens = 0.0;
    Clock::time_point last_refill{};
  };

  std::mutex mutex_;
  std::unordered_map<std::string, Bucket> buckets_;
};

inline TokenBucketRateLimiter& GlobalRateLimiter() {
  static TokenBucketRateLimiter limiter;
  return limiter;
}

}  // namespace utils
