#ifndef RECORD_HELPER_H
#define RECORD_HELPER_H

#pragma once

#include <tuple>

// ============================================================================
// Preprocessor utilities
// ============================================================================

#define LIGARIUM_PP_CAT(a, b) LIGARIUM_PP_CAT_I(a, b)

#define LIGARIUM_PP_CAT_I(a, b) a##b


#define LIGARIUM_PP_NARG(...) LIGARIUM_PP_NARG_I(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define LIGARIUM_PP_NARG_I(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N


#define LIGARIUM_PP_FOR_EACH(macro, ...)                                                                               \
  LIGARIUM_PP_CAT(LIGARIUM_PP_FOR_EACH_, LIGARIUM_PP_NARG(__VA_ARGS__))(macro, __VA_ARGS__)


#define LIGARIUM_PP_FOR_EACH_1(m, a) m a

#define LIGARIUM_PP_FOR_EACH_2(m, a, ...) m a LIGARIUM_PP_FOR_EACH_1(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_3(m, a, ...) m a LIGARIUM_PP_FOR_EACH_2(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_4(m, a, ...) m a LIGARIUM_PP_FOR_EACH_3(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_5(m, a, ...) m a LIGARIUM_PP_FOR_EACH_4(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_6(m, a, ...) m a LIGARIUM_PP_FOR_EACH_5(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_7(m, a, ...) m a LIGARIUM_PP_FOR_EACH_6(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_8(m, a, ...) m a LIGARIUM_PP_FOR_EACH_7(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_9(m, a, ...) m a LIGARIUM_PP_FOR_EACH_8(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_10(m, a, ...) m a LIGARIUM_PP_FOR_EACH_9(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_11(m, a, ...) m a LIGARIUM_PP_FOR_EACH_10(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_12(m, a, ...) m a LIGARIUM_PP_FOR_EACH_11(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_13(m, a, ...) m a LIGARIUM_PP_FOR_EACH_12(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_14(m, a, ...) m a LIGARIUM_PP_FOR_EACH_13(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_15(m, a, ...) m a LIGARIUM_PP_FOR_EACH_14(m, __VA_ARGS__)

#define LIGARIUM_PP_FOR_EACH_16(m, a, ...) m a LIGARIUM_PP_FOR_EACH_15(m, __VA_ARGS__)


// ============================================================================
// FIELD tuple helpers
//
// FIELD(QString, name)
// becomes:
//     type = QString
//     name = name
// ============================================================================

#define LIGARIUM_FIELD(type, name) type name;


// ============================================================================
// SQL field generation
// ============================================================================

#define LIGARIUM_SQL_FIELD(Class, type, name) Ligarium::field(u## #name, &Class::name),


// ============================================================================
// Equality generation
// ============================================================================

#define LIGARIUM_EQUAL_FIELD(type, name) &&lhs.name == rhs.name


// ============================================================================
// FIELD filtering
//
// We need to distinguish FIELD(...) from DUMP(...).
// ============================================================================

#define LIGARIUM_FIELD(type, name) LIGARIUM_FIELD_IMPL(type, name)

#define LIGARIUM_FIELD_IMPL(type, name) type name;


// ============================================================================
// DUMP
// ============================================================================

#define LIGARIUM_DUMP(name) return name;


// ============================================================================
// Main record macro
// ============================================================================

#define LIGARIUM_RECORD(Class, TableValue, Dump, ...) LIGARIUM_RECORD_IMPL(Class, TableValue, Dump, __VA_ARGS__)


#define LIGARIUM_RECORD_IMPL(Class, TableValue, Dump, ...)                                                             \
  class Class final : public Ligarium::Record<Class>                                                                   \
  {                                                                                                                    \
  public:                                                                                                              \
    using Ligarium::Record<Class>::Record;                                                                             \
                                                                                                                       \
    static constexpr Ligarium::Table static_table = TableValue;                                                        \
                                                                                                                       \
    LIGARIUM_PP_FOR_EACH(LIGARIUM_FIELD, __VA_ARGS__)                                                                  \
                                                                                                                       \
    [[nodiscard]]                                                                                                      \
    QString dump() const override                                                                                      \
    {                                                                                                                  \
      LIGARIUM_DUMP(Dump)                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    static constexpr auto sql_fields()                                                                                 \
    {                                                                                                                  \
      return std::tuple{LIGARIUM_PP_FOR_EACH(LIGARIUM_SQL_FIELD(Class), __VA_ARGS__)};                                 \
    }                                                                                                                  \
                                                                                                                       \
    friend bool operator==(const Class& lhs, const Class& rhs)                                                         \
    {                                                                                                                  \
      return lhs.id() == rhs.id() LIGARIUM_PP_FOR_EACH(LIGARIUM_EQUAL_FIELD, __VA_ARGS__);                             \
    }                                                                                                                  \
  };

#endif