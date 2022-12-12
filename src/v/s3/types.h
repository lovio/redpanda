/*
 * Copyright 2022 Redpanda Data, Inc.
 *
 * Licensed as a Redpanda Enterprise file under the Redpanda Community
 * License (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * https://github.com/redpanda-data/redpanda/blob/master/licenses/rcl.md
 */

#pragma once

#include <system_error>

namespace s3 {

enum class error_outcome {
    none = 0,
    /// Error condition that could be retried
    retry,
    /// The service asked us to retry (SlowDown response)
    retry_slowdown,
    /// Error condition that couldn't be retried
    fail,
    /// NotFound API error (only suitable for downloads)
    notfound
};

struct error_outcome_category final : public std::error_category {
    const char* name() const noexcept final { return "s3::error_outcome"; }

    std::string message(int c) const final {
        switch (static_cast<error_outcome>(c)) {
        case error_outcome::none:
            return "No error";
        case error_outcome::retry:
            return "Retryable error";
        case error_outcome::retry_slowdown:
            return "Cloud service asked us to slow down";
        case error_outcome::fail:
            return "Non retriable error";
        case error_outcome::notfound:
            return "Key not found error";
        default:
            return "Undefined error_outcome encountered";
        }
    }
};

inline const std::error_category& error_category() noexcept {
    static error_outcome_category e;
    return e;
}

inline std::error_code make_error_code(error_outcome e) noexcept {
    return {static_cast<int>(e), error_category()};
}

} // namespace s3

namespace std {
template<>
struct is_error_code_enum<s3::error_outcome> : true_type {};
} // namespace std
