/* This is free and unencumbered software released into the public domain. */

#ifndef LMDBXX_H
#define LMDBXX_H

/**
 * <lmdb++.h> - C++11 wrapper for LMDB.
 */

#ifndef __cplusplus
#error "<lmdb++.h> requires a C++ compiler"
#endif

////////////////////////////////////////////////////////////////////////////////

#include <lmdb.h>    /* for MDB_*, mdb_*() */

#include <cstddef>   /* for std::size_t */
#include <stdexcept> /* for std::runtime_error */

namespace lmdb {
  using mode = mdb_mode_t;
}

////////////////////////////////////////////////////////////////////////////////
/* Error Handling */

namespace lmdb {
  class error;
  class key_exist_error;
  class not_found_error;
}

/**
 * Base class for LMDB exception conditions.
 */
class lmdb::error : public std::runtime_error {
protected:
  const int _code;

public:
  /**
   * Throws an error based on the given LMDB return code.
   */
  [[noreturn]] static inline void raise(const char* origin, int rc);

  /**
   * Constructor.
   */
  error(const char* const origin,
        const int rc) noexcept
    : runtime_error{origin},
      _code{rc} {}

  /**
   * Returns the underlying LMDB error code.
   */
  int code() const noexcept {
    return _code;
  }

  /**
   * Returns the origin of the LMDB error.
   */
  const char* origin() const noexcept {
    return runtime_error::what();
  }

  /**
   * Returns the underlying LMDB error code.
   */
  virtual const char* what() const noexcept {
    static thread_local char buffer[1024];
    std::snprintf(buffer, sizeof(buffer),
      "%s: %s", origin(), ::mdb_strerror(code()));
    return buffer;
  }
};

/**
 * Exception class for `MDB_KEYEXIST` errors.
 */
class lmdb::key_exist_error final : public lmdb::error {
public:
  using error::error;
};

/**
 * Exception class for `MDB_NOTFOUND` errors.
 */
class lmdb::not_found_error final : public lmdb::error {
public:
  using error::error;
};

inline void
lmdb::error::raise(const char* const origin,
                   const int rc) {
  switch (rc) {
    case MDB_KEYEXIST: throw key_exist_error{origin, rc};
    case MDB_NOTFOUND: throw not_found_error{origin, rc};
    default: throw error{origin, rc};
  }
}

////////////////////////////////////////////////////////////////////////////////
/* Procedural Interface */

namespace lmdb {
  static inline void env_create(MDB_env** env);
  static inline void env_open(MDB_env* env,
    const char* path, unsigned int flags, mode mode);
  static inline void env_close(MDB_env* env);
  static inline void env_set_flags(MDB_env* env, unsigned int flags, bool onoff);
  static inline void env_set_map_size(MDB_env* env, std::size_t size);
  static inline void env_set_max_readers(MDB_env* env, unsigned int count);
  static inline void env_set_max_dbs(MDB_env* env, MDB_dbi count);
  static inline void env_sync(MDB_env* env, bool force);
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_create(MDB_env** env) {
  const int rc = ::mdb_env_create(env);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_create", rc);
  }
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_open(MDB_env* env,
               const char* const path,
               const unsigned int flags,
               const mode mode) {
  const int rc = ::mdb_env_open(env, path, flags, mode);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_open", rc);
  }
}

/**
 * @note never throws an exception
 */
static inline void
lmdb::env_close(MDB_env* env) {
  ::mdb_env_close(env);
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_set_flags(MDB_env* env,
                    const unsigned int flags,
                    const bool onoff = true) {
  const int rc = ::mdb_env_set_flags(env, flags, onoff ? 1 : 0);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_set_flags", rc);
  }
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_set_map_size(MDB_env* env,
                       const std::size_t size) {
  const int rc = ::mdb_env_set_mapsize(env, size);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_set_mapsize", rc);
  }
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_set_max_readers(MDB_env* env,
                          const unsigned int count) {
  const int rc = ::mdb_env_set_maxreaders(env, count);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_set_maxreaders", rc);
  }
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_set_max_dbs(MDB_env* env,
                      const MDB_dbi count) {
  const int rc = ::mdb_env_set_maxdbs(env, count);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_set_maxdbs", rc);
  }
}

/**
 * @throws lmdb::error on failure
 */
static inline void
lmdb::env_sync(MDB_env* env,
               const bool force = true) {
  const int rc = ::mdb_env_sync(env, force);
  if (rc != MDB_SUCCESS) {
    error::raise("mdb_env_sync", rc);
  }
}

////////////////////////////////////////////////////////////////////////////////

#endif /* LMDBXX_H */
