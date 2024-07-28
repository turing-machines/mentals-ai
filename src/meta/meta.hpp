/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <version>

//
//
//

#if !defined(META_HPP_VERSION_MAJOR)
#    define META_HPP_VERSION_MAJOR 0
#endif

#if !defined(META_HPP_VERSION_MINOR)
#    define META_HPP_VERSION_MINOR 0
#endif

#if !defined(META_HPP_VERSION_PATCH)
#    define META_HPP_VERSION_PATCH 1
#endif

//
//
//

#if META_HPP_VERSION_MINOR < 0 || META_HPP_VERSION_MINOR > 99
#    error "meta.hpp: incorrect minor version number"
#endif

#if META_HPP_VERSION_PATCH < 0 || META_HPP_VERSION_PATCH > 99
#    error "meta.hpp: incorrect patch version number"
#endif

#define META_HPP_VERSION META_HPP_VERSION_MAJOR * 10000 + META_HPP_VERSION_MINOR * 100 + META_HPP_VERSION_PATCH

//
//
//

#if !defined(META_HPP_NO_EXCEPTIONS) && !defined(__cpp_exceptions)
#    define META_HPP_NO_EXCEPTIONS
#endif

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#    define META_HPP_NO_RTTI
#endif

//
//
//

#if !defined(META_HPP_FWD)
#    define META_HPP_FWD(v) std::forward<decltype(v)>(v)
#endif

#if !defined(META_HPP_ASSERT)
#    define META_HPP_ASSERT(...) assert(__VA_ARGS__) // NOLINT
#endif

#if !defined(META_HPP_DEV_ASSERT)
#    if defined(META_HPP_SANITIZERS)
#        define META_HPP_DEV_ASSERT(...) META_HPP_ASSERT(__VA_ARGS__)
#    else
#        define META_HPP_DEV_ASSERT(...) (void)0
#    endif
#endif

//
//
//

#define META_HPP_PP_CAT(x, y) META_HPP_PP_CAT_I(x, y)
#define META_HPP_PP_CAT_I(x, y) x##y

//
//
//

#define META_HPP_DETAIL_CLANG_COMPILER_ID 1
#define META_HPP_DETAIL_GCC_COMPILER_ID 2
#define META_HPP_DETAIL_MSVC_COMPILER_ID 3
#define META_HPP_DETAIL_UNKNOWN_COMPILER_ID 4

#if defined(__clang__)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_CLANG_COMPILER_ID
#elif defined(__GNUC__)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_GCC_COMPILER_ID
#elif defined(_MSC_VER)
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_MSVC_COMPILER_ID
#else
#    define META_HPP_DETAIL_COMPILER_ID META_HPP_DETAIL_UNKNOWN_COMPILER_ID
#endif

//
//
//

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
#    define META_HPP_ALLOCA(size) _alloca(size)
#else
#    define META_HPP_ALLOCA(size) __builtin_alloca(size)
#endif

//
//
//

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_CLANG_COMPILER_ID
#    define META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNING(w) META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(clang diagnostic ignored w)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() _Pragma("clang diagnostic push")
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP() _Pragma("clang diagnostic pop")
#else
#    define META_HPP_DETAIL_CLANG_PRAGMA_TO_STR(x)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_GCC_COMPILER_ID
#    define META_HPP_DETAIL_GCC_PRAGMA_TO_STR(x) _Pragma(#x)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNING(w) META_HPP_DETAIL_GCC_PRAGMA_TO_STR(GCC diagnostic ignored w)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_PUSH() _Pragma("GCC diagnostic push")
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_POP() _Pragma("GCC diagnostic pop")
#else
#    define META_HPP_DETAIL_GCC_PRAGMA_TO_STR(x)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_GCC_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_DETAIL_COMPILER_ID == META_HPP_DETAIL_MSVC_COMPILER_ID
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNING(w) __pragma(warning(disable : w))
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_PUSH() __pragma(warning(push))
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_POP() __pragma(warning(pop))
#else
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNING(w)
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_PUSH()
#    define META_HPP_DETAIL_MSVC_IGNORE_WARNINGS_POP()
#endif

//
//
//

#define META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Walloca")

#define META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP() META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()

//
//
//

#define META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_PUSH() \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wunknown-warning-option") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Winconsistent-missing-override") \
    META_HPP_DETAIL_CLANG_IGNORE_WARNING("-Wsuggest-override")

#define META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_POP() META_HPP_DETAIL_CLANG_IGNORE_WARNINGS_POP()

namespace meta_hpp::detail
{
    template < typename Enum >
    class bitflags final {
        static_assert(std::is_enum_v<Enum>);

    public:
        using enum_type = Enum;
        using underlying_type = std::underlying_type_t<Enum>;

        bitflags() = default;
        ~bitflags() = default;

        bitflags(bitflags&&) = default;
        bitflags(const bitflags&) = default;

        bitflags& operator=(bitflags&&) = default;
        bitflags& operator=(const bitflags&) = default;

        constexpr bitflags(enum_type flags)
        : flags_(static_cast<underlying_type>(flags)) {}

        constexpr explicit bitflags(underlying_type flags)
        : flags_(flags) {}

        constexpr void swap(bitflags& other) noexcept {
            using std::swap;
            swap(flags_, other.flags_);
        }

        [[nodiscard]] constexpr explicit operator bool() const noexcept {
            return !!flags_;
        }

        [[nodiscard]] constexpr underlying_type as_raw() const noexcept {
            return flags_;
        }

        [[nodiscard]] constexpr enum_type as_enum() const noexcept {
            return static_cast<enum_type>(flags_);
        }

        [[nodiscard]] constexpr bool has(bitflags flags) const noexcept {
            return flags.flags_ == (flags_ & flags.flags_);
        }

        constexpr bitflags& set(bitflags flags) noexcept {
            flags_ |= flags.flags_;
            return *this;
        }

        constexpr bitflags& toggle(bitflags flags) noexcept {
            flags_ ^= flags.flags_;
            return *this;
        }

        constexpr bitflags& clear(bitflags flags) noexcept {
            flags_ &= ~flags.flags_;
            return *this;
        }

    private:
        underlying_type flags_{};
    };

    template < typename Enum >
    constexpr void swap(bitflags<Enum>& l, bitflags<Enum>& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template < typename Enum >
    struct hash<meta_hpp::detail::bitflags<Enum>> {
        size_t operator()(meta_hpp::detail::bitflags<Enum> bf) const noexcept {
            return hash<Enum>{}(bf.as_enum());
        }
    };
}

namespace meta_hpp::detail
{
#define META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(op) \
    template < typename Enum > \
    constexpr bool operator op(Enum l, bitflags<Enum> r) noexcept { \
        return l op r.as_enum(); \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, Enum r) noexcept { \
        return l.as_enum() op r; \
    } \
    template < typename Enum > \
    constexpr bool operator op(std::underlying_type_t<Enum> l, bitflags<Enum> r) noexcept { \
        return l op r.as_raw(); \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, std::underlying_type_t<Enum> r) noexcept { \
        return l.as_raw() op r; \
    } \
    template < typename Enum > \
    constexpr bool operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept { \
        return l.as_raw() op r.as_raw(); \
    }
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(<)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(>)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(<=)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(>=)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(==)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(!=)
#undef META_HPP_DETAIL_DEFINE_BINARY_OPERATOR
}

namespace meta_hpp::detail
{
    template < typename Enum >
    constexpr bitflags<Enum> operator~(bitflags<Enum> l) noexcept {
        return static_cast<Enum>(~l.as_raw());
    }

#define META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(op) \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(Enum l, bitflags<Enum> r) noexcept { \
        return bitflags{l} op r; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, Enum r) noexcept { \
        return l op bitflags<Enum>{r}; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept { \
        return static_cast<Enum>(l.as_raw() op r.as_raw()); \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, Enum r) noexcept { \
        return l = l op bitflags<Enum>{r}; \
    } \
    template < typename Enum > \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, bitflags<Enum> r) noexcept { \
        return l = l op r; \
    }
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(|)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(&)
    META_HPP_DETAIL_DEFINE_BINARY_OPERATOR(^)
#undef META_HPP_DETAIL_DEFINE_BINARY_OPERATOR
}

//
// META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL
//

#define META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(Enum) \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator~[[maybe_unused]] (Enum l) noexcept { \
        return ~::meta_hpp::detail::bitflags<Enum>(l); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator| [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) | ::meta_hpp::detail::bitflags<Enum>(r); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator& [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) & ::meta_hpp::detail::bitflags<Enum>(r); \
    } \
    constexpr ::meta_hpp::detail::bitflags<Enum> operator^ [[maybe_unused]] (Enum l, Enum r) noexcept { \
        return ::meta_hpp::detail::bitflags<Enum>(l) ^ ::meta_hpp::detail::bitflags<Enum>(r); \
    }

namespace meta_hpp::detail
{
    template < typename From >
    struct cv_traits {
        static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
        static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

        template < bool yesno, template < typename > typename Q, typename V >
        using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

        // clang-format off
        template < typename To >
        using add_to =
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>;
        // clang-format on

        template < typename To >
        using copy_to = add_to<std::remove_cv_t<To>>;
    };

    template < typename From, typename To >
    struct add_cv {
        using type = typename cv_traits<From>::template add_to<To>;
    };

    template < typename From, typename To >
    struct copy_cv {
        using type = typename cv_traits<From>::template copy_to<To>;
    };

    template < typename From, typename To >
    using add_cv_t = typename add_cv<From, To>::type;

    template < typename From, typename To >
    using copy_cv_t = typename copy_cv<From, To>::type;
}

namespace meta_hpp::detail
{
    template < typename From >
    struct cvref_traits {
        static constexpr bool is_lvalue = std::is_lvalue_reference_v<From>;
        static constexpr bool is_rvalue = std::is_rvalue_reference_v<From>;
        static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
        static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

        template < bool yesno, template < typename > typename Q, typename V >
        using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

        // clang-format off
        template < typename To >
        using add_to =
            apply_t_if<is_lvalue, std::add_lvalue_reference_t,
            apply_t_if<is_rvalue, std::add_rvalue_reference_t,
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>>>;
        // clang-format on

        template < typename To >
        using copy_to = add_to<std::remove_cvref_t<To>>;
    };

    template < typename From, typename To >
    struct add_cvref {
        using type = typename cvref_traits<From>::template add_to<To>;
    };

    template < typename From, typename To >
    struct copy_cvref {
        using type = typename cvref_traits<From>::template copy_to<To>;
    };

    template < typename From, typename To >
    using add_cvref_t = typename add_cvref<From, To>::type;

    template < typename From, typename To >
    using copy_cvref_t = typename copy_cvref<From, To>::type;
}

namespace meta_hpp::detail
{
    enum class error_code {
        no_error,

        bad_cast,

        bad_const_access,
        bad_uvalue_access,

        bad_argument_cast,
        bad_instance_cast,

        arity_mismatch,
        instance_type_mismatch,
        argument_type_mismatch,
    };

    inline const char* get_error_code_message(error_code error) noexcept {
        switch ( error ) {
        case error_code::no_error:
            return "no error";
        case error_code::bad_cast:
            return "bad cast";
        case error_code::bad_const_access:
            return "bad const access";
        case error_code::bad_uvalue_access:
            return "bad uvalue access";
        case error_code::bad_argument_cast:
            return "bad argument cast";
        case error_code::bad_instance_cast:
            return "bad instance cast";
        case error_code::arity_mismatch:
            return "arity mismatch";
        case error_code::instance_type_mismatch:
            return "instance type mismatch";
        case error_code::argument_type_mismatch:
            return "argument type mismatch";
        }

        META_HPP_ASSERT(false);
        return "unexpected error code";
    }
}

namespace meta_hpp::detail
{
    class exception final : public std::exception {
    public:
        explicit exception(error_code error)
        : error_{error} {}

        [[nodiscard]] error_code get_error() const noexcept {
            return error_;
        }

        [[nodiscard]] const char* what() const noexcept override {
            return get_error_code_message(error_);
        }

    private:
        error_code error_{};
    };

    [[noreturn]] inline void throw_exception(error_code err) {
#if !defined(META_HPP_NO_EXCEPTIONS)
        throw exception{err};
#else
        (void)err;
        std::abort();
#endif
    }
}

namespace meta_hpp::detail
{
    template < typename Function >
    class fixed_function;

    template < typename R, typename... Args >
    class fixed_function<R(Args...)> final {
    public:
        using result_type = R;

        fixed_function() = default;

        ~fixed_function() {
            reset();
        }

        fixed_function(const fixed_function& other) = delete;
        fixed_function& operator=(const fixed_function& other) = delete;

        fixed_function(fixed_function&& other) noexcept {
            if ( other.vtable_ ) {
                other.vtable_->move(other, *this);
            }
        }

        fixed_function& operator=(fixed_function&& other) noexcept {
            if ( this != &other ) {
                fixed_function{std::move(other)}.swap(*this);
            }
            return *this;
        }

        template <                                    //
            typename F,                               //
            typename Fp = std::decay_t<F>,            //
            typename = std::enable_if_t<              //
                !std::is_same_v<Fp, fixed_function>>> //
        fixed_function(F&& fun) {
            vtable_t::construct(*this, std::forward<F>(fun));
        }

        template <                                    //
            typename F,                               //
            typename Fp = std::decay_t<F>,            //
            typename = std::enable_if_t<              //
                !std::is_same_v<Fp, fixed_function>>> //
        fixed_function& operator=(F&& fun) {
            fixed_function{std::forward<F>(fun)}.swap(*this);
            return *this;
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return vtable_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        R operator()(Args... args) const {
            META_HPP_ASSERT(vtable_ && "bad function call");
            return vtable_->call(*this, std::forward<Args>(args)...);
        }

        void reset() noexcept {
            if ( vtable_ ) {
                vtable_->destroy(*this);
            }
        }

        void swap(fixed_function& other) noexcept {
            vtable_t::swap(*this, other);
        }

    private:
        struct vtable_t;
        vtable_t* vtable_{};

    private:
        struct buffer_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            alignas(std::max_align_t) std::byte data[sizeof(void*) * 3];
        } buffer_{};
    };

    template < typename Function >
    void swap(fixed_function<Function>& l, fixed_function<Function>& r) noexcept {
        l.swap(r);
    }
}

namespace meta_hpp::detail
{
    template < typename R, typename... Args >
    struct fixed_function<R(Args...)>::vtable_t final {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)

        R (*const call)(const fixed_function& self, Args... args);
        void (*const move)(fixed_function& from, fixed_function& to) noexcept;
        void (*const destroy)(fixed_function& self);

        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < typename T >
        static T* buffer_cast(buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(buffer.data));
        }

        template < typename T >
        static const T* buffer_cast(const buffer_t& buffer) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(buffer.data));
        }

        template < typename Fp >
        static vtable_t* get() {
            static_assert(std::is_same_v<Fp, std::decay_t<Fp>>);

            static vtable_t table{
                .call{[](const fixed_function& self, Args... args) -> R {
                    META_HPP_DEV_ASSERT(self);

                    const Fp& src = *buffer_cast<Fp>(self.buffer_);
                    return std::invoke(src, std::forward<Args>(args)...);
                }},

                .move{[](fixed_function& from, fixed_function& to) noexcept {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(from);

                    Fp& src = *buffer_cast<Fp>(from.buffer_);
                    std::construct_at(buffer_cast<Fp>(to.buffer_), std::move(src));
                    std::destroy_at(&src);

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                }},

                .destroy{[](fixed_function& self) {
                    META_HPP_DEV_ASSERT(self);

                    Fp& src = *buffer_cast<Fp>(self.buffer_);
                    std::destroy_at(&src);

                    self.vtable_ = nullptr;
                }},
            };
            return &table;
        }

        template < typename F, typename Fp = std::decay_t<F> >
        static void construct(fixed_function& dst, F&& fun) {
            META_HPP_DEV_ASSERT(!dst);

            static_assert(sizeof(Fp) <= sizeof(buffer_t));
            static_assert(alignof(buffer_t) % alignof(Fp) == 0);
            static_assert(std::is_invocable_r_v<R, Fp, Args...>);
            static_assert(std::is_nothrow_move_constructible_v<Fp>);

            std::construct_at(buffer_cast<Fp>(dst.buffer_), std::forward<F>(fun));

            dst.vtable_ = vtable_t::get<Fp>();
        }

        static void swap(fixed_function& l, fixed_function& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                fixed_function temp;
                r.vtable_->move(r, temp);
                l.vtable_->move(l, r);
                temp.vtable_->move(temp, l);
            } else {
                if ( l ) {
                    l.vtable_->move(l, r);
                } else {
                    r.vtable_->move(r, l);
                }
            }
        }
    };
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename F >
        struct strip_signature_impl;

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...)> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) const> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...)&> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) const&> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) noexcept> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) const noexcept> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) & noexcept> {
            using type = R(Args...);
        };

        template < typename R, typename C, typename... Args >
        struct strip_signature_impl<R (C::*)(Args...) const & noexcept> {
            using type = R(Args...);
        };

        template < typename F >
        using strip_signature_impl_t = typename strip_signature_impl<F>::type;
    }

    template < typename R, typename... Args >
    fixed_function(R (*)(Args...)) -> fixed_function<R(Args...)>;

    template < typename F, typename S = impl::strip_signature_impl_t<decltype(&F::operator())> >
    fixed_function(F) -> fixed_function<S>;
}

namespace meta_hpp::detail
{
    // REFERENCE:
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

    template < std::size_t SizeBytes = sizeof(std::size_t) >
    struct fnv1a_hash_traits;

    template <>
    struct fnv1a_hash_traits<sizeof(std::uint32_t)> {
        using underlying_type = std::uint32_t;
        static inline constexpr underlying_type prime{16777619U};
        static inline constexpr underlying_type offset_basis{2166136261U};
    };

    template <>
    struct fnv1a_hash_traits<sizeof(std::uint64_t)> {
        using underlying_type = std::uint64_t;
        static inline constexpr underlying_type prime{1099511628211U};
        static inline constexpr underlying_type offset_basis{14695981039346656037U};
    };

    template < typename T >
        requires(std::is_same_v<T, std::byte>) || (std::is_integral_v<T> && sizeof(T) == 1)
    constexpr std::size_t fnv1a_hash(const T* mem, std::size_t size) noexcept {
        using traits = fnv1a_hash_traits<>;
        std::size_t hash{traits::offset_basis};
        for ( T byte : std::span(mem, size) ) {
            hash ^= static_cast<std::size_t>(byte);
            hash *= traits::prime;
        }
        return hash;
    }

    inline std::size_t fnv1a_hash(const void* mem, std::size_t size) noexcept {
        return fnv1a_hash(static_cast<const std::byte*>(mem), size);
    }
}

namespace meta_hpp::detail
{
    template < std::size_t SizeBytes = sizeof(std::size_t) >
    struct hash_composer_traits {};

    template <>
    struct hash_composer_traits<sizeof(std::uint32_t)> {
        [[nodiscard]] static constexpr std::size_t combine(std::size_t a, std::size_t b) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (a ^= b + 0x9e3779b9U + (a << 6) + (a >> 2));
        }
    };

    template <>
    struct hash_composer_traits<sizeof(std::uint64_t)> {
        [[nodiscard]] static constexpr std::size_t combine(std::size_t a, std::size_t b) noexcept {
            // NOLINTNEXTLINE(*-magic-numbers)
            return (a ^= b + 0x9e3779b97f4a7c15LLU + (a << 12) + (a >> 4));
        }
    };

    struct hash_composer final : private hash_composer_traits<> {
        std::size_t hash{fnv1a_hash_traits<>::prime};

        constexpr operator std::size_t() const noexcept {
            return hash;
        }

        constexpr hash_composer operator<<(std::string_view value) noexcept {
            hash = combine(hash, fnv1a_hash(value.data(), value.size()));
            return *this;
        }

        template < typename Pointer >
            requires std::is_pointer_v<Pointer>
        constexpr hash_composer operator<<(Pointer value) noexcept {
            // NOLINTNEXTLINE(*-reinterpret-cast)
            hash = combine(hash, reinterpret_cast<std::uintptr_t>(value));
            return *this;
        }

        template < typename Integral >
            requires(std::is_integral_v<Integral> && sizeof(Integral) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(Integral value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value));
            return *this;
        }

        template < typename Enum >
            requires(std::is_enum_v<Enum> && sizeof(std::underlying_type_t<Enum>) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(Enum value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value));
            return *this;
        }

        template < typename Enum >
            requires(std::is_enum_v<Enum> && sizeof(std::underlying_type_t<Enum>) <= sizeof(std::size_t))
        constexpr hash_composer operator<<(bitflags<Enum> value) noexcept {
            hash = combine(hash, static_cast<std::size_t>(value.as_raw()));
            return *this;
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    class inline_vector final {
    public:
        inline_vector() = delete;

        inline_vector(inline_vector&&) = delete;
        inline_vector& operator=(inline_vector&&) = delete;

        inline_vector(const inline_vector&) = delete;
        inline_vector& operator=(const inline_vector&) = delete;

        inline_vector(T* mem, std::size_t capacity)
        : begin_{mem}
        , end_{mem}
        , capacity_{mem + capacity} {}

        ~inline_vector() {
            std::destroy(begin_, end_);
        }

        // clang-format off

        [[nodiscard]] T* data() noexcept { return begin_; }
        [[nodiscard]] const T* data() const noexcept { return begin_; }

        [[nodiscard]] T* begin() noexcept { return begin_; }
        [[nodiscard]] const T* begin() const noexcept { return begin_; }
        [[nodiscard]] const T* cbegin() const noexcept { return begin_; }

        [[nodiscard]] T* end() noexcept { return end_; }
        [[nodiscard]] const T* end() const noexcept { return end_; }
        [[nodiscard]] const T* cend() const noexcept { return end_; }

        // clang-format on

        template < typename... Args >
        T& emplace_back(Args&&... args) {
            META_HPP_ASSERT(end_ < capacity_ && "full vector");
            return *std::construct_at(end_++, std::forward<Args>(args)...);
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return static_cast<std::size_t>(end_ - begin_);
        }

        [[nodiscard]] std::size_t get_capacity() const noexcept {
            return static_cast<std::size_t>(capacity_ - begin_);
        }

    private:
        T* begin_{};
        T* end_{};
        T* capacity_{};
    };
}

namespace meta_hpp::detail
{
    template < typename Value, typename Allocator >
        requires std::is_move_constructible_v<Value> && std::is_move_assignable_v<Value>
    typename std::vector<Value, Allocator>::iterator insert_or_assign( //
        std::vector<Value, Allocator>& vector,
        typename std::vector<Value, Allocator>::value_type&& value
    ) {
        if ( auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end() ) {
            *position = std::move(value);
            return position;
        }
        return vector.insert(vector.end(), std::move(value));
    }

    template < typename Value, typename Allocator >
        requires std::is_copy_constructible_v<Value> && std::is_copy_assignable_v<Value>
    typename std::vector<Value, Allocator>::iterator insert_or_assign( //
        std::vector<Value, Allocator>& vector,
        const typename std::vector<Value, Allocator>::value_type& value
    ) {
        if ( auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end() ) {
            *position = value;
            return position;
        }
        return vector.insert(vector.end(), value);
    }
}

namespace meta_hpp::detail
{
    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>& value
    ) {
        map.swap(value);
        map.merge(value);
    }

    template < typename Key, typename Value, typename Compare, typename Allocator >
    void insert_or_assign( //
        std::map<Key, Value, Compare, Allocator>& map,
        std::map<Key, Value, Compare, Allocator>&& value
    ) {
        map.swap(value);
        map.merge(std::move(value));
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    struct is_in_place_type : std::false_type {};

    template < typename U >
    struct is_in_place_type<std::in_place_type_t<U>> : std::true_type {};

    template < typename T >
    inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;
}

namespace meta_hpp::detail
{
    class memory_buffer final {
    public:
        memory_buffer() = default;

        memory_buffer(const memory_buffer&) = delete;
        memory_buffer& operator=(const memory_buffer&) = delete;

        memory_buffer(memory_buffer&& other) noexcept
        : data_{other.data_}
        , size_{other.size_}
        , align_{other.align_} {
            other.data_ = nullptr;
            other.size_ = 0;
            other.align_ = std::align_val_t{};
        }

        memory_buffer& operator=(memory_buffer&& other) noexcept {
            if ( this != &other ) {
                memory_buffer{std::move(other)}.swap(*this);
            }
            return *this;
        }

        explicit memory_buffer(std::size_t size, std::align_val_t align)
        : data_{::operator new(size, align)}
        , size_{size}
        , align_{align} {}

        explicit memory_buffer(const void* mem, std::size_t size, std::align_val_t align)
        : memory_buffer{size, align} {
            if ( mem != nullptr && size > 0 ) {
                std::memcpy(data_, mem, size);
            }
        }

        ~memory_buffer() noexcept {
            reset();
        }

        [[nodiscard]] bool is_valid() const noexcept {
            return data_ != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return is_valid();
        }

        void reset() noexcept {
            if ( data_ != nullptr ) {
                ::operator delete(data_, align_);
                data_ = nullptr;
                size_ = 0;
                align_ = std::align_val_t{};
            }
        }

        [[nodiscard]] void* get_data() noexcept {
            return data_;
        }

        [[nodiscard]] const void* get_data() const noexcept {
            return data_;
        }

        [[nodiscard]] std::size_t get_size() const noexcept {
            return size_;
        }

        [[nodiscard]] std::align_val_t get_align() const noexcept {
            return align_;
        }

        void swap(memory_buffer& other) noexcept {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
            std::swap(align_, other.align_);
        }

        [[nodiscard]] std::size_t get_hash() const noexcept {
            return fnv1a_hash(data_, size_);
        }

        [[nodiscard]] bool operator==(const memory_buffer& other) const noexcept {
            return (size_ == other.size_) //
                && (size_ == 0 || std::memcmp(data_, other.data_, size_) == 0);
        }

        [[nodiscard]] std::strong_ordering operator<=>(const memory_buffer& other) const noexcept {
            if ( const std::strong_ordering cmp{size_ <=> other.size_}; cmp != std::strong_ordering::equal ) {
                return cmp;
            }
            return (size_ == 0 ? 0 : std::memcmp(data_, other.data_, size_)) <=> 0;
        }

    private:
        void* data_{};
        std::size_t size_{};
        std::align_val_t align_{};
    };

    inline void swap(memory_buffer& l, memory_buffer& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::detail::memory_buffer> {
        size_t operator()(const meta_hpp::detail::memory_buffer& mb) const noexcept {
            return mb.get_hash();
        }
    };
}

namespace meta_hpp::detail
{
    class noncopyable {
    protected:
        noncopyable() = default;
        ~noncopyable() = default;

    public:
        noncopyable(noncopyable&&) = delete;
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(noncopyable&&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
}

namespace meta_hpp::detail
{
    class nonesuch {
    public:
        nonesuch() = delete;
        ~nonesuch() = delete;
        nonesuch(nonesuch&&) = delete;
        nonesuch(const nonesuch&) = delete;
        nonesuch& operator=(nonesuch&&) = delete;
        nonesuch& operator=(const nonesuch&) = delete;
    };
}

namespace meta_hpp::detail
{
    template < typename... Ts >
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template < typename... Ts >
    overloaded(Ts...) -> overloaded<Ts...>;
}

namespace meta_hpp::detail
{
    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_const(R (C::*func)(Args...) const noexcept) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...)) -> decltype(func) {
        return func;
    }

    template < typename C, typename R, typename... Args >
    constexpr auto select_non_const(R (C::*func)(Args...) noexcept) -> decltype(func) {
        return func;
    }

    template < typename Signature >
    constexpr auto select_overload(Signature* func) noexcept -> decltype(func) {
        return func;
    }

    template < typename Signature, typename C >
    constexpr auto select_overload(Signature C::*func) noexcept -> decltype(func) {
        return func;
    }
}

namespace meta_hpp::detail
{
    template < typename Enum >
    [[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}

namespace meta_hpp::detail
{
    template < typename... Types >
    struct type_list {};

    template < std::size_t I >
    using size_constant = std::integral_constant<std::size_t, I>;

    template < std::size_t I >
    using index_constant = std::integral_constant<std::size_t, I>;
}

namespace meta_hpp::detail
{
    template < std::size_t Index, typename TypeList >
    struct type_list_at;

    template < std::size_t Index, typename... Types >
    struct type_list_at<Index, type_list<Types...>> {
        using type = std::tuple_element_t<Index, std::tuple<Types...>>;
    };

    template < std::size_t Index, typename TypeList >
    using type_list_at_t = typename type_list_at<Index, TypeList>::type;
}

namespace meta_hpp::detail
{
    template < typename TypeList >
    struct type_list_arity;

    template < typename... Types >
    struct type_list_arity<type_list<Types...>> : size_constant<sizeof...(Types)> {};

    template < typename TypeList >
    inline constexpr std::size_t type_list_arity_v = type_list_arity<TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_count_of;

    template < template < typename > class Pred, typename... Types >
    struct type_list_count_of<Pred, type_list<Types...>> : size_constant<(0 + ... + Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr std::size_t type_list_count_of_v = type_list_count_of<Pred, TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename Default, typename TypeList >
    struct type_list_first_of;

    template < template < typename > class Pred, typename Default >
    struct type_list_first_of<Pred, Default, type_list<>> {
        using type = Default;
    };

    template < template < typename > class Pred, typename Default, typename Type, typename... Types >
    struct type_list_first_of<Pred, Default, type_list<Type, Types...>> {
        using type = std::conditional_t< //
            Pred<Type>::value,
            Type,
            typename type_list_first_of<Pred, Default, type_list<Types...>>::type>;
    };

    template < template < typename > class Pred, typename Default, typename TypeList >
    using type_list_first_of_t = typename type_list_first_of<Pred, Default, TypeList>::type;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_and;

    template < template < typename > class Pred, typename... Types >
    struct type_list_and<Pred, type_list<Types...>> : std::bool_constant<(... && Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr bool type_list_and_v = type_list_and<Pred, TypeList>::value;
}

namespace meta_hpp::detail
{
    template < template < typename > class Pred, typename TypeList >
    struct type_list_or;

    template < template < typename > class Pred, typename... Types >
    struct type_list_or<Pred, type_list<Types...>> : std::bool_constant<(... || Pred<Types>::value)> {};

    template < template < typename > class Pred, typename TypeList >
    inline constexpr bool type_list_or_v = type_list_or<Pred, TypeList>::value;
}

namespace meta_hpp
{
    using detail::error_code;
    using detail::exception;
    using detail::get_error_code_message;

    using detail::memory_buffer;
    using detail::overloaded;

    using detail::select_const;
    using detail::select_non_const;
    using detail::select_overload;
}

namespace meta_hpp
{
    class uerror;
    class uresult;

    class uvalue;

    namespace detail
    {
        class uarg_base;
        class uarg;

        class uinst_base;
        class uinst;
    }

    template < typename T >
    concept uvalue_family                       //
        = std::is_same_v<T, uerror>             //
       || std::is_same_v<T, uresult>            //
       || std::is_same_v<T, uvalue>             //
       || std::is_same_v<T, detail::uarg_base>  //
       || std::is_same_v<T, detail::uarg>       //
       || std::is_same_v<T, detail::uinst_base> //
       || std::is_same_v<T, detail::uinst>;     //
}

namespace meta_hpp
{
    class argument;
    class constructor;
    class destructor;
    class evalue;
    class function;
    class member;
    class method;
    class scope;
    class variable;

    template < typename T >
    concept state_family                 //
        = std::is_same_v<T, argument>    //
       || std::is_same_v<T, constructor> //
       || std::is_same_v<T, destructor>  //
       || std::is_same_v<T, evalue>      //
       || std::is_same_v<T, function>    //
       || std::is_same_v<T, member>      //
       || std::is_same_v<T, method>      //
       || std::is_same_v<T, scope>       //
       || std::is_same_v<T, variable>;   //
}

namespace meta_hpp
{
    class any_type;
    class array_type;
    class class_type;
    class constructor_type;
    class destructor_type;
    class enum_type;
    class function_type;
    class member_type;
    class method_type;
    class nullptr_type;
    class number_type;
    class pointer_type;
    class reference_type;
    class void_type;

    template < typename T >
    concept type_family                       //
        = std::is_same_v<T, any_type>         //
       || std::is_same_v<T, array_type>       //
       || std::is_same_v<T, class_type>       //
       || std::is_same_v<T, constructor_type> //
       || std::is_same_v<T, destructor_type>  //
       || std::is_same_v<T, enum_type>        //
       || std::is_same_v<T, function_type>    //
       || std::is_same_v<T, member_type>      //
       || std::is_same_v<T, method_type>      //
       || std::is_same_v<T, nullptr_type>     //
       || std::is_same_v<T, number_type>      //
       || std::is_same_v<T, pointer_type>     //
       || std::is_same_v<T, reference_type>   //
       || std::is_same_v<T, void_type>;       //
}

namespace meta_hpp
{
    class argument_index;
    class constructor_index;
    class destructor_index;
    class evalue_index;
    class function_index;
    class member_index;
    class method_index;
    class scope_index;
    class variable_index;

    template < typename T >
    concept index_family                       //
        = std::is_same_v<T, argument_index>    //
       || std::is_same_v<T, constructor_index> //
       || std::is_same_v<T, destructor_index>  //
       || std::is_same_v<T, evalue_index>      //
       || std::is_same_v<T, function_index>    //
       || std::is_same_v<T, member_index>      //
       || std::is_same_v<T, method_index>      //
       || std::is_same_v<T, scope_index>       //
       || std::is_same_v<T, variable_index>;   //
}

namespace meta_hpp
{
    using metadata_map = std::map<std::string, uvalue, std::less<>>;
    using typedef_map = std::map<std::string, any_type, std::less<>>;
    using uvalue_list = std::vector<uvalue>;

    using any_type_list = std::vector<any_type>;
    using class_list = std::vector<class_type>;
    using enum_list = std::vector<enum_type>;

    using argument_list = std::vector<argument>;
    using constructor_list = std::vector<constructor>;
    using destructor_list = std::vector<destructor>;
    using evalue_list = std::vector<evalue>;
    using function_list = std::vector<function>;
    using member_list = std::vector<member>;
    using method_list = std::vector<method>;
    using variable_list = std::vector<variable>;
}

namespace meta_hpp::detail
{
    enum class type_kind : std::uint32_t {
        array_,
        class_,
        constructor_,
        destructor_,
        enum_,
        function_,
        member_,
        method_,
        nullptr_,
        number_,
        pointer_,
        reference_,
        void_,
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    concept array_kind = std::is_array_v<T>;

    template < typename T >
    concept class_kind = std::is_class_v<T>;

    template < typename T >
    concept enum_kind = std::is_enum_v<T>;

    template < typename T >
    concept function_kind = std::is_function_v<T>;

    template < typename T >
    concept function_pointer_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

    template < typename T >
    concept member_pointer_kind = std::is_member_object_pointer_v<T>;

    template < typename T >
    concept method_pointer_kind = std::is_member_function_pointer_v<T>;

    template < typename T >
    concept nullptr_kind = std::is_null_pointer_v<T>;

    template < typename T >
    concept number_kind = std::is_arithmetic_v<T>;

    template < typename T >
    concept pointer_kind = std::is_pointer_v<T>;

    template < typename T >
    concept reference_kind = std::is_reference_v<T>;

    template < typename T >
    concept void_kind = std::is_void_v<T>;

    template < typename T >
    concept any_pointer_family = std::is_pointer_v<T> || std::is_member_pointer_v<T>;

    template < typename T >
    concept not_any_pointer_family = !std::is_pointer_v<T> && !std::is_member_pointer_v<T>;
}

namespace meta_hpp::detail
{
    template < typename T >
    struct type_to_kind;

    template < typename T >
    inline constexpr type_kind type_to_kind_v = type_to_kind<T>::value;
}

namespace meta_hpp::detail
{
    template < array_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::array_> {};

    template < class_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::class_> {};

    template < enum_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::enum_> {};

    template < function_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::function_> {};

    template < member_pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::member_> {};

    template < method_pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::method_> {};

    template < nullptr_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::nullptr_> {};

    template < number_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::number_> {};

    template < pointer_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::pointer_> {};

    template < reference_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::reference_> {};

    template < void_kind T >
    struct type_to_kind<T> : std::integral_constant<type_kind, type_kind::void_> {};
}

namespace meta_hpp::detail
{
    enum class array_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
        is_bounded = 1 << 2,
        is_unbounded = 1 << 3,
    };

    using array_bitflags = bitflags<array_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(array_flags)
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    struct array_traits {
        static constexpr std::size_t extent{std::extent_v<Array>};

        using cv_data_type = std::remove_extent_t<Array>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr array_bitflags make_flags() noexcept {
            array_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(array_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(array_flags::is_volatile);
            }

            if constexpr ( std::is_bounded_array_v<Array> ) {
                flags.set(array_flags::is_bounded);
            }

            if constexpr ( std::is_unbounded_array_v<Array> ) {
                flags.set(array_flags::is_unbounded);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class class_flags : std::uint8_t {
        is_empty = 1 << 0,
        is_final = 1 << 1,
        is_abstract = 1 << 2,
        is_polymorphic = 1 << 3,
        is_template_instantiation = 1 << 4,
    };

    using class_bitflags = bitflags<class_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(class_flags)
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < class_kind Class >
        struct class_argument_traits_impl {
            using argument_types = type_list<>;
            static constexpr std::tuple argument_values = std::make_tuple();
        };

        template < typename T >
        inline constexpr decltype(std::ignore) type_to_ignore_v = std::ignore;

        //
        // typename...
        //

        template < //
            template < typename... >
            typename Class,
            typename... Zs >
        struct class_argument_traits_impl<Class<Zs...>> {
            using argument_types = type_list<Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(type_to_ignore_v<Zs>...);
        };

        //
        // auto, typename...
        //

        template < //
            template < auto, typename... >
            typename Class,
            auto A,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, Zs...>> {
            using argument_types = type_list<decltype(A), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, type_to_ignore_v<Zs>...);
        };

        //
        // auto, auto, typename...
        // typename, auto, typename...
        //

        template < //
            template < auto, auto, typename... >
            typename Class,
            auto A,
            auto B,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, auto, typename... >
            typename Class,
            typename A,
            auto B,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<A, decltype(B), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, type_to_ignore_v<Zs>...);
        };

        //
        // auto, auto, auto, typename...
        // typename, auto, auto, typename...
        // auto, typename, auto, typename...
        // typename, typename, auto, typename...
        //

        template < //
            template < auto, auto, auto, typename... >
            typename Class,
            auto A,
            auto B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, auto, auto, typename... >
            typename Class,
            typename A,
            auto B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, decltype(B), decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < auto, typename, auto, typename... >
            typename Class,
            auto A,
            typename B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), B, decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, C, type_to_ignore_v<Zs>...);
        };

        template < //
            template < typename, typename, auto, typename... >
            typename Class,
            typename A,
            typename B,
            auto C,
            typename... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, B, decltype(C), Zs...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, C, type_to_ignore_v<Zs>...);
        };

        //
        // auto...
        //

        template < //
            template < auto... >
            typename Class,
            auto... Zs >
        struct class_argument_traits_impl<Class<Zs...>> {
            using argument_types = type_list<decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(Zs...);
        };

        //
        // typename, auto...
        //

        template < //
            template < typename, auto... >
            typename Class,
            typename A,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, Zs...>> {
            using argument_types = type_list<A, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, Zs...);
        };

        //
        // auto, typename, auto...
        // typename, typename, auto...
        //

        template < //
            template < auto, typename, auto... >
            typename Class,
            auto A,
            typename B,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<decltype(A), B, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, Zs...);
        };

        template < //
            template < typename, typename, auto... >
            typename Class,
            typename A,
            typename B,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, Zs...>> {
            using argument_types = type_list<A, B, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, Zs...);
        };

        //
        // auto, auto, typename, auto...
        // typename, auto, typename, auto...
        // auto, typename, typename, auto...
        // typename, typename, typename, auto...
        //

        template < //
            template < auto, auto, typename, auto... >
            typename Class,
            auto A,
            auto B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), decltype(B), C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, B, std::ignore, Zs...);
        };

        template < //
            template < typename, auto, typename, auto... >
            typename Class,
            typename A,
            auto B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, decltype(B), C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, B, std::ignore, Zs...);
        };

        template < //
            template < auto, typename, typename, auto... >
            typename Class,
            auto A,
            typename B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<decltype(A), B, C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(A, std::ignore, std::ignore, Zs...);
        };

        template < //
            template < typename, typename, typename, auto... >
            typename Class,
            typename A,
            typename B,
            typename C,
            auto... Zs >
            requires(sizeof...(Zs) > 0)
        struct class_argument_traits_impl<Class<A, B, C, Zs...>> {
            using argument_types = type_list<A, B, C, decltype(Zs)...>;
            static constexpr std::tuple argument_values = std::make_tuple(std::ignore, std::ignore, std::ignore, Zs...);
        };
    }

    template < class_kind Class >
    struct class_traits {
        static constexpr std::size_t size{sizeof(Class)};
        static constexpr std::size_t align{alignof(Class)};

        using argument_types = typename impl::class_argument_traits_impl<Class>::argument_types;
        static constexpr std::tuple argument_values = impl::class_argument_traits_impl<Class>::argument_values;
        static_assert(type_list_arity_v<argument_types> == std::tuple_size_v<decltype(argument_values)>);

        [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
            class_bitflags flags{};

            if constexpr ( std::is_empty_v<Class> ) {
                flags.set(class_flags::is_empty);
            }

            if constexpr ( std::is_final_v<Class> ) {
                flags.set(class_flags::is_final);
            }

            if constexpr ( std::is_abstract_v<Class> ) {
                flags.set(class_flags::is_abstract);
            }

            if constexpr ( std::is_polymorphic_v<Class> ) {
                flags.set(class_flags::is_polymorphic);
            }

            if constexpr ( type_list_arity_v<argument_types> > 0 ) {
                flags.set(class_flags::is_template_instantiation);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class constructor_flags : std::uint8_t {
        is_noexcept = 1 << 0,
    };

    using constructor_bitflags = bitflags<constructor_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(constructor_flags)
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits;

    template < typename Class, typename... Args >
    concept class_constructor_kind //
        = class_kind<Class> && std::is_constructible_v<Class, Args...>;
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    struct constructor_traits {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = Class;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr constructor_bitflags make_flags() noexcept {
            constructor_bitflags flags{};

            if constexpr ( std::is_nothrow_constructible_v<Class, Args...> ) {
                flags.set(constructor_flags::is_noexcept);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class destructor_flags : std::uint8_t {
        is_noexcept = 1 << 0,
        is_virtual = 1 << 1,
    };

    using destructor_bitflags = bitflags<destructor_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(destructor_flags)
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_traits;

    template < typename Class >
    concept class_destructor_kind //
        = class_kind<Class> && std::is_destructible_v<Class>;
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    struct destructor_traits {
        using class_type = Class;

        [[nodiscard]] static constexpr destructor_bitflags make_flags() noexcept {
            destructor_bitflags flags{};

            if constexpr ( std::is_nothrow_destructible_v<Class> ) {
                flags.set(destructor_flags::is_noexcept);
            }

            if constexpr ( std::has_virtual_destructor_v<Class> ) {
                flags.set(destructor_flags::is_virtual);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class enum_flags : std::uint8_t {
        is_scoped = 1 << 0,
    };

    using enum_bitflags = bitflags<enum_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(enum_flags)
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    struct enum_traits {
        using underlying_type = std::underlying_type_t<Enum>;

        [[nodiscard]] static constexpr enum_bitflags make_flags() noexcept {
            enum_bitflags flags{};

            if constexpr ( !std::is_convertible_v<Enum, underlying_type> ) {
                flags.set(enum_flags::is_scoped);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class function_flags : std::uint8_t {
        is_noexcept = 1 << 0,
    };

    using function_bitflags = bitflags<function_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(function_flags)
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    struct function_traits;

    template < typename R, typename... Args >
    struct function_traits<R(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using return_type = std::remove_cv_t<R>;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename... Args >
    struct function_traits<R(Args...) noexcept> : function_traits<R(Args...)> {
        [[nodiscard]] static constexpr function_bitflags make_flags() noexcept {
            return function_flags::is_noexcept;
        }
    };
}

namespace meta_hpp::detail
{
    enum class member_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
    };

    using member_bitflags = bitflags<member_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(member_flags)
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    struct member_traits;

    template < typename Member, typename Class >
    concept class_member_pointer_kind                      //
        = class_kind<Class> && member_pointer_kind<Member> //
       && std::is_same_v<Class, typename member_traits<Member>::class_type>;
}

namespace meta_hpp::detail
{
    template < typename V, typename C >
    struct member_traits<V C::*> {
        using cv_value_type = V;
        inline static constexpr bool is_readonly = std::is_const_v<cv_value_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_value_type>;

        using class_type = C;
        using value_type = std::remove_cv_t<cv_value_type>;

        [[nodiscard]] static constexpr member_bitflags make_flags() noexcept {
            member_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(member_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(member_flags::is_volatile);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class method_flags : std::uint8_t {
        is_const = 1 << 0,
        is_noexcept = 1 << 1,
        is_lvalue_qualified = 1 << 2,
        is_rvalue_qualified = 1 << 3,
    };

    using method_bitflags = bitflags<method_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(method_flags)
}

namespace meta_hpp::detail
{
    template < method_pointer_kind Method >
    struct method_traits;

    template < typename Method, typename Class >
    concept class_method_pointer_kind                      //
        = class_kind<Class> && method_pointer_kind<Method> //
       && std::is_same_v<Class, typename method_traits<Method>::class_type>;
}

namespace meta_hpp::detail
{
    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...)> {
        static constexpr std::size_t arity{sizeof...(Args)};

        using class_type = C;
        using return_type = std::remove_cv_t<R>;
        using qualified_type = C;
        using argument_types = type_list<std::remove_cv_t<Args>...>;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return {};
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...)&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) & noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const & noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) &&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) && noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const&&> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_rvalue_qualified;
        }
    };

    template < typename R, typename C, typename... Args >
    struct method_traits<R (C::*)(Args...) const && noexcept> : method_traits<R (C::*)(Args...)> {
        using qualified_type = const C&&;

        [[nodiscard]] static constexpr method_bitflags make_flags() noexcept {
            return method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified;
        }
    };
}

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    struct nullptr_traits {};
}

namespace meta_hpp::detail
{
    enum class number_flags : std::uint8_t {
        is_signed = 1 << 0,
        is_unsigned = 1 << 1,
        is_integral = 1 << 2,
        is_floating_point = 1 << 3,
    };

    using number_bitflags = bitflags<number_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(number_flags)
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    struct number_traits {
        static constexpr std::size_t size{sizeof(Number)};
        static constexpr std::size_t align{alignof(Number)};

        [[nodiscard]] static constexpr number_bitflags make_flags() noexcept {
            number_bitflags flags{};

            if constexpr ( std::is_signed_v<Number> ) {
                flags.set(number_flags::is_signed);
            }

            if constexpr ( std::is_unsigned_v<Number> ) {
                flags.set(number_flags::is_unsigned);
            }

            if constexpr ( std::is_integral_v<Number> ) {
                flags.set(number_flags::is_integral);
            }

            if constexpr ( std::is_floating_point_v<Number> ) {
                flags.set(number_flags::is_floating_point);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class pointer_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
    };

    using pointer_bitflags = bitflags<pointer_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(pointer_flags)
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    struct pointer_traits {
        using cv_data_type = std::remove_pointer_t<Pointer>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr pointer_bitflags make_flags() noexcept {
            pointer_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(pointer_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(pointer_flags::is_volatile);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    enum class reference_flags : std::uint8_t {
        is_readonly = 1 << 0,
        is_volatile = 1 << 1,
        is_lvalue = 1 << 2,
        is_rvalue = 1 << 3,
    };

    using reference_bitflags = bitflags<reference_flags>;
    META_HPP_DETAIL_BITFLAGS_OPERATORS_DECL(reference_flags)
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    struct reference_traits {
        using cv_data_type = std::remove_reference_t<Reference>;
        inline static constexpr bool is_readonly = std::is_const_v<cv_data_type>;
        inline static constexpr bool is_volatile = std::is_volatile_v<cv_data_type>;

        using data_type = std::remove_cv_t<cv_data_type>;

        [[nodiscard]] static constexpr reference_bitflags make_flags() noexcept {
            reference_bitflags flags{};

            if constexpr ( is_readonly ) {
                flags.set(reference_flags::is_readonly);
            }

            if constexpr ( is_volatile ) {
                flags.set(reference_flags::is_volatile);
            }

            if constexpr ( std::is_lvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_lvalue);
            }

            if constexpr ( std::is_rvalue_reference_v<Reference> ) {
                flags.set(reference_flags::is_rvalue);
            }

            return flags;
        }
    };
}

namespace meta_hpp::detail
{
    template < void_kind Void >
    struct void_traits {};
}

namespace meta_hpp::detail
{
    struct type_data_base;
    struct array_type_data;
    struct class_type_data;
    struct constructor_type_data;
    struct destructor_type_data;
    struct enum_type_data;
    struct function_type_data;
    struct member_type_data;
    struct method_type_data;
    struct nullptr_type_data;
    struct number_type_data;
    struct pointer_type_data;
    struct reference_type_data;
    struct void_type_data;
}

namespace meta_hpp::detail
{
    template < type_family Type >
    struct type_traits;

    template < type_family Type >
    [[nodiscard]] typename type_traits<Type>::data_ptr type_access(const Type& type) {
        return type.data_;
    }
}

namespace meta_hpp::detail
{
    template <>
    struct type_traits<any_type> {
        using data_ptr = type_data_base*;
    };

    template <>
    struct type_traits<array_type> {
        using type = array_type;
        using data_ptr = array_type_data*;
        using data_type = array_type_data;
        inline static constexpr type_kind kind{type_kind::array_};
    };

    template <>
    struct type_traits<class_type> {
        using type = class_type;
        using data_ptr = class_type_data*;
        using data_type = class_type_data;
        inline static constexpr type_kind kind{type_kind::class_};
    };

    template <>
    struct type_traits<constructor_type> {
        using type = constructor_type;
        using data_ptr = constructor_type_data*;
        using data_type = constructor_type_data;
        inline static constexpr type_kind kind{type_kind::constructor_};
    };

    template <>
    struct type_traits<destructor_type> {
        using type = destructor_type;
        using data_ptr = destructor_type_data*;
        using data_type = destructor_type_data;
        inline static constexpr type_kind kind{type_kind::destructor_};
    };

    template <>
    struct type_traits<enum_type> {
        using type = enum_type;
        using data_ptr = enum_type_data*;
        using data_type = enum_type_data;
        inline static constexpr type_kind kind{type_kind::enum_};
    };

    template <>
    struct type_traits<function_type> {
        using type = function_type;
        using data_ptr = function_type_data*;
        using data_type = function_type_data;
        inline static constexpr type_kind kind{type_kind::function_};
    };

    template <>
    struct type_traits<member_type> {
        using type = member_type;
        using data_ptr = member_type_data*;
        using data_type = member_type_data;
        inline static constexpr type_kind kind{type_kind::member_};
    };

    template <>
    struct type_traits<method_type> {
        using type = method_type;
        using data_ptr = method_type_data*;
        using data_type = method_type_data;
        inline static constexpr type_kind kind{type_kind::method_};
    };

    template <>
    struct type_traits<nullptr_type> {
        using type = nullptr_type;
        using data_ptr = nullptr_type_data*;
        using data_type = nullptr_type_data;
        inline static constexpr type_kind kind{type_kind::nullptr_};
    };

    template <>
    struct type_traits<number_type> {
        using type = number_type;
        using data_ptr = number_type_data*;
        using data_type = number_type_data;
        inline static constexpr type_kind kind{type_kind::number_};
    };

    template <>
    struct type_traits<pointer_type> {
        using type = pointer_type;
        using data_ptr = pointer_type_data*;
        using data_type = pointer_type_data;
        inline static constexpr type_kind kind{type_kind::pointer_};
    };

    template <>
    struct type_traits<reference_type> {
        using type = reference_type;
        using data_ptr = reference_type_data*;
        using data_type = reference_type_data;
        inline static constexpr type_kind kind{type_kind::reference_};
    };

    template <>
    struct type_traits<void_type> {
        using type = void_type;
        using data_ptr = void_type_data*;
        using data_type = void_type_data;
        inline static constexpr type_kind kind{type_kind::void_};
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct type_to_traits;

    template < typename T >
    using type_to_traits_t = typename type_to_traits<T>::type;

    template < array_kind T >
    struct type_to_traits<T> {
        using type = array_traits<T>;
    };

    template < class_kind T >
    struct type_to_traits<T> {
        using type = class_traits<T>;
    };

    template < enum_kind T >
    struct type_to_traits<T> {
        using type = enum_traits<T>;
    };

    template < function_kind T >
    struct type_to_traits<T> {
        using type = function_traits<T>;
    };

    template < member_pointer_kind T >
    struct type_to_traits<T> {
        using type = member_traits<T>;
    };

    template < method_pointer_kind T >
    struct type_to_traits<T> {
        using type = method_traits<T>;
    };

    template < nullptr_kind T >
    struct type_to_traits<T> {
        using type = nullptr_traits<T>;
    };

    template < number_kind T >
    struct type_to_traits<T> {
        using type = number_traits<T>;
    };

    template < pointer_kind T >
    struct type_to_traits<T> {
        using type = pointer_traits<T>;
    };

    template < reference_kind T >
    struct type_to_traits<T> {
        using type = reference_traits<T>;
    };

    template < void_kind T >
    struct type_to_traits<T> {
        using type = void_traits<T>;
    };
}

namespace meta_hpp::detail
{
    template < typename Traits >
    struct traits_to_type_traits;

    template < typename Traits >
    using traits_to_type_traits_t = typename traits_to_type_traits<Traits>::type;

    template < array_kind Array >
    struct traits_to_type_traits<array_traits<Array>> {
        using type = type_traits<array_type>;
    };

    template < class_kind Class >
    struct traits_to_type_traits<class_traits<Class>> {
        using type = type_traits<class_type>;
    };

    template < class_kind Class, typename... Args >
    struct traits_to_type_traits<constructor_traits<Class, Args...>> {
        using type = type_traits<constructor_type>;
    };

    template < class_kind Class >
    struct traits_to_type_traits<destructor_traits<Class>> {
        using type = type_traits<destructor_type>;
    };

    template < enum_kind Enum >
    struct traits_to_type_traits<enum_traits<Enum>> {
        using type = type_traits<enum_type>;
    };

    template < function_kind Function >
    struct traits_to_type_traits<function_traits<Function>> {
        using type = type_traits<function_type>;
    };

    template < member_pointer_kind Member >
    struct traits_to_type_traits<member_traits<Member>> {
        using type = type_traits<member_type>;
    };

    template < method_pointer_kind Method >
    struct traits_to_type_traits<method_traits<Method>> {
        using type = type_traits<method_type>;
    };

    template < nullptr_kind Nullptr >
    struct traits_to_type_traits<nullptr_traits<Nullptr>> {
        using type = type_traits<nullptr_type>;
    };

    template < number_kind Number >
    struct traits_to_type_traits<number_traits<Number>> {
        using type = type_traits<number_type>;
    };

    template < pointer_kind Pointer >
    struct traits_to_type_traits<pointer_traits<Pointer>> {
        using type = type_traits<pointer_type>;
    };

    template < reference_kind Reference >
    struct traits_to_type_traits<reference_traits<Reference>> {
        using type = type_traits<reference_type>;
    };

    template < void_kind Void >
    struct traits_to_type_traits<void_traits<Void>> {
        using type = type_traits<void_type>;
    };
}

namespace meta_hpp
{
    using detail::array_bitflags;
    using detail::array_flags;

    using detail::class_bitflags;
    using detail::class_flags;

    using detail::constructor_bitflags;
    using detail::constructor_flags;

    using detail::destructor_bitflags;
    using detail::destructor_flags;

    using detail::enum_bitflags;
    using detail::enum_flags;

    using detail::function_bitflags;
    using detail::function_flags;

    using detail::member_bitflags;
    using detail::member_flags;

    using detail::method_bitflags;
    using detail::method_flags;

    using detail::number_bitflags;
    using detail::number_flags;

    using detail::pointer_bitflags;
    using detail::pointer_flags;

    using detail::reference_bitflags;
    using detail::reference_flags;
}

namespace meta_hpp
{
    using detail::type_kind;

    using detail::array_kind;
    using detail::class_kind;
    using detail::enum_kind;
    using detail::function_kind;
    using detail::function_pointer_kind;
    using detail::member_pointer_kind;
    using detail::method_pointer_kind;
    using detail::nullptr_kind;
    using detail::number_kind;
    using detail::pointer_kind;
    using detail::reference_kind;
    using detail::void_kind;

    using detail::any_pointer_family;
    using detail::not_any_pointer_family;

    using detail::class_constructor_kind;
    using detail::class_destructor_kind;
    using detail::class_member_pointer_kind;
    using detail::class_method_pointer_kind;
}

namespace meta_hpp
{
    class uvalue final {
    public:
        uvalue() = default;
        ~uvalue() noexcept;

        uvalue(uvalue&& other) noexcept;
        uvalue& operator=(uvalue&& other) noexcept;

        uvalue(const uvalue& other) = delete;
        uvalue& operator=(const uvalue& other) = delete;

        template <                                              //
            typename T,                                         //
            typename = std::enable_if_t<                        //
                !uvalue_family<std::decay_t<T>> &&              //
                !detail::is_in_place_type_v<std::decay_t<T>> && //
                std::is_constructible_v<std::decay_t<T>, T>>>   //
        uvalue(T&& val);

        template <                                            //
            typename T,                                       //
            typename = std::enable_if_t<                      //
                !uvalue_family<std::decay_t<T>> &&            //
                std::is_constructible_v<std::decay_t<T>, T>>> //
        uvalue& operator=(T&& val);

        template < typename T, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, Args...>
        explicit uvalue(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
        explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, Args...>
        std::decay_t<T>& emplace(Args&&... args);

        template < typename T, typename U, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
        std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

        [[nodiscard]] bool has_value() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void reset() noexcept;
        void swap(uvalue& other) noexcept;

        [[nodiscard]] any_type get_type() const noexcept;

        [[nodiscard]] void* get_data() noexcept;
        [[nodiscard]] const void* get_data() const noexcept;
        [[nodiscard]] const void* get_cdata() const noexcept;

        [[nodiscard]] uvalue operator*() const;
        [[nodiscard]] bool has_deref_op() const noexcept;

        [[nodiscard]] uvalue operator[](std::size_t index) const;
        [[nodiscard]] bool has_index_op() const noexcept;

        [[nodiscard]] uvalue copy() const;
        [[nodiscard]] bool has_copy_op() const noexcept;

        [[nodiscard]] uvalue unmap() const;
        [[nodiscard]] bool has_unmap_op() const noexcept;

        template < typename T >
        [[nodiscard]] bool is() const noexcept;

        template < any_pointer_family T >
        [[nodiscard]] T as();
        template < any_pointer_family T >
        [[nodiscard]] T as() const;

        template < not_any_pointer_family T >
        [[nodiscard]] T& as() &;
        template < not_any_pointer_family T >
        [[nodiscard]] const T& as() const&;
        template < not_any_pointer_family T >
        [[nodiscard]] T as() &&;
        template < not_any_pointer_family T >
        [[nodiscard]] const T&& as() const&&;

        template < any_pointer_family T >
        [[nodiscard]] T try_as() noexcept;
        template < any_pointer_family T >
        [[nodiscard]] T try_as() const noexcept;

        template < not_any_pointer_family T >
        [[nodiscard]] T* try_as() & noexcept;
        template < not_any_pointer_family T >
        [[nodiscard]] const T* try_as() const& noexcept;
        template < not_any_pointer_family T >
        void try_as() && = delete;
        template < not_any_pointer_family T >
        void try_as() const&& = delete;

    private:
        struct vtable_t;

        struct alignas(std::max_align_t) internal_storage_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            std::byte data[sizeof(void*) * 2];
        };

        struct external_storage_t final {
            // NOLINTNEXTLINE(*-avoid-c-arrays)
            std::byte padding[sizeof(internal_storage_t) - sizeof(void*)];
            void* ptr;
        };

        enum class storage_e : std::uintptr_t {
            nothing,
            trivial,
            internal,
            external,
        };

        // NOLINTNEXTLINE(*-union-access)
        struct storage_u final {
            union {
                internal_storage_t internal;
                external_storage_t external;
            };

            std::uintptr_t vtag;
        } storage_{};

        static_assert(std::is_standard_layout_v<storage_u>);
        static_assert(alignof(storage_u) == alignof(std::max_align_t));
        static_assert(sizeof(internal_storage_t) == sizeof(external_storage_t));
    };

    inline void swap(uvalue& l, uvalue& r) noexcept {
        l.swap(r);
    }

    template < typename T, typename... Args >
    [[nodiscard]] uvalue make_uvalue(Args&&... args) {
        return uvalue(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
    [[nodiscard]] uvalue make_uvalue(std::initializer_list<U> ilist, Args&&... args) {
        return uvalue(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
    }
}

namespace meta_hpp
{
    class type_id final {
    public:
        type_id() = default;
        ~type_id() = default;

        type_id(type_id&&) = default;
        type_id(const type_id&) = default;

        type_id& operator=(type_id&&) = default;
        type_id& operator=(const type_id&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        void swap(type_id& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;

        [[nodiscard]] bool operator==(const type_id& other) const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const type_id& other) const noexcept;

    private:
        template < type_family T >
        friend class type_base;

        explicit type_id(const detail::type_data_base* data);

    private:
        const detail::type_data_base* data_{};
    };
}

namespace std
{
    template <>
    struct hash<meta_hpp::type_id> {
        size_t operator()(const meta_hpp::type_id& id) const noexcept {
            return id.get_hash();
        }
    };
}

namespace meta_hpp
{
    template < type_family Type >
    class type_base {
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        friend data_ptr detail::type_access<Type>(const Type&);

    public:
        using id_type = type_id;

        type_base() = default;

        explicit type_base(data_ptr data);

        type_base(type_base&&) = default;
        type_base(const type_base&) = default;

        type_base& operator=(type_base&&) = default;
        type_base& operator=(const type_base&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;

        [[nodiscard]] id_type get_id() const noexcept;
        [[nodiscard]] type_kind get_kind() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

    protected:
        ~type_base() = default;

        data_ptr data_{};
    };
}

namespace meta_hpp
{
    class any_type final : public type_base<any_type> {
    public:
        using type_base<any_type>::type_base;

        template < type_family Type >
        any_type(const Type& other) noexcept;

        template < type_family Type >
        [[nodiscard]] bool is() const noexcept;
        [[nodiscard]] bool is(type_kind kind) const noexcept;

        template < type_family Type >
        [[nodiscard]] Type as() const noexcept;

        template < typename F >
        bool match(F&& f) const;

        [[nodiscard]] bool is_array() const noexcept;
        [[nodiscard]] bool is_class() const noexcept;
        [[nodiscard]] bool is_constructor() const noexcept;
        [[nodiscard]] bool is_destructor() const noexcept;
        [[nodiscard]] bool is_enum() const noexcept;
        [[nodiscard]] bool is_function() const noexcept;
        [[nodiscard]] bool is_member() const noexcept;
        [[nodiscard]] bool is_method() const noexcept;
        [[nodiscard]] bool is_nullptr() const noexcept;
        [[nodiscard]] bool is_number() const noexcept;
        [[nodiscard]] bool is_pointer() const noexcept;
        [[nodiscard]] bool is_reference() const noexcept;
        [[nodiscard]] bool is_void() const noexcept;

        [[nodiscard]] array_type as_array() const noexcept;
        [[nodiscard]] class_type as_class() const noexcept;
        [[nodiscard]] constructor_type as_constructor() const noexcept;
        [[nodiscard]] destructor_type as_destructor() const noexcept;
        [[nodiscard]] enum_type as_enum() const noexcept;
        [[nodiscard]] function_type as_function() const noexcept;
        [[nodiscard]] member_type as_member() const noexcept;
        [[nodiscard]] method_type as_method() const noexcept;
        [[nodiscard]] nullptr_type as_nullptr() const noexcept;
        [[nodiscard]] number_type as_number() const noexcept;
        [[nodiscard]] pointer_type as_pointer() const noexcept;
        [[nodiscard]] reference_type as_reference() const noexcept;
        [[nodiscard]] void_type as_void() const noexcept;
    };

    class array_type final : public type_base<array_type> {
    public:
        using type_base<array_type>::type_base;
        [[nodiscard]] array_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_extent() const noexcept;
        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class class_type final : public type_base<class_type> {
    public:
        using type_base<class_type>::type_base;
        [[nodiscard]] class_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const uvalue& get_argument_value(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
        [[nodiscard]] const uvalue_list& get_argument_values() const noexcept;

        [[nodiscard]] const class_list& get_base_classes() const noexcept;
        [[nodiscard]] const constructor_list& get_constructors() const noexcept;
        [[nodiscard]] const function_list& get_functions() const noexcept;
        [[nodiscard]] const member_list& get_members() const noexcept;
        [[nodiscard]] const method_list& get_methods() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_list& get_variables() const noexcept;

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;
        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename Arg >
        bool destroy(Arg&& arg) const;
        bool destroy_at(void* mem) const;

        template < class_kind Derived >
        [[nodiscard]] bool is_base_of() const;
        [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

        template < class_kind Derived >
        [[nodiscard]] bool is_direct_base_of() const;
        [[nodiscard]] bool is_direct_base_of(const class_type& derived) const noexcept;

        template < class_kind Base >
        [[nodiscard]] bool is_derived_from() const;
        [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

        template < class_kind Base >
        [[nodiscard]] bool is_direct_derived_from() const;
        [[nodiscard]] bool is_direct_derived_from(const class_type& base) const noexcept;

        [[nodiscard]] function get_function(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] member get_member(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] method get_method(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name, bool recursively = true) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name, bool recursively = true) const noexcept;

        template < typename... Args >
        [[nodiscard]] constructor get_constructor_with() const;
        template < typename Iter >
        [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const;
        [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
        [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

        [[nodiscard]] destructor get_destructor() const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            bool recursively = true
        ) const;

        template < typename Iter >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::span<const any_type> args,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::initializer_list<any_type> args,
            bool recursively = true
        ) const noexcept;

        template < typename... Args >
        [[nodiscard]] method get_method_with( //
            std::string_view name,
            bool recursively = true
        ) const;

        template < typename Iter >
        [[nodiscard]] method get_method_with( //
            std::string_view name,
            Iter first,
            Iter last,
            bool recursively = true
        ) const;

        [[nodiscard]] method get_method_with( //
            std::string_view name,
            std::span<const any_type> args,
            bool recursively = true
        ) const noexcept;

        [[nodiscard]] method get_method_with( //
            std::string_view name,
            std::initializer_list<any_type> args,
            bool recursively = true
        ) const noexcept;
    };

    class constructor_type final : public type_base<constructor_type> {
    public:
        using type_base<constructor_type>::type_base;
        [[nodiscard]] constructor_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class destructor_type final : public type_base<destructor_type> {
    public:
        using type_base<destructor_type>::type_base;
        [[nodiscard]] destructor_bitflags get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
    };

    class enum_type final : public type_base<enum_type> {
    public:
        using type_base<enum_type>::type_base;
        [[nodiscard]] enum_bitflags get_flags() const noexcept;

        [[nodiscard]] number_type get_underlying_type() const noexcept;

        [[nodiscard]] const evalue_list& get_evalues() const noexcept;

        [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

        template < enum_kind Enum >
        [[nodiscard]] std::string_view value_to_name(Enum value) const;
        [[nodiscard]] const uvalue& name_to_value(std::string_view name) const noexcept;
    };

    class function_type final : public type_base<function_type> {
    public:
        using type_base<function_type>::type_base;
        [[nodiscard]] function_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class member_type final : public type_base<member_type> {
    public:
        using type_base<member_type>::type_base;
        [[nodiscard]] member_bitflags get_flags() const noexcept;

        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_value_type() const noexcept;
    };

    class method_type final : public type_base<method_type> {
    public:
        using type_base<method_type>::type_base;
        [[nodiscard]] method_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] class_type get_owner_type() const noexcept;
        [[nodiscard]] any_type get_return_type() const noexcept;
        [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
        [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
    };

    class nullptr_type final : public type_base<nullptr_type> {
    public:
        using type_base<nullptr_type>::type_base;
    };

    class number_type final : public type_base<number_type> {
    public:
        using type_base<number_type>::type_base;
        [[nodiscard]] number_bitflags get_flags() const noexcept;

        [[nodiscard]] std::size_t get_size() const noexcept;
        [[nodiscard]] std::size_t get_align() const noexcept;
    };

    class pointer_type final : public type_base<pointer_type> {
    public:
        using type_base<pointer_type>::type_base;
        [[nodiscard]] pointer_bitflags get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class reference_type final : public type_base<reference_type> {
    public:
        using type_base<reference_type>::type_base;
        [[nodiscard]] reference_bitflags get_flags() const noexcept;

        [[nodiscard]] any_type get_data_type() const noexcept;
    };

    class void_type final : public type_base<void_type> {
    public:
        using type_base<void_type>::type_base;
    };
}

namespace std
{
    template < meta_hpp::type_family Type >
    struct hash<Type> {
        size_t operator()(const Type& type) const noexcept {
            return type.get_hash();
        }
    };
}

namespace meta_hpp
{
    template < type_family TypeL, type_family TypeR >
    [[nodiscard]] bool operator==(const TypeL& l, const TypeR& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_id() == r.get_id());
    }

    template < type_family TypeL, type_family TypeR >
    [[nodiscard]] std::strong_ordering operator<=>(const TypeL& l, const TypeR& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_id() <=> r.get_id() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < type_family Type >
    [[nodiscard]] bool operator==(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() && l.get_id() == r;
    }

    template < type_family Type >
    [[nodiscard]] std::strong_ordering operator<=>(const Type& l, const typename Type::id_type& r) noexcept {
        return l.is_valid() ? l.get_id() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const type_kind kind;
        const std::size_t shared;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        metadata_map metadata;

        explicit type_data_base(type_kind nkind, std::size_t nshared)
        : kind{nkind}
        , shared{nshared} {}

        type_data_base(type_data_base&&) = delete;
        type_data_base(const type_data_base&) = delete;
        type_data_base& operator=(type_data_base&&) = delete;
        type_data_base& operator=(const type_data_base&) = delete;

    protected:
        ~type_data_base() = default;
    };

    struct array_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const array_bitflags flags;
        const std::size_t extent;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < array_kind Array >
        explicit array_type_data(array_traits<Array>);
    };

    struct class_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const class_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        const any_type_list argument_types;
        const uvalue_list argument_values;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        class_list base_classes;
        constructor_list constructors;
        destructor_list destructors;
        function_list functions;
        member_list members;
        method_list methods;
        typedef_map typedefs;
        variable_list variables;

        struct upcast_func_t final {
            using upcast_t = void* (*)(void*);

            type_id target{};
            upcast_t upcast{};

            [[nodiscard]] void* apply(void* ptr) const noexcept;
            [[nodiscard]] const void* apply(const void* ptr) const noexcept;
        };

        using deep_upcasts_t = std::vector<upcast_func_t>;
        deep_upcasts_t deep_upcasts;

        template < class_kind Class >
        explicit class_type_data(class_traits<Class>);
    };

    struct constructor_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const constructor_bitflags flags;
        const class_type owner_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < class_kind Class, typename... Args >
        explicit constructor_type_data(constructor_traits<Class, Args...>);
    };

    struct destructor_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const destructor_bitflags flags;
        const class_type owner_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < class_kind Class >
        explicit destructor_type_data(destructor_traits<Class>);
    };

    struct enum_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const enum_bitflags flags;
        const number_type underlying_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        evalue_list evalues;

        template < enum_kind Enum >
        explicit enum_type_data(enum_traits<Enum>);
    };

    struct function_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const function_bitflags flags;
        const any_type return_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < function_kind Function >
        explicit function_type_data(function_traits<Function>);
    };

    struct member_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const member_bitflags flags;
        const class_type owner_type;
        const any_type value_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < member_pointer_kind Member >
        explicit member_type_data(member_traits<Member>);
    };

    struct method_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const method_bitflags flags;
        const class_type owner_type;
        const any_type return_type;
        const any_type_list argument_types;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < method_pointer_kind Method >
        explicit method_type_data(method_traits<Method>);
    };

    struct nullptr_type_data final : type_data_base {
        template < nullptr_kind Nullptr >
        explicit nullptr_type_data(nullptr_traits<Nullptr>);
    };

    struct number_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const number_bitflags flags;
        const std::size_t size;
        const std::size_t align;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < number_kind Number >
        explicit number_type_data(number_traits<Number>);
    };

    struct pointer_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const pointer_bitflags flags;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < pointer_kind Pointer >
        explicit pointer_type_data(pointer_traits<Pointer>);
    };

    struct reference_type_data final : type_data_base {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const reference_bitflags flags;
        const any_type data_type;
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < reference_kind Reference >
        explicit reference_type_data(reference_traits<Reference>);
    };

    struct void_type_data final : type_data_base {
        template < void_kind Void >
        explicit void_type_data(void_traits<Void>);
    };
}

namespace meta_hpp
{
    inline type_id::type_id(const detail::type_data_base* data)
    : data_{data} {}

    inline bool type_id::is_valid() const noexcept {
        return data_ != nullptr;
    }

    inline type_id::operator bool() const noexcept {
        return is_valid();
    }

    inline void type_id::swap(type_id& other) noexcept {
        std::swap(data_, other.data_);
    }

    inline std::size_t type_id::get_hash() const noexcept {
        return data_ != nullptr ? data_->shared : 0;
    }

    inline bool type_id::operator==(const type_id& other) const noexcept {
        if ( data_ == other.data_ ) {
            return true;
        }

        if ( is_valid() != other.is_valid() ) {
            return false;
        }

        return data_->shared == other.data_->shared;
    }

    inline std::strong_ordering type_id::operator<=>(const type_id& other) const noexcept {
        if ( data_ == other.data_ ) {
            return std::strong_ordering::equal;
        }

        // NOLINTNEXTLINE(*-bool-conversion)
        if ( const std::strong_ordering cmp{is_valid() <=> other.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }

        return data_->shared <=> other.data_->shared;
    }
}

namespace meta_hpp
{
    template < type_family Type >
    type_base<Type>::type_base(data_ptr data)
    : data_{std::move(data)} {}

    template < type_family Type >
    bool type_base<Type>::is_valid() const noexcept {
        return data_ != nullptr;
    }

    template < type_family Type >
    type_base<Type>::operator bool() const noexcept {
        return is_valid();
    }

    template < type_family Type >
    std::size_t type_base<Type>::get_hash() const noexcept {
        return get_id().get_hash();
    }

    template < type_family Type >
    typename type_base<Type>::id_type type_base<Type>::get_id() const noexcept {
        return id_type{data_};
    }

    template < type_family Type >
    type_kind type_base<Type>::get_kind() const noexcept {
        return data_->kind;
    }

    template < type_family Type >
    const metadata_map& type_base<Type>::get_metadata() const noexcept {
        return data_->metadata;
    }
}

namespace meta_hpp
{
    class argument_index final {
    public:
        argument_index() = delete;
        explicit argument_index(any_type type, std::size_t position);

        [[nodiscard]] any_type get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        void swap(argument_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const argument_index&) const = default;

    private:
        any_type type_;
        std::size_t position_{};
    };

    class constructor_index final {
    public:
        constructor_index() = delete;
        explicit constructor_index(constructor_type type);

        [[nodiscard]] constructor_type get_type() const noexcept;

        void swap(constructor_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const constructor_index&) const = default;

    private:
        constructor_type type_;
    };

    class destructor_index final {
    public:
        destructor_index() = delete;
        explicit destructor_index(destructor_type type);

        [[nodiscard]] destructor_type get_type() const noexcept;

        void swap(destructor_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const destructor_index&) const = default;

    private:
        destructor_type type_;
    };

    class evalue_index final {
    public:
        evalue_index() = delete;
        explicit evalue_index(enum_type type, std::string name);

        [[nodiscard]] enum_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(evalue_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const evalue_index&) const = default;

    private:
        enum_type type_;
        std::string name_;
    };

    class function_index final {
    public:
        function_index() = delete;
        explicit function_index(function_type type, std::string name);

        [[nodiscard]] function_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(function_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const function_index&) const = default;

    private:
        function_type type_;
        std::string name_;
    };

    class member_index final {
    public:
        member_index() = delete;
        explicit member_index(member_type type, std::string name);

        [[nodiscard]] member_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(member_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const member_index&) const = default;

    private:
        member_type type_;
        std::string name_;
    };

    class method_index final {
    public:
        method_index() = delete;
        explicit method_index(method_type type, std::string name);

        [[nodiscard]] method_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(method_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const method_index&) const = default;

    private:
        method_type type_;
        std::string name_;
    };

    class scope_index final {
    public:
        scope_index() = delete;
        explicit scope_index(std::string name);

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(scope_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const scope_index&) const = default;

    private:
        std::string name_;
    };

    class variable_index final {
    public:
        variable_index() = delete;
        explicit variable_index(pointer_type type, std::string name);

        [[nodiscard]] pointer_type get_type() const noexcept;

        [[nodiscard]] std::string get_name() && noexcept;
        [[nodiscard]] const std::string& get_name() const& noexcept;

        void swap(variable_index& other) noexcept;
        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const variable_index&) const = default;

    private:
        pointer_type type_;
        std::string name_;
    };

    template < index_family Index >
    void swap(Index& l, Index& r) noexcept {
        l.swap(r);
    }
}

namespace std
{
    template < meta_hpp::index_family Index >
    struct hash<Index> {
        size_t operator()(const Index& index) const noexcept {
            return index.get_hash();
        }
    };
}

namespace meta_hpp
{
    namespace constructor_policy
    {
        inline constexpr struct as_object_t final {
        } as_object{};

        inline constexpr struct as_raw_pointer_t final {
        } as_raw_pointer{};

        inline constexpr struct as_shared_pointer_t final {
        } as_shared_pointer{};

        inline constexpr struct as_unique_pointer_t final {
        } as_unique_pointer{};
    }

    namespace function_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct discard_return_t final {
        } discard_return{};

        inline constexpr struct return_reference_as_pointer_t final {
        } return_reference_as_pointer{};
    }

    namespace member_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct as_pointer_t final {
        } as_pointer{};

        inline constexpr struct as_reference_wrapper_t final {
        } as_reference_wrapper{};
    }

    namespace method_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct discard_return_t final {
        } discard_return{};

        inline constexpr struct return_reference_as_pointer_t final {
        } return_reference_as_pointer{};
    }

    namespace variable_policy
    {
        inline constexpr struct as_copy_t final {
        } as_copy{};

        inline constexpr struct as_pointer_t final {
        } as_pointer{};

        inline constexpr struct as_reference_wrapper_t final {
        } as_reference_wrapper{};
    }
}

namespace meta_hpp::detail
{
    template < typename Policy >
    concept constructor_policy_family                                      //
        = std::is_same_v<Policy, constructor_policy::as_object_t>          //
       || std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>     //
       || std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>  //
       || std::is_same_v<Policy, constructor_policy::as_unique_pointer_t>; //

    template < typename Policy >
    concept function_policy_family                                                //
        = std::is_same_v<Policy, function_policy::as_copy_t>                      //
       || std::is_same_v<Policy, function_policy::discard_return_t>               //
       || std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept member_policy_family                                         //
        = std::is_same_v<Policy, member_policy::as_copy_t>               //
       || std::is_same_v<Policy, member_policy::as_pointer_t>            //
       || std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

    template < typename Policy >
    concept method_policy_family                                                //
        = std::is_same_v<Policy, method_policy::as_copy_t>                      //
       || std::is_same_v<Policy, method_policy::discard_return_t>               //
       || std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>; //

    template < typename Policy >
    concept variable_policy_family                                         //
        = std::is_same_v<Policy, variable_policy::as_copy_t>               //
       || std::is_same_v<Policy, variable_policy::as_pointer_t>            //
       || std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //

    template < typename T >
    using is_constructor_policy = std::bool_constant<constructor_policy_family<T>>;

    template < typename T >
    using is_function_policy = std::bool_constant<function_policy_family<T>>;

    template < typename T >
    using is_member_policy = std::bool_constant<member_policy_family<T>>;

    template < typename T >
    using is_method_policy = std::bool_constant<method_policy_family<T>>;

    template < typename T >
    using is_variable_policy = std::bool_constant<variable_policy_family<T>>;
}

namespace meta_hpp
{
    class uerror final {
    public:
        uerror() = default;
        ~uerror() = default;

        uerror(uerror&&) = default;
        uerror& operator=(uerror&&) = default;

        uerror(const uerror&) = default;
        uerror& operator=(const uerror&) = default;

        explicit uerror(error_code error) noexcept;
        uerror& operator=(error_code error) noexcept;

        [[nodiscard]] bool has_error() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] error_code operator*() const noexcept;
        [[nodiscard]] error_code get_error() const noexcept;

        void reset() noexcept;
        void swap(uerror& other) noexcept;

        [[nodiscard]] std::size_t get_hash() const noexcept;
        [[nodiscard]] std::strong_ordering operator<=>(const uerror& other) const = default;

    private:
        error_code error_{error_code::no_error};
    };

    inline void swap(uerror& l, uerror& r) noexcept {
        l.swap(r);
    }

    [[nodiscard]] inline uerror make_uerror(error_code error) {
        return uerror{error};
    }
}

namespace std
{
    template <>
    struct hash<meta_hpp::uerror> {
        size_t operator()(meta_hpp::uerror ue) const noexcept {
            return ue.get_hash();
        }
    };
}

namespace meta_hpp
{
    class uresult final {
    public:
        uresult() = default;
        ~uresult() = default;

        uresult(uresult&&) = default;
        uresult& operator=(uresult&&) = default;

        uresult(const uresult&) = delete;
        uresult& operator=(const uresult&) = delete;

        explicit(false) uresult(uerror error) noexcept;
        explicit(false) uresult(uvalue value) noexcept;

        uresult& operator=(uerror error) noexcept;
        uresult& operator=(uvalue value) noexcept;

        template <                                              //
            typename T,                                         //
            typename = std::enable_if_t<                        //
                !uvalue_family<std::decay_t<T>> &&              //
                !detail::is_in_place_type_v<std::decay_t<T>> && //
                std::is_constructible_v<std::decay_t<T>, T>>>   //
        uresult(T&& val);

        template <                                            //
            typename T,                                       //
            typename = std::enable_if_t<                      //
                !uvalue_family<std::decay_t<T>> &&            //
                std::is_constructible_v<std::decay_t<T>, T>>> //
        uresult& operator=(T&& val);

        template < typename T, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, Args...>
        explicit uresult(std::in_place_type_t<T>, Args&&... args);

        template < typename T, typename U, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
        explicit uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

        template < typename T, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, Args...>
        std::decay_t<T>& emplace(Args&&... args);

        template < typename T, typename U, typename... Args >
            requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
        std::decay_t<T>& emplace(std::initializer_list<U> ilist, Args&&... args);

        [[nodiscard]] bool has_error() const noexcept;
        [[nodiscard]] bool has_value() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] uvalue* operator->() noexcept;
        [[nodiscard]] const uvalue* operator->() const noexcept;

        [[nodiscard]] uvalue& operator*() & noexcept;
        [[nodiscard]] uvalue&& operator*() && noexcept;
        [[nodiscard]] const uvalue& operator*() const& noexcept;
        [[nodiscard]] const uvalue&& operator*() const&& noexcept;

        [[nodiscard]] uvalue& get_value() & noexcept;
        [[nodiscard]] uvalue&& get_value() && noexcept;
        [[nodiscard]] const uvalue& get_value() const& noexcept;
        [[nodiscard]] const uvalue&& get_value() const&& noexcept;

        [[nodiscard]] error_code get_error() const noexcept;

        void reset() noexcept;
        void swap(uresult& other) noexcept;

    private:
        uvalue value_{};
        error_code error_{error_code::no_error};
    };

    inline void swap(uresult& l, uresult& r) noexcept {
        l.swap(r);
    }

    template < typename T, typename... Args >
    [[nodiscard]] uresult make_uresult(Args&&... args) {
        return uresult(std::in_place_type<T>, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
    [[nodiscard]] uresult make_uresult(std::initializer_list<U> ilist, Args&&... args) {
        return uresult(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
    }
}

namespace meta_hpp::detail
{
    struct argument_state;
    struct constructor_state;
    struct destructor_state;
    struct evalue_state;
    struct function_state;
    struct member_state;
    struct method_state;
    struct scope_state;
    struct variable_state;
}

namespace meta_hpp::detail
{
    template < state_family State >
    struct state_traits;

    template < state_family State >
    [[nodiscard]] typename state_traits<State>::state_ptr state_access(const State& state) {
        return state.state_;
    }
}

namespace meta_hpp::detail
{
    template <>
    struct state_traits<argument> {
        using index_type = argument_index;
        using state_ptr = std::shared_ptr<argument_state>;
    };

    template <>
    struct state_traits<constructor> {
        using index_type = constructor_index;
        using state_ptr = std::shared_ptr<constructor_state>;
    };

    template <>
    struct state_traits<destructor> {
        using index_type = destructor_index;
        using state_ptr = std::shared_ptr<destructor_state>;
    };

    template <>
    struct state_traits<evalue> {
        using index_type = evalue_index;
        using state_ptr = std::shared_ptr<evalue_state>;
    };

    template <>
    struct state_traits<function> {
        using index_type = function_index;
        using state_ptr = std::shared_ptr<function_state>;
    };

    template <>
    struct state_traits<member> {
        using index_type = member_index;
        using state_ptr = std::shared_ptr<member_state>;
    };

    template <>
    struct state_traits<method> {
        using index_type = method_index;
        using state_ptr = std::shared_ptr<method_state>;
    };

    template <>
    struct state_traits<scope> {
        using index_type = scope_index;
        using state_ptr = std::shared_ptr<scope_state>;
    };

    template <>
    struct state_traits<variable> {
        using index_type = variable_index;
        using state_ptr = std::shared_ptr<variable_state>;
    };
}

namespace meta_hpp
{
    template < state_family State >
    class state_base {
        using state_ptr = typename detail::state_traits<State>::state_ptr;
        friend state_ptr detail::state_access<State>(const State&);

    public:
        using index_type = typename detail::state_traits<State>::index_type;

        state_base() = default;

        explicit state_base(state_ptr state);

        state_base(state_base&&) = default;
        state_base(const state_base&) = default;

        state_base& operator=(state_base&&) = default;
        state_base& operator=(const state_base&) = default;

        [[nodiscard]] bool is_valid() const noexcept;
        [[nodiscard]] explicit operator bool() const noexcept;

        [[nodiscard]] const index_type& get_index() const noexcept;
        [[nodiscard]] const metadata_map& get_metadata() const noexcept;

    protected:
        ~state_base() = default;

        state_ptr state_{};
    };
}

namespace meta_hpp
{
    class argument final : public state_base<argument> {
    public:
        using state_base<argument>::state_base;

        [[nodiscard]] any_type get_type() const noexcept;
        [[nodiscard]] std::size_t get_position() const noexcept;

        [[nodiscard]] const std::string& get_name() const noexcept;
    };

    class constructor final : public state_base<constructor> {
    public:
        using state_base<constructor>::state_base;

        [[nodiscard]] constructor_type get_type() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename... Args >
        [[nodiscard]] uvalue create(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uresult try_create(Args&&... args) const;

        template < typename... Args >
        uvalue create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        uresult try_create_at(void* mem, Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error(Args&&... args) const;

        //

        template < typename Iter >
        [[nodiscard]] uvalue create_variadic(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uresult try_create_variadic(Iter first, Iter last) const;

        template < typename Iter >
        uvalue create_variadic_at(void* mem, Iter first, Iter last) const;

        template < typename Iter >
        uresult try_create_variadic_at(void* mem, Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Iter first, Iter last) const;
    };

    class destructor final : public state_base<destructor> {
    public:
        using state_base<destructor>::state_base;

        [[nodiscard]] destructor_type get_type() const noexcept;

        template < typename Arg >
        void destroy(Arg&& arg) const;

        template < typename Arg >
        uresult try_destroy(Arg&& arg) const;

        void destroy_at(void* mem) const;

        uresult try_destroy_at(void* mem) const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename Arg >
        [[nodiscard]] bool is_invocable_with(Arg&& arg) const;

        template < typename Arg >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename Arg >
        [[nodiscard]] uerror check_invocable_error(Arg&& arg) const;
    };

    class evalue final : public state_base<evalue> {
    public:
        using state_base<evalue>::state_base;

        [[nodiscard]] enum_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const uvalue& get_value() const noexcept;
        [[nodiscard]] const uvalue& get_underlying_value() const noexcept;
    };

    class function final : public state_base<function> {
    public:
        using state_base<function>::state_base;

        [[nodiscard]] function_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename... Args >
        uvalue invoke(Args&&... args) const;

        template < typename... Args >
        uresult try_invoke(Args&&... args) const;

        template < typename... Args >
        uvalue operator()(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename... Args >
        [[nodiscard]] bool is_invocable_with(Args&&... args) const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename... Args >
        [[nodiscard]] uerror check_invocable_error(Args&&... args) const;

        //

        template < typename Iter >
        uvalue invoke_variadic(Iter first, Iter last) const;

        template < typename Iter >
        uresult try_invoke_variadic(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Iter first, Iter last) const;

        template < typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Iter first, Iter last) const;
    };

    class member final : public state_base<member> {
    public:
        using state_base<member>::state_base;

        [[nodiscard]] member_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        template < typename Instance >
        [[nodiscard]] uvalue get(Instance&& instance) const;

        template < typename Instance >
        [[nodiscard]] uresult try_get(Instance&& instance) const;

        template < typename Instance >
        [[nodiscard]] uvalue operator()(Instance&& instance) const;

        template < typename Instance, typename Value >
        void set(Instance&& instance, Value&& value) const;

        template < typename Instance, typename Value >
        uresult try_set(Instance&& instance, Value&& value) const;

        template < typename Instance, typename Value >
        void operator()(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with() const;

        template < typename Instance >
        [[nodiscard]] bool is_gettable_with(Instance&& instance) const;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with() const;

        template < typename Instance, typename Value >
        [[nodiscard]] bool is_settable_with(Instance&& instance, Value&& value) const;

        template < typename Instance >
        [[nodiscard]] uerror check_gettable_error() const;

        template < typename Instance >
        [[nodiscard]] uerror check_gettable_error(Instance&& instance) const;

        template < typename Instance, typename Value >
        [[nodiscard]] uerror check_settable_error() const;

        template < typename Instance, typename Value >
        [[nodiscard]] uerror check_settable_error(Instance&& instance, Value&& value) const;
    };

    class method final : public state_base<method> {
    public:
        using state_base<method>::state_base;

        [[nodiscard]] method_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] std::size_t get_arity() const noexcept;
        [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
        [[nodiscard]] const argument_list& get_arguments() const noexcept;

        //

        template < typename Instance, typename... Args >
        uvalue invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uresult try_invoke(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        uvalue operator()(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with() const;

        template < typename Instance, typename... Args >
        [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const;

        template < typename Instance, typename... Args >
        [[nodiscard]] uerror check_invocable_error() const;

        template < typename Instance, typename... Args >
        [[nodiscard]] uerror check_invocable_error(Instance&& instance, Args&&... args) const;

        //

        template < typename Instance, typename Iter >
        uvalue invoke_variadic(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        uresult try_invoke_variadic(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        [[nodiscard]] bool is_variadic_invocable_with(Instance&& instance, Iter first, Iter last) const;

        template < typename Instance, typename Iter >
        [[nodiscard]] uerror check_variadic_invocable_error(Instance&& instance, Iter first, Iter last) const;
    };

    class scope final : public state_base<scope> {
    public:
        using state_base<scope>::state_base;

        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] const function_list& get_functions() const noexcept;
        [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
        [[nodiscard]] const variable_list& get_variables() const noexcept;

        [[nodiscard]] function get_function(std::string_view name) const noexcept;
        [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
        [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

        template < typename... Args >
        [[nodiscard]] function get_function_with( //
            std::string_view name
        ) const;

        template < typename Iter >
        [[nodiscard]] function get_function_with( //
            std::string_view name,
            Iter first,
            Iter last
        ) const;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::span<const any_type> args
        ) const noexcept;

        [[nodiscard]] function get_function_with( //
            std::string_view name,
            std::initializer_list<any_type> args
        ) const noexcept;
    };

    class variable final : public state_base<variable> {
    public:
        using state_base<variable>::state_base;

        [[nodiscard]] pointer_type get_type() const noexcept;
        [[nodiscard]] const std::string& get_name() const noexcept;

        [[nodiscard]] uvalue get() const;
        [[nodiscard]] uresult try_get() const;

        [[nodiscard]] uvalue operator()() const;

        template < typename Value >
        void set(Value&& value) const;

        template < typename Value >
        uresult try_set(Value&& value) const;

        template < typename Value >
        void operator()(Value&& value) const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with() const;

        template < typename Value >
        [[nodiscard]] bool is_settable_with(Value&& value) const;

        template < typename Value >
        [[nodiscard]] uerror check_settable_error() const;

        template < typename Value >
        [[nodiscard]] uerror check_settable_error(Value&& value) const;
    };
}

namespace std
{
    template < meta_hpp::state_family State >
    struct hash<State> {
        size_t operator()(const State& state) const noexcept {
            return state.is_valid() ? state.get_index().get_hash() : 0;
        }
    };
}

namespace meta_hpp
{
    template < state_family State >
    [[nodiscard]] bool operator==(const State& l, const State& r) noexcept {
        return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_index() == r.get_index());
    }

    template < state_family State >
    [[nodiscard]] std::strong_ordering operator<=>(const State& l, const State& r) noexcept {
        if ( const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal ) {
            return cmp;
        }
        return l.is_valid() ? l.get_index() <=> r.get_index() : std::strong_ordering::equal;
    }
}

namespace meta_hpp
{
    template < state_family State >
    [[nodiscard]] bool operator==(const State& l, const typename State::index_type& r) noexcept {
        return l.is_valid() && l.get_index() == r;
    }

    template < state_family State >
    [[nodiscard]] std::strong_ordering operator<=>(const State& l, const typename State::index_type& r) noexcept {
        return l.is_valid() ? l.get_index() <=> r : std::strong_ordering::less;
    }
}

namespace meta_hpp::detail
{
    struct argument_state final : private state_traits<argument> {
        argument_index index;
        metadata_map metadata;

        std::string name{};

        template < typename Argument >
        [[nodiscard]] static state_ptr make(std::size_t position, metadata_map metadata);
        explicit argument_state(argument_index index, metadata_map metadata);
    };

    struct constructor_state final : private state_traits<constructor> {
        using create_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using create_at_impl = fixed_function<uvalue(void*, std::span<const uarg>)>;
        using create_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

        constructor_index index;
        metadata_map metadata;

        create_impl create{};
        create_at_impl create_at{};
        create_error_impl create_error{};
        argument_list arguments{};

        template < constructor_policy_family Policy, class_kind Class, typename... Args >
        [[nodiscard]] static state_ptr make(metadata_map metadata);
        explicit constructor_state(constructor_index index, metadata_map metadata);
    };

    struct destructor_state final : private state_traits<destructor> {
        using destroy_impl = fixed_function<void(const uarg&)>;
        using destroy_at_impl = fixed_function<void(void*)>;
        using destroy_error_impl = fixed_function<uerror(const uarg_base&)>;

        destructor_index index;
        metadata_map metadata;

        destroy_impl destroy{};
        destroy_at_impl destroy_at{};
        destroy_error_impl destroy_error{};

        template < class_kind Class >
        [[nodiscard]] static state_ptr make(metadata_map metadata);
        explicit destructor_state(destructor_index index, metadata_map metadata);
    };

    struct evalue_state final : private state_traits<evalue> {
        evalue_index index;
        metadata_map metadata;

        uvalue enum_value{};
        uvalue underlying_value{};

        template < enum_kind Enum >
        [[nodiscard]] static state_ptr make(std::string name, Enum evalue, metadata_map metadata);
        explicit evalue_state(evalue_index index, metadata_map metadata);
    };

    struct function_state final : private state_traits<function> {
        using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
        using invoke_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

        function_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        invoke_error_impl invoke_error{};
        argument_list arguments{};

        template < function_policy_family Policy, function_pointer_kind Function >
        [[nodiscard]] static state_ptr make(std::string name, Function function_ptr, metadata_map metadata);
        explicit function_state(function_index index, metadata_map metadata);
    };

    struct member_state final : private state_traits<member> {
        using getter_impl = fixed_function<uvalue(const uinst&)>;
        using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

        using getter_error_impl = fixed_function<uerror(const uinst_base&)>;
        using setter_error_impl = fixed_function<uerror(const uinst_base&, const uarg_base&)>;

        member_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        getter_error_impl getter_error{};
        setter_error_impl setter_error{};

        template < member_policy_family Policy, member_pointer_kind Member >
        [[nodiscard]] static state_ptr make(std::string name, Member member_ptr, metadata_map metadata);
        explicit member_state(member_index index, metadata_map metadata);
    };

    struct method_state final : private state_traits<method> {
        using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
        using invoke_error_impl = fixed_function<uerror(const uinst_base&, std::span<const uarg_base>)>;

        method_index index;
        metadata_map metadata;

        invoke_impl invoke{};
        invoke_error_impl invoke_error{};
        argument_list arguments{};

        template < method_policy_family Policy, method_pointer_kind Method >
        [[nodiscard]] static state_ptr make(std::string name, Method method_ptr, metadata_map metadata);
        explicit method_state(method_index index, metadata_map metadata);
    };

    struct scope_state final : private state_traits<scope> {
        scope_index index;
        metadata_map metadata;

        function_list functions{};
        typedef_map typedefs{};
        variable_list variables{};

        [[nodiscard]] static state_ptr make(std::string name, metadata_map metadata);
        explicit scope_state(scope_index index, metadata_map metadata);
    };

    struct variable_state final : private state_traits<variable> {
        using getter_impl = fixed_function<uvalue()>;
        using setter_impl = fixed_function<void(const uarg&)>;
        using setter_error_impl = fixed_function<uerror(const uarg_base&)>;

        variable_index index;
        metadata_map metadata;

        getter_impl getter{};
        setter_impl setter{};
        setter_error_impl setter_error{};

        template < variable_policy_family Policy, pointer_kind Pointer >
        [[nodiscard]] static state_ptr make(std::string name, Pointer variable_ptr, metadata_map metadata);
        explicit variable_state(variable_index index, metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < state_family State >
    state_base<State>::state_base(state_ptr state)
    : state_{std::move(state)} {}

    template < state_family State >
    bool state_base<State>::is_valid() const noexcept {
        return state_ != nullptr;
    }

    template < state_family State >
    state_base<State>::operator bool() const noexcept {
        return is_valid();
    }

    template < state_family State >
    const typename state_base<State>::index_type& state_base<State>::get_index() const noexcept {
        return state_->index;
    }

    template < state_family State >
    const metadata_map& state_base<State>::get_metadata() const noexcept {
        return state_->metadata;
    }
}

namespace meta_hpp::detail
{
    class type_registry final {
    public:
        class locker final {
        public:
            explicit locker()
            : lock_{instance().mutex_} {}

            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;

        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static type_registry& instance() {
            static type_registry instance;
            return instance;
        }

    public:
        template < typename F >
        // NOLINTNEXTLINE(*-missing-std-forward)
        void for_each_type(F&& f) const {
            const locker lock;

            for ( auto&& type : types_ ) {
                std::invoke(f, type);
            }
        }

    public:
        template < typename T >
        [[nodiscard]] auto resolve_by_type() {
            return resolve_by_traits<type_to_traits_t<std::remove_cv_t<T>>>();
        }

        template < typename Traits >
        [[nodiscard]] auto resolve_by_traits() {
            using type_traits = traits_to_type_traits_t<Traits>;

            static auto type_data_instance = [this]() {
                auto new_data{std::make_unique<typename type_traits::data_type>(Traits{})};

                const locker lock;
                types_.emplace(new_data.get());

                return new_data;
            }();

            return typename type_traits::type{type_data_instance.get()};
        }

    private:
        type_registry() = default;

    private:
        std::recursive_mutex mutex_;
        std::set<any_type, std::less<>> types_;
    };
}

namespace meta_hpp::detail
{
    struct poly_info final {
        const void* ptr{};
        class_type type{};
    };

    template < typename T >
    concept check_base_info_enabled //
        = requires { typename T::meta_base_info; };

    template < check_base_info_enabled T >
    using get_meta_base_info = typename T::meta_base_info;

    template < typename T >
    concept poly_info_enabled //
        = requires(type_registry& r, const T& v) { v.meta_poly_info(r); };

    template < poly_info_enabled T >
    poly_info get_meta_poly_info(type_registry& r, const T& v) {
        return v.meta_poly_info(r);
    }
}

#define META_HPP_ENABLE_BASE_INFO(...) \
public: \
    using meta_base_info = ::meta_hpp::detail::type_list<__VA_ARGS__>; \
\
private:

#define META_HPP_ENABLE_POLY_INFO(...) \
    META_HPP_ENABLE_BASE_INFO(__VA_ARGS__) \
public: \
    META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_PUSH() \
    virtual ::meta_hpp::uvalue meta_poly_ptr() { \
        return ::meta_hpp::uvalue{this}; \
    } \
    virtual ::meta_hpp::uvalue meta_poly_ptr() const { \
        return ::meta_hpp::uvalue{this}; \
    } \
    virtual ::meta_hpp::detail::poly_info meta_poly_info(::meta_hpp::detail::type_registry& registry) const { \
        using self_type = std::remove_cvref_t<decltype(*this)>; \
        return ::meta_hpp::detail::poly_info{.ptr = this, .type = registry.resolve_by_type<self_type>()}; \
    } \
    META_HPP_DETAIL_IGNORE_OVERRIDE_WARNINGS_POP() \
private:

namespace meta_hpp::detail
{
    class state_registry final {
    public:
        class locker final {
        public:
            explicit locker()
            : lock_{instance().mutex_} {}

            ~locker() = default;

            locker(locker&&) = default;
            locker& operator=(locker&&) = default;

            locker(const locker&) = delete;
            locker& operator=(const locker&) = delete;

        private:
            std::unique_lock<std::recursive_mutex> lock_;
        };

        [[nodiscard]] static state_registry& instance() {
            static state_registry instance;
            return instance;
        }

    public:
        template < typename F >
        // NOLINTNEXTLINE(*-missing-std-forward)
        void for_each_scope(F&& f) const {
            const locker lock;

            for ( auto&& [_, scope] : scopes_ ) {
                std::invoke(f, scope);
            }
        }

        [[nodiscard]] scope get_scope_by_name(std::string_view name) const noexcept {
            const locker lock;

            if ( auto iter{scopes_.find(name)}; iter != scopes_.end() ) {
                return iter->second;
            }

            return scope{};
        }

        [[nodiscard]] scope resolve_scope(std::string_view name) {
            const locker lock;

            if ( auto iter{scopes_.find(name)}; iter != scopes_.end() ) {
                return iter->second;
            }

            auto&& [iter, _] = scopes_.emplace( //
                std::string{name},
                scope_state::make(std::string{name}, metadata_map{})
            );

            return iter->second;
        }

    private:
        state_registry() = default;

    private:
        std::recursive_mutex mutex_;
        std::map<std::string, scope, std::less<>> scopes_;
    };
}

namespace meta_hpp
{
    template < type_family Type = any_type, typename F >
    // NOLINTNEXTLINE(*-missing-std-forward)
    void for_each_type(F&& f) {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        registry.for_each_type([&f](const any_type& type) {
            if ( type.is<Type>() ) {
                std::invoke(f, type.as<Type>());
            }
        });
    }

    template < typename T >
    auto resolve_type() {
        using namespace detail;
        type_registry& registry = type_registry::instance();
        return registry.resolve_by_type<std::remove_cv_t<T>>();
    }

    template < typename T >
    // NOLINTNEXTLINE(*-missing-std-forward)
    auto resolve_type(T&& from) {
        using namespace detail;

        using raw_type = std::remove_cvref_t<T>;
        type_registry& registry = type_registry::instance();

        if constexpr ( std::is_class_v<raw_type> && std::is_polymorphic_v<raw_type> ) {
            static_assert(
                detail::poly_info_enabled<T>,
                "The class doesn't support polymorphic type resolving. "
                "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
            );
            return detail::get_meta_poly_info(registry, from).type;
        } else {
            (void)from;
            return registry.resolve_by_type<raw_type>();
        }
    }
}

namespace meta_hpp
{
    template < typename F >
    void for_each_scope(F&& f) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        registry.for_each_scope(std::forward<F>(f));
    }

    inline scope resolve_scope(std::string_view name) {
        using namespace detail;
        state_registry& registry = state_registry::instance();
        return registry.resolve_scope(name);
    }
}

namespace meta_hpp
{
    template < type_family Type >
    class type_bind_base {
    public:
        explicit type_bind_base(const Type& type, metadata_map metadata)
        : data_{detail::type_access(type)} {
            detail::insert_or_assign(data_->metadata, std::move(metadata));
        }

        operator Type() const noexcept {
            return Type{data_};
        }

    protected:
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        using data_ref = decltype(*std::declval<data_ptr>());

        [[nodiscard]] data_ref get_data() noexcept {
            return *data_;
        }

    private:
        data_ptr data_;
        detail::type_registry::locker locker_;
    };

    template < state_family State >
    class state_bind_base {
    public:
        explicit state_bind_base(const State& state, metadata_map metadata)
        : state_{detail::state_access(state)} {
            detail::insert_or_assign(state_->metadata, std::move(metadata));
        }

        operator State() const noexcept {
            return State{state_};
        }

    protected:
        using state_ptr = typename detail::state_traits<State>::state_ptr;
        using state_ref = decltype(*std::declval<state_ptr>());

        [[nodiscard]] state_ref get_state() noexcept {
            return *state_;
        }

    private:
        state_ptr state_;
        detail::state_registry::locker locker_;
    };
}

namespace meta_hpp
{
    template < array_kind Array >
    class array_bind final : public type_bind_base<array_type> {
    public:
        explicit array_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < class_kind Class >
    class class_bind final : public type_bind_base<class_type> {
    public:
        explicit class_bind(metadata_map metadata);

        template < typename... Args, typename... Opts >
            requires class_constructor_kind<Class, Args...>
        class_bind& constructor_(Opts&&... opts);

        template < typename... Opts >
            requires class_destructor_kind<Class>
        class_bind& destructor_(Opts&&... opts);

        template < function_pointer_kind Function, typename... Opts >
        class_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

        template < class_member_pointer_kind<Class> Member, typename... Opts >
        class_bind& member_(std::string name, Member member_ptr, Opts&&... opts);

        template < class_method_pointer_kind<Class> Method, typename... Opts >
        class_bind& method_(std::string name, Method method_ptr, Opts&&... opts);

        template < typename Type >
        class_bind& typedef_(std::string name);

        template < pointer_kind Pointer, typename... Opts >
        class_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < enum_kind Enum >
    class enum_bind final : public type_bind_base<enum_type> {
    public:
        explicit enum_bind(metadata_map metadata);

        template < typename... Opts >
        enum_bind& evalue_(std::string name, Enum value, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < function_kind Function >
    class function_bind final : public type_bind_base<function_type> {
    public:
        explicit function_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < member_pointer_kind Member >
    class member_bind final : public type_bind_base<member_type> {
    public:
        explicit member_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < method_pointer_kind Method >
    class method_bind final : public type_bind_base<method_type> {
    public:
        explicit method_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < nullptr_kind Nullptr >
    class nullptr_bind final : public type_bind_base<nullptr_type> {
    public:
        explicit nullptr_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < number_kind Number >
    class number_bind final : public type_bind_base<number_type> {
    public:
        explicit number_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < pointer_kind Pointer >
    class pointer_bind final : public type_bind_base<pointer_type> {
    public:
        explicit pointer_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < reference_kind Reference >
    class reference_bind final : public type_bind_base<reference_type> {
    public:
        explicit reference_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < void_kind Void >
    class void_bind final : public type_bind_base<void_type> {
    public:
        explicit void_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    class scope_bind final : public state_bind_base<scope> {
    public:
        explicit scope_bind(const scope& scope, metadata_map metadata);

        template < function_pointer_kind Function, typename... Opts >
        scope_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

        template < typename Type >
        scope_bind& typedef_(std::string name);

        template < pointer_kind Pointer, typename... Opts >
        scope_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < array_kind Array >
    array_bind<Array> array_(metadata_map metadata = {}) {
        return array_bind<Array>{std::move(metadata)};
    }

    template < class_kind Class >
    class_bind<Class> class_(metadata_map metadata = {}) {
        return class_bind<Class>{std::move(metadata)};
    }

    template < enum_kind Enum >
    enum_bind<Enum> enum_(metadata_map metadata = {}) {
        return enum_bind<Enum>{std::move(metadata)};
    }

    template < function_kind Function >
    function_bind<Function> function_(metadata_map metadata = {}) {
        return function_bind<Function>{std::move(metadata)};
    }

    template < member_pointer_kind Member >
    member_bind<Member> member_(metadata_map metadata = {}) {
        return member_bind<Member>{std::move(metadata)};
    }

    template < method_pointer_kind Method >
    method_bind<Method> method_(metadata_map metadata = {}) {
        return method_bind<Method>{std::move(metadata)};
    }

    template < nullptr_kind Nullptr >
    nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {}) {
        return nullptr_bind<Nullptr>{std::move(metadata)};
    }

    template < number_kind Number >
    number_bind<Number> number_(metadata_map metadata = {}) {
        return number_bind<Number>{std::move(metadata)};
    }

    template < pointer_kind Pointer >
    pointer_bind<Pointer> pointer_(metadata_map metadata = {}) {
        return pointer_bind<Pointer>{std::move(metadata)};
    }

    template < reference_kind Reference >
    reference_bind<Reference> reference_(metadata_map metadata = {}) {
        return reference_bind<Reference>{std::move(metadata)};
    }

    template < void_kind Void >
    void_bind<Void> void_(metadata_map metadata = {}) {
        return void_bind<Void>{std::move(metadata)};
    }
}

namespace meta_hpp
{
    inline scope_bind local_scope_(std::string name, metadata_map metadata = {}) {
        const scope local_scope{detail::scope_state::make(std::move(name), std::move(metadata))};
        return scope_bind{local_scope, {}};
    }

    inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
        const scope static_scope{resolve_scope(name)};
        return scope_bind{static_scope, std::move(metadata)};
    }

    inline scope_bind extend_scope_(const scope& scope, metadata_map metadata = {}) {
        return scope_bind{scope, std::move(metadata)};
    }
}

namespace meta_hpp
{
    class arguments_bind final {
    public:
        class argument_info final {
        public:
            argument_info() = default;
            ~argument_info() = default;

            argument_info(argument_info&&) = default;
            argument_info& operator=(argument_info&&) = default;

            argument_info(const argument_info&) = delete;
            argument_info& operator=(const argument_info&) = delete;

            argument_info(std::string name);
            argument_info(std::string name, metadata_map metadata);

            [[nodiscard]] std::string& get_name() noexcept;
            [[nodiscard]] const std::string& get_name() const noexcept;

            [[nodiscard]] metadata_map& get_metadata() noexcept;
            [[nodiscard]] const metadata_map& get_metadata() const noexcept;

        private:
            std::string name_;
            metadata_map metadata_;
        };

    public:
        using values_t = std::vector<argument_info>;

        operator values_t() &&;

        template < typename... Opts >
        static values_t from_opts(Opts&&... opts);

    public:
        arguments_bind() = default;
        ~arguments_bind() = default;

        arguments_bind(arguments_bind&&) = default;
        arguments_bind(const arguments_bind&) = delete;

        arguments_bind& operator=(arguments_bind&&) = default;
        arguments_bind& operator=(const arguments_bind&) = delete;

        arguments_bind& operator()(std::string name) &;
        arguments_bind operator()(std::string name) &&;

        arguments_bind& operator()(std::string name, metadata_map metadata) &;
        arguments_bind operator()(std::string name, metadata_map metadata) &&;

        arguments_bind& operator()(arguments_bind bind) &;
        arguments_bind operator()(arguments_bind bind) &&;

    private:
        values_t values_;
    };

    inline arguments_bind arguments_() {
        return arguments_bind{};
    }

    inline arguments_bind::argument_info::argument_info(std::string name)
    : name_{std::move(name)} {}

    inline arguments_bind::argument_info::argument_info(std::string name, metadata_map metadata)
    : name_{std::move(name)}
    , metadata_{std::move(metadata)} {}

    inline std::string& arguments_bind::argument_info::get_name() noexcept {
        return name_;
    }

    inline const std::string& arguments_bind::argument_info::get_name() const noexcept {
        return name_;
    }

    inline metadata_map& arguments_bind::argument_info::get_metadata() noexcept {
        return metadata_;
    }

    inline const metadata_map& arguments_bind::argument_info::get_metadata() const noexcept {
        return metadata_;
    }

    inline arguments_bind::operator values_t() && {
        return std::move(values_);
    }

    template < typename... Opts >
    inline arguments_bind::values_t arguments_bind::from_opts(Opts&&... opts) {
        arguments_bind bind;

        const auto process_opt = detail::overloaded{
            [&bind](arguments_bind b) { bind(std::move(b)); }, //
            [](auto&&) {},                                     // ignore other opts
        };

        (process_opt(META_HPP_FWD(opts)), ...);
        return std::move(bind.values_);
    }

    inline arguments_bind& arguments_bind::operator()(std::string name) & {
        values_.emplace_back(std::move(name));
        return *this;
    }

    inline arguments_bind arguments_bind::operator()(std::string name) && {
        (*this)(std::move(name));
        return std::move(*this);
    }

    inline arguments_bind& arguments_bind::operator()(std::string name, metadata_map metadata) & {
        values_.emplace_back(std::move(name), std::move(metadata));
        return *this;
    }

    inline arguments_bind arguments_bind::operator()(std::string name, metadata_map metadata) && {
        (*this)(std::move(name), std::move(metadata));
        return std::move(*this);
    }

    inline arguments_bind& arguments_bind::operator()(arguments_bind bind) & {
        values_.insert( //
            values_.end(),
            std::make_move_iterator(bind.values_.begin()),
            std::make_move_iterator(bind.values_.end())
        );
        return *this;
    }

    inline arguments_bind arguments_bind::operator()(arguments_bind bind) && {
        (*this)(std::move(bind));
        return std::move(*this);
    }
}

namespace meta_hpp
{
    class metadata_bind final {
    public:
        using values_t = metadata_map;

        operator values_t() &&;

        template < typename... Opts >
        static values_t from_opts(Opts&&... opts);

    public:
        metadata_bind() = default;
        ~metadata_bind() = default;

        metadata_bind(metadata_bind&&) = default;
        metadata_bind& operator=(metadata_bind&&) = default;

        metadata_bind(const metadata_bind&) = delete;
        metadata_bind& operator=(const metadata_bind&) = delete;

        metadata_bind& operator()(std::string name, uvalue value) &;
        metadata_bind operator()(std::string name, uvalue value) &&;

        metadata_bind& operator()(metadata_bind bind) &;
        metadata_bind operator()(metadata_bind bind) &&;

    private:
        values_t values_;
    };

    inline metadata_bind metadata_() {
        return metadata_bind{};
    }

    inline metadata_bind::operator values_t() && {
        return std::move(values_);
    }

    template < typename... Opts >
    metadata_bind::values_t metadata_bind::from_opts(Opts&&... opts) {
        metadata_bind bind;

        const auto process_opt = detail::overloaded{
            [&bind](metadata_bind b) { bind(std::move(b)); }, //
            [](auto&&) {},                                    // ignore other opts
        };

        (process_opt(META_HPP_FWD(opts)), ...);
        return std::move(bind.values_);
    }

    inline metadata_bind& metadata_bind::operator()(std::string name, uvalue value) & {
        values_.insert_or_assign(std::move(name), std::move(value));
        return *this;
    }

    inline metadata_bind metadata_bind::operator()(std::string name, uvalue value) && {
        (*this)(std::move(name), std::move(value));
        return std::move(*this);
    }

    inline metadata_bind& metadata_bind::operator()(metadata_bind bind) & {
        detail::insert_or_assign(values_, std::move(bind.values_));
        return *this;
    }

    inline metadata_bind metadata_bind::operator()(metadata_bind bind) && {
        (*this)(std::move(bind));
        return std::move(*this);
    }
}

namespace meta_hpp
{
    template < array_kind Array >
    array_bind<Array>::array_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Array>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < class_kind Class >
    class_bind<Class>::class_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Class>(), std::move(metadata)} {
        if constexpr ( std::is_destructible_v<Class> ) {
            destructor_();
        }
    }

    template < class_kind Class >
    template < typename... Args, typename... Opts >
        requires class_constructor_kind<Class, Args...>
    class_bind<Class>& class_bind<Class>::constructor_(Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_constructor_policy, constructor_policy::as_object_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_constructor_policy, opts_t> <= 1,
            "constructor policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = constructor_state::make<policy_t, Class, Args...>(std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match constructor argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().constructors, constructor{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < typename... Opts >
        requires class_destructor_kind<Class>
    class_bind<Class>& class_bind<Class>::destructor_(Opts&&... opts) {
        using namespace detail;
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = destructor_state::make<Class>(std::move(metadata));
        insert_or_assign(get_data().destructors, destructor{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < function_pointer_kind Function, typename... Opts >
    class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_function_policy, function_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_function_policy, opts_t> <= 1,
            "function policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = function_state::make<policy_t>(std::move(name), function_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match function argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().functions, function{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < class_member_pointer_kind<Class> Member, typename... Opts >
    class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_member_policy, member_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_member_policy, opts_t> <= 1,
            "member policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = member_state::make<policy_t>(std::move(name), member_ptr, std::move(metadata));
        insert_or_assign(get_data().members, member{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < class_method_pointer_kind<Class> Method, typename... Opts >
    class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_method_policy, method_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_method_policy, opts_t> <= 1,
            "method policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = method_state::make<policy_t>(std::move(name), method_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided argument names don't match method argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_data().methods, method{std::move(state)});
        return *this;
    }

    template < class_kind Class >
    template < typename Type >
    class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
        get_data().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    template < class_kind Class >
    template < pointer_kind Pointer, typename... Opts >
    class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_variable_policy, variable_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_variable_policy, opts_t> <= 1,
            "variable policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = variable_state::make<policy_t>(std::move(name), variable_ptr, std::move(metadata));
        insert_or_assign(get_data().variables, variable{std::move(state)});
        return *this;
    }
}

namespace meta_hpp
{
    template < enum_kind Enum >
    enum_bind<Enum>::enum_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Enum>(), std::move(metadata)} {}

    template < enum_kind Enum >
    template < typename... Opts >
    enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value, Opts&&... opts) {
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = detail::evalue_state::make(std::move(name), std::move(value), std::move(metadata));
        detail::insert_or_assign(get_data().evalues, evalue{std::move(state)});
        return *this;
    }
}

namespace meta_hpp
{
    template < function_kind Function >
    function_bind<Function>::function_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Function>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < member_pointer_kind Member >
    member_bind<Member>::member_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Member>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < method_pointer_kind Method >
    method_bind<Method>::method_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Method>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < nullptr_kind Nullptr >
    nullptr_bind<Nullptr>::nullptr_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Nullptr>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < number_kind Number >
    number_bind<Number>::number_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Number>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < pointer_kind Pointer >
    pointer_bind<Pointer>::pointer_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Pointer>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    template < reference_kind Reference >
    reference_bind<Reference>::reference_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Reference>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    inline scope_bind::scope_bind(const scope& scope, metadata_map metadata)
    : state_bind_base{scope, std::move(metadata)} {}

    template < function_pointer_kind Function, typename... Opts >
    scope_bind& scope_bind::function_(std::string name, Function function_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_function_policy, function_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_function_policy, opts_t> <= 1,
            "function policy may be specified only once"
        );

        // there is no 'use after move' here because `from opts` takes only relevant opts
        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        arguments_bind::values_t arguments = arguments_bind::from_opts(META_HPP_FWD(opts)...);

        auto state = function_state::make<policy_t>(std::move(name), function_ptr, std::move(metadata));

        META_HPP_ASSERT(                                //
            arguments.size() <= state->arguments.size() //
            && "provided arguments don't match function argument count"
        );

        using std::min; // prevents windows.h min/max issues
        for ( std::size_t i{}, e{min(arguments.size(), state->arguments.size())}; i < e; ++i ) {
            argument& arg = state->arguments[i];
            state_access(arg)->name = std::move(arguments[i].get_name());
            state_access(arg)->metadata = std::move(arguments[i].get_metadata());
        }

        insert_or_assign(get_state().functions, function{std::move(state)});
        return *this;
    }

    template < typename Type >
    scope_bind& scope_bind::typedef_(std::string name) {
        get_state().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
        return *this;
    }

    template < pointer_kind Pointer, typename... Opts >
    scope_bind& scope_bind::variable_(std::string name, Pointer variable_ptr, Opts&&... opts) {
        using namespace detail;

        using opts_t = type_list<std::remove_cvref_t<Opts>...>;
        using policy_t = type_list_first_of_t<is_variable_policy, variable_policy::as_copy_t, opts_t>;

        static_assert( //
            type_list_count_of_v<is_variable_policy, opts_t> <= 1,
            "variable policy may be specified only once"
        );

        metadata_bind::values_t metadata = metadata_bind::from_opts(META_HPP_FWD(opts)...);
        auto state = variable_state::make<policy_t>(std::move(name), variable_ptr, std::move(metadata));
        insert_or_assign(get_state().variables, variable{std::move(state)});
        return *this;
    }
}

namespace meta_hpp
{
    template < void_kind Void >
    void_bind<Void>::void_bind(metadata_map metadata)
    : type_bind_base{resolve_type<Void>(), std::move(metadata)} {}
}

namespace meta_hpp
{
    inline argument_index::argument_index(any_type type, std::size_t position)
    : type_{type}
    , position_{position} {}

    inline any_type argument_index::get_type() const noexcept {
        return type_;
    }

    inline std::size_t argument_index::get_position() const noexcept {
        return position_;
    }

    inline void argument_index::swap(argument_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(position_, other.position_);
    }

    inline std::size_t argument_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << position_;
    }
}

namespace meta_hpp
{
    inline constructor_index::constructor_index(constructor_type type)
    : type_{type} {}

    inline constructor_type constructor_index::get_type() const noexcept {
        return type_;
    }

    inline void constructor_index::swap(constructor_index& other) noexcept {
        std::swap(type_, other.type_);
    }

    inline std::size_t constructor_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash();
    }
}

namespace meta_hpp
{
    inline destructor_index::destructor_index(destructor_type type)
    : type_{type} {}

    inline destructor_type destructor_index::get_type() const noexcept {
        return type_;
    }

    inline void destructor_index::swap(destructor_index& other) noexcept {
        std::swap(type_, other.type_);
    }

    inline std::size_t destructor_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash();
    }
}

namespace meta_hpp
{
    inline evalue_index::evalue_index(enum_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline enum_type evalue_index::get_type() const noexcept {
        return type_;
    }

    inline std::string evalue_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& evalue_index::get_name() const& noexcept {
        return name_;
    }

    inline void evalue_index::swap(evalue_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t evalue_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}

namespace meta_hpp
{
    inline function_index::function_index(function_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline function_type function_index::get_type() const noexcept {
        return type_;
    }

    inline std::string function_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& function_index::get_name() const& noexcept {
        return name_;
    }

    inline void function_index::swap(function_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t function_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}

namespace meta_hpp
{
    inline member_index::member_index(member_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline member_type member_index::get_type() const noexcept {
        return type_;
    }

    inline std::string member_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& member_index::get_name() const& noexcept {
        return name_;
    }

    inline void member_index::swap(member_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t member_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}

namespace meta_hpp
{
    inline method_index::method_index(method_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline method_type method_index::get_type() const noexcept {
        return type_;
    }

    inline std::string method_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& method_index::get_name() const& noexcept {
        return name_;
    }

    inline void method_index::swap(method_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t method_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}

namespace meta_hpp
{
    inline scope_index::scope_index(std::string name)
    : name_{std::move(name)} {}

    inline std::string scope_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& scope_index::get_name() const& noexcept {
        return name_;
    }

    inline void scope_index::swap(scope_index& other) noexcept {
        std::swap(name_, other.name_);
    }

    inline std::size_t scope_index::get_hash() const noexcept {
        return detail::hash_composer{} << name_;
    }
}

namespace meta_hpp
{
    inline variable_index::variable_index(pointer_type type, std::string name)
    : type_{type}
    , name_{std::move(name)} {}

    inline pointer_type variable_index::get_type() const noexcept {
        return type_;
    }

    inline std::string variable_index::get_name() && noexcept {
        return std::move(name_);
    }

    inline const std::string& variable_index::get_name() const& noexcept {
        return name_;
    }

    inline void variable_index::swap(variable_index& other) noexcept {
        std::swap(type_, other.type_);
        std::swap(name_, other.name_);
    }

    inline std::size_t variable_index::get_hash() const noexcept {
        return detail::hash_composer{} << type_.get_hash() << name_;
    }
}

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args);

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args);

    template < function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args);

    template < function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args);

    //

    template < typename Iter >
    uvalue invoke_variadic(const function& function, Iter first, Iter last);

    template < typename Iter >
    uresult try_invoke_variadic(const function& function, Iter first, Iter last);

    template < function_pointer_kind Function, typename Iter >
    uvalue invoke_variadic(Function function_ptr, Iter first, Iter last);

    template < function_pointer_kind Function, typename Iter >
    uresult try_invoke_variadic(Function function_ptr, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance);

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance);

    template < member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance);

    template < member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance);
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args);

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args);

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args);

    //

    template < typename Instance, typename Iter >
    uvalue invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter >
    uresult try_invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last);

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uvalue invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uresult try_invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept;

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept;

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr) noexcept;

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr, Args&&... args) noexcept;

    template < typename... Args >
    uerror check_invocable_error(const function& function) noexcept;

    template < typename... Args >
    uerror check_invocable_error(const function& function, Args&&... args) noexcept;

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function function_ptr) noexcept;

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function function_ptr, Args&&... args) noexcept;

    //

    template < typename Iter >
    bool is_variadic_invocable_with(const function& function, Iter first, Iter last);

    template < typename Iter, function_pointer_kind Function >
    bool is_variadic_invocable_with(Function function_ptr, Iter first, Iter last);

    template < typename Iter >
    uerror check_variadic_invocable_error(const function& function, Iter first, Iter last);

    template < typename Iter, function_pointer_kind Function >
    uerror check_variadic_invocable_error(Function function_ptr, Iter first, Iter last);
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept;

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept;

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr) noexcept;

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept;

    template < typename Instance >
    uerror check_invocable_error(const member& member) noexcept;

    template < typename Instance >
    uerror check_invocable_error(const member& member, Instance&& instance) noexcept;

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member member_ptr) noexcept;

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member member_ptr, Instance&& instance) noexcept;
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept;

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method) noexcept;

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method method_ptr) noexcept;

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method method_ptr, Instance&& instance, Args&&... args) noexcept;

    //

    template < typename Instance, typename Iter >
    bool is_variadic_invocable_with(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter, method_pointer_kind Method >
    bool is_variadic_invocable_with(Method method_ptr, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter >
    uerror check_variadic_invocable_error(const method& method, Instance&& instance, Iter first, Iter last);

    template < typename Instance, typename Iter, method_pointer_kind Method >
    uerror check_variadic_invocable_error(Method method_ptr, Instance&& instance, Iter first, Iter last);
}

namespace meta_hpp::detail
{
    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_lvalue_ref_kind //
        = (!uvalue_family<Tp>)  //
        &&(std::is_lvalue_reference_v<T>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept arg_rvalue_ref_kind //
        = (!uvalue_family<Tp>)  //
        &&(!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}

namespace meta_hpp::detail
{
    template < typename T >
    concept inst_class_ref_kind //
        = (std::is_class_v<T>)  //
       || (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_lvalue_ref_kind    //
        = (!uvalue_family<Tp>)            //
        &&(std::is_lvalue_reference_v<T>) //
        &&(std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

    template < typename T, typename Tp = std::decay_t<T> >
    concept inst_class_rvalue_ref_kind                               //
        = (!uvalue_family<Tp>)                                       //
        &&(!std::is_reference_v<T> || std::is_rvalue_reference_v<T>) //
        &&(std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < inst_class_ref_kind Q, bool is_const, bool is_lvalue, bool is_rvalue >
        struct inst_traits_impl;

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)();
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() &;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, false, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() &&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, true, false> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const&;
        };

        template < inst_class_ref_kind Q >
        struct inst_traits_impl<Q, true, false, true> {
            using class_type = std::remove_cvref_t<Q>;
            using method_type = void (class_type::*)() const&&;
        };
    }

    template < inst_class_ref_kind Q >
    struct inst_traits final
    : impl::inst_traits_impl< //
          Q,
          cvref_traits<Q>::is_const,
          cvref_traits<Q>::is_lvalue,
          cvref_traits<Q>::is_rvalue> {};
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline bool is_a(const any_type& base, const any_type& derived) noexcept {
        if ( base == derived ) {
            return true;
        }

        const class_type& base_class = base.as_class();
        const class_type& derived_class = derived.as_class();

        if ( base_class && derived_class ) {
            const class_type_data& derived_data = *type_access(derived_class);
            const class_type_data::deep_upcasts_t& deep_upcasts = derived_data.deep_upcasts;

            for ( const auto& upcast : deep_upcasts ) {
                if ( upcast.target == base_class ) {
                    return upcast.upcast != nullptr;
                }
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline void* pointer_upcast(void* ptr, const class_type& from, const class_type& to) {
        if ( nullptr == ptr || !from || !to ) {
            return nullptr;
        }

        if ( from == to ) {
            return ptr;
        }

        const class_type_data& from_data = *type_access(from);
        const class_type_data::deep_upcasts_t& deep_upcasts = from_data.deep_upcasts;

        for ( const auto& upcast : deep_upcasts ) {
            if ( upcast.target == to ) {
                return upcast.upcast != nullptr ? upcast.apply(ptr) : nullptr;
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const class_type& from, const class_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }
}

namespace meta_hpp::detail
{
    [[nodiscard]] inline void* pointer_upcast(void* ptr, const any_type& from, const any_type& to) {
        if ( nullptr == ptr || !from || !to ) {
            return nullptr;
        }

        if ( to.is_void() || from == to ) {
            return ptr;
        }

        const class_type& to_class = to.as_class();
        const class_type& from_class = from.as_class();

        if ( to_class && from_class ) {
            if ( void* base_ptr = pointer_upcast(ptr, from_class, to_class) ) {
                return base_ptr;
            }
        }

        return nullptr;
    }

    [[nodiscard]] inline const void* pointer_upcast(const void* ptr, const any_type& from, const any_type& to) {
        // NOLINTNEXTLINE(*-const-cast)
        return pointer_upcast(const_cast<void*>(ptr), from, to);
    }
}

namespace meta_hpp::detail
{
    template < typename To, typename From >
    [[nodiscard]] To* pointer_upcast(type_registry& registry, From* ptr) {
        return static_cast<To*>(pointer_upcast( //
            ptr,
            registry.resolve_by_type<From>(),
            registry.resolve_by_type<To>()
        ));
    }

    template < typename To, typename From >
    [[nodiscard]] const To* pointer_upcast(type_registry& registry, const From* ptr) {
        return static_cast<const To*>(pointer_upcast( //
            ptr,
            registry.resolve_by_type<From>(),
            registry.resolve_by_type<To>()
        ));
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    concept uarg_cast_to_object = !pointer_kind<T> || function_pointer_kind<T>;

    template < typename T >
    concept uarg_cast_to_pointer = pointer_kind<T> && !function_pointer_kind<T>;
}

namespace meta_hpp::detail
{
    class uarg_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };

    public:
        uarg_base() = default;
        ~uarg_base() = default;

        uarg_base(uarg_base&&) = default;
        uarg_base& operator=(uarg_base&&) = default;

        uarg_base(const uarg_base&) = delete;
        uarg_base& operator=(const uarg_base&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        // NOLINTNEXTLINE(*-missing-std-forward)
        explicit uarg_base(type_registry& registry, T&&)
        : uarg_base{registry, type_list<T&&>{}} {}

        template < arg_lvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        template < arg_rvalue_ref_kind T >
        explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        explicit uarg_base(type_registry&, uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        // NOLINTNEXTLINE(*-param-not-moved)
        explicit uarg_base(type_registry&, uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uarg_base(type_registry&, const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uarg_base(type_registry& registry, T&& v)
        : uarg_base{registry, *std::forward<T>(v)} {}

        [[nodiscard]] bool is_ref_const() const noexcept {
            return ref_type_ == ref_types::const_lvalue //
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] any_type get_raw_type() const noexcept {
            return raw_type_;
        }

        template < uarg_cast_to_pointer To >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

        template < uarg_cast_to_object To >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uarg final : public uarg_base {
    public:
        uarg() = default;
        ~uarg() = default;

        uarg(uarg&&) = default;
        uarg& operator=(uarg&&) = default;

        uarg(const uarg&) = delete;
        uarg& operator=(const uarg&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uvalue>
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v.get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v->get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        explicit uarg(type_registry& registry, T&& v)
        : uarg_base{registry, std::forward<T>(v)}
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uarg_base' doesn't actually move 'v', just gets its type
        }

        template < uarg_cast_to_pointer To >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

        template < uarg_cast_to_object To >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < uarg_cast_to_pointer To >
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type = std::remove_cv_t<To>;

        const any_type& from_type = get_raw_type();
        const pointer_type& to_type_ptr = registry.resolve_by_type<to_raw_type>();

        if ( from_type.is_nullptr() ) {
            return true;
        }

        if ( from_type.is_array() ) {
            const array_type& from_type_array = from_type.as_array();

            const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const bool from_type_array_readonly = is_ref_const();

            const any_type& to_data_type = to_type_ptr.get_data_type();
            const any_type& from_data_type = from_type_array.get_data_type();

            if ( to_type_ptr_readonly >= from_type_array_readonly ) {
                if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                    return true;
                }
            }
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();

            const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

            const any_type& to_data_type = to_type_ptr.get_data_type();
            const any_type& from_data_type = from_type_ptr.get_data_type();

            if ( to_type_ptr_readonly >= from_type_ptr_readonly ) {
                if ( to_data_type.is_void() || is_a(to_data_type, from_data_type) ) {
                    return true;
                }
            }
        }

        return false;
    }

    template < uarg_cast_to_object To >
    [[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<to_raw_type>();

        const auto is_convertible_to_ref = [this]<typename ToRef>(type_list<ToRef>) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                return std::is_convertible_v<noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_lvalue:
                return std::is_convertible_v<const noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::rvalue:
                return std::is_convertible_v<noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_rvalue:
                return std::is_convertible_v<const noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
            }
            return false;
        };

        const auto is_constructible_from_type = [this, &is_convertible_to_ref]<typename FromType>(type_list<FromType>) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                return std::is_constructible_v<To, FromType&> && is_convertible_to_ref(type_list<FromType&>{});
            case ref_types::const_lvalue:
                return std::is_constructible_v<To, const FromType&> && is_convertible_to_ref(type_list<const FromType&>{});
            case ref_types::rvalue:
                return std::is_constructible_v<To, FromType&&> && is_convertible_to_ref(type_list<FromType&&>{});
            case ref_types::const_rvalue:
                return std::is_constructible_v<To, const FromType&&> && is_convertible_to_ref(type_list<const FromType&&>{});
            }
            return false;
        };

        if constexpr ( std::is_reference_v<To> ) {
            if ( is_a(to_type, from_type) && is_convertible_to_ref(type_list<To>{}) ) {
                return true;
            }
        }

        if constexpr ( !std::is_reference_v<To> ) {
            if ( is_a(to_type, from_type) && is_constructible_from_type(type_list<to_raw_type>{}) ) {
                return true;
            }
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < uarg_cast_to_pointer To >
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type = std::remove_cv_t<To>;

        const any_type& from_type = get_raw_type();
        const pointer_type& to_type_ptr = registry.resolve_by_type<to_raw_type>();

        if ( from_type.is_nullptr() ) {
            return static_cast<To>(nullptr);
        }

        if ( from_type.is_array() ) {
            const array_type& from_type_array = from_type.as_array();

            void* to_ptr = pointer_upcast( //
                data_,
                from_type_array.get_data_type(),
                to_type_ptr.get_data_type()
            );
            META_HPP_ASSERT(to_ptr);

            return static_cast<To>(to_ptr);
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();

            void* to_ptr = pointer_upcast( //
                *static_cast<void**>(data_),
                from_type_ptr.get_data_type(),
                to_type_ptr.get_data_type()
            );
            META_HPP_ASSERT(to_ptr);

            return static_cast<To>(to_ptr);
        }

        throw_exception(error_code::bad_argument_cast);
    }

    template < uarg_cast_to_object To >
    [[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

        using to_raw_type_cv = std::remove_reference_t<To>;
        using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

        static_assert( //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>),
            "references to pointers are not supported yet"
        );

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<to_raw_type>();

        void* to_ptr = pointer_upcast(data_, from_type, to_type);
        META_HPP_ASSERT(to_ptr);

        if constexpr ( std::is_lvalue_reference_v<To> ) {
            return *static_cast<to_raw_type_cv*>(to_ptr);
        }

        if constexpr ( std::is_rvalue_reference_v<To> ) {
            return std::move(*static_cast<to_raw_type_cv*>(to_ptr));
        }

        if constexpr ( !std::is_reference_v<To> ) {
            switch ( get_ref_type() ) {
            case ref_types::lvalue:
                if constexpr ( std::is_constructible_v<To, to_raw_type&> ) {
                    return To{*static_cast<to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::const_lvalue:
                if constexpr ( std::is_constructible_v<To, const to_raw_type&> ) {
                    return To{*static_cast<const to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::rvalue:
                if constexpr ( std::is_constructible_v<To, to_raw_type&&> ) {
                    return To{std::move(*static_cast<to_raw_type*>(to_ptr))};
                }
                break;
            case ref_types::const_rvalue:
                if constexpr ( std::is_constructible_v<To, const to_raw_type&&> ) {
                    return To{std::move(*static_cast<const to_raw_type*>(to_ptr))};
                }
                break;
            }
            throw_exception(error_code::bad_argument_cast);
        }
    }
}

namespace meta_hpp::detail
{
    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg> args) noexcept {
        if ( args.size() != type_list_arity_v<ArgTypeList> ) {
            return false;
        }
        return [args, &registry]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }(std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList >
    bool can_cast_all_uargs(type_registry& registry, std::span<const uarg_base> args) noexcept {
        if ( args.size() != type_list_arity_v<ArgTypeList> ) {
            return false;
        }
        return [args, &registry]<std::size_t... Is>(std::index_sequence<Is...>) {
            return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry));
        }(std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }

    template < typename ArgTypeList, typename F >
    auto unchecked_call_with_uargs(type_registry& registry, std::span<const uarg> args, F&& f) {
        META_HPP_DEV_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
        return [args, &registry]<std::size_t... Is>(auto&& captured_f, std::index_sequence<Is...>) {
            return std::invoke(META_HPP_FWD(captured_f), args[Is].cast<type_list_at_t<Is, ArgTypeList>>(registry)...);
        }(META_HPP_FWD(f), std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
    }
}

namespace meta_hpp::detail
{
    namespace impl
    {
        template < typename Type >
        struct shared_type;

        template < typename Traits >
        struct shared_traits;

        template < typename Type >
        concept shared_type_kind = requires {
            { shared_type<Type>{} };
        };

        template < typename Traits >
        concept shared_traits_kind = requires {
            { shared_traits<Traits>{} };
        };

        template < typename Type >
        struct is_shared_type : std::bool_constant<shared_type_kind<Type>> {};

        template < typename Traits >
        struct is_shared_traits : std::bool_constant<shared_traits_kind<Traits>> {};
    }

    template < typename Traits >
    struct shared_traits_hash {
        [[nodiscard]] std::size_t operator()(const void* type_data) const noexcept {
            return hash_composer{} << type_data;
        }
    };

    template < impl::shared_traits_kind Traits >
    struct shared_traits_hash<Traits> {
        [[nodiscard]] std::size_t operator()(const void*) const noexcept {
            return impl::shared_traits<Traits>{}();
        }
    };
}

namespace meta_hpp::detail::impl
{
    template < array_kind Array >
        requires shared_type_kind<typename array_traits<Array>::data_type>
    struct shared_type<Array> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Array>>{}();
        }
    };

    template < function_kind Function >
        requires shared_type_kind<typename function_traits<Function>::return_type>
              && type_list_and_v<is_shared_type, typename function_traits<Function>::argument_types>
    struct shared_type<Function> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Function>>{}();
        }
    };

    template < member_pointer_kind Member >
        requires shared_type_kind<typename member_traits<Member>::class_type>
              && shared_type_kind<typename member_traits<Member>::value_type>
    struct shared_type<Member> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Member>>{}();
        }
    };

    template < method_pointer_kind Method >
        requires shared_type_kind<typename method_traits<Method>::class_type>
              && shared_type_kind<typename method_traits<Method>::return_type>
              && type_list_and_v<is_shared_type, typename method_traits<Method>::argument_types>
    struct shared_type<Method> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Method>>{}();
        }
    };

    template < pointer_kind Pointer >
        requires shared_type_kind<typename pointer_traits<Pointer>::data_type>
    struct shared_type<Pointer> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Pointer>>{}();
        }
    };

    template < reference_kind Reference >
        requires shared_type_kind<typename reference_traits<Reference>::data_type>
    struct shared_type<Reference> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            return shared_traits<type_to_traits_t<Reference>>{}();
        }
    };
}

namespace meta_hpp::detail::impl
{
    template < shared_type_kind Array >
    struct shared_traits<array_traits<Array>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::array_;

            using traits = array_traits<Array>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            hash << traits::extent;

            return hash.hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_traits<class_traits<Class>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::class_;

            using traits = class_traits<Class>;
            hash << traits::make_flags();

            hash << shared_type<Class>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Class, shared_type_kind... Args >
    struct shared_traits<constructor_traits<Class, Args...>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::constructor_;

            using traits = constructor_traits<Class, Args...>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Class >
    struct shared_traits<destructor_traits<Class>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::destructor_;

            using traits = destructor_traits<Class>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Enum >
    struct shared_traits<enum_traits<Enum>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::enum_;

            using traits = enum_traits<Enum>;
            hash << traits::make_flags();

            hash << shared_type<Enum>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Function >
    struct shared_traits<function_traits<Function>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::function_;

            using traits = function_traits<Function>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::return_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Member >
    struct shared_traits<member_traits<Member>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::member_;

            using traits = member_traits<Member>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();
            hash << shared_type<typename traits::value_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Method >
    struct shared_traits<method_traits<Method>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::method_;

            using traits = method_traits<Method>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::class_type>{}();
            hash << shared_type<typename traits::return_type>{}();

            [&hash]<typename... ArgTypes>(type_list<ArgTypes...>) {
                ((hash << shared_type<ArgTypes>{}()), ...);
            }(typename traits::argument_types{});

            return hash.hash;
        }
    };

    template < shared_type_kind Nullptr >
    struct shared_traits<nullptr_traits<Nullptr>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::nullptr_;

            hash << shared_type<Nullptr>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Number >
    struct shared_traits<number_traits<Number>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::number_;

            using traits = number_traits<Number>;
            hash << traits::make_flags();

            hash << shared_type<Number>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Pointer >
    struct shared_traits<pointer_traits<Pointer>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::pointer_;

            using traits = pointer_traits<Pointer>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Reference >
    struct shared_traits<reference_traits<Reference>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::reference_;

            using traits = reference_traits<Reference>;
            hash << traits::make_flags();

            hash << shared_type<typename traits::data_type>{}();

            return hash.hash;
        }
    };

    template < shared_type_kind Void >
    struct shared_traits<void_traits<Void>> {
        [[nodiscard]] constexpr std::size_t operator()() const noexcept {
            hash_composer hash{};
            hash << type_kind::void_;

            hash << shared_type<Void>{}();

            return hash.hash;
        }
    };
}

#define META_HPP_DEFINE_SHARED_TYPE(Type, Name) \
    namespace meta_hpp::detail::impl \
    { \
        template <> \
        struct shared_type<Type> { \
            [[nodiscard]] constexpr std::size_t operator()() const noexcept { \
                return hash_composer{} << std::string_view{Name}; \
            } \
        }; \
    }

META_HPP_DEFINE_SHARED_TYPE(void, "void")
META_HPP_DEFINE_SHARED_TYPE(bool, "bool")
META_HPP_DEFINE_SHARED_TYPE(wchar_t, "wchar_t")
META_HPP_DEFINE_SHARED_TYPE(decltype(nullptr), "nullptr_t")

META_HPP_DEFINE_SHARED_TYPE(char8_t, "char8_t")
META_HPP_DEFINE_SHARED_TYPE(char16_t, "char16_t")
META_HPP_DEFINE_SHARED_TYPE(char32_t, "char32_t")

META_HPP_DEFINE_SHARED_TYPE(float, "float")
META_HPP_DEFINE_SHARED_TYPE(double, "double")
META_HPP_DEFINE_SHARED_TYPE(long double, "long double")

META_HPP_DEFINE_SHARED_TYPE(signed char, "schar")
META_HPP_DEFINE_SHARED_TYPE(unsigned char, "uchar")
META_HPP_DEFINE_SHARED_TYPE(signed short, "sshort")
META_HPP_DEFINE_SHARED_TYPE(unsigned short, "ushort")
META_HPP_DEFINE_SHARED_TYPE(signed int, "sint")
META_HPP_DEFINE_SHARED_TYPE(unsigned int, "uint")
META_HPP_DEFINE_SHARED_TYPE(signed long, "slong")
META_HPP_DEFINE_SHARED_TYPE(unsigned long, "ulong")
META_HPP_DEFINE_SHARED_TYPE(signed long long, "sllong")
META_HPP_DEFINE_SHARED_TYPE(unsigned long long, "ullong")

namespace meta_hpp::detail::function_type_data_impl
{
    template < function_kind Function >
    any_type_list make_argument_types() {
        using ft = function_traits<Function>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ft_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ft_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ft_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < function_kind Function >
    function_type_data::function_type_data(function_traits<Function>)
    : type_data_base{type_kind::function_, shared_traits_hash<function_traits<Function>>{}(this)}
    , flags{function_traits<Function>::make_flags()}
    , return_type{resolve_type<typename function_traits<Function>::return_type>()}
    , argument_types(function_type_data_impl::make_argument_types<Function>()) {}
}

namespace meta_hpp
{
    inline function_bitflags function_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t function_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type function_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type function_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& function_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < function_policy_family Policy, function_pointer_kind Function >
    uvalue raw_function_invoke(type_registry& registry, Function function_ptr, std::span<const uarg> args) {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using return_type = typename ft::return_type;
        using argument_types = typename ft::argument_types;

        constexpr bool as_copy                             //
            = std::is_constructible_v<uvalue, return_type> //
           && std::is_same_v<Policy, function_policy::as_copy_t>;

        constexpr bool as_void            //
            = std::is_void_v<return_type> //
           || std::is_same_v<Policy, function_policy::discard_return_t>;

        constexpr bool ref_as_ptr              //
            = std::is_reference_v<return_type> //
           && std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        META_HPP_ASSERT(             //
            args.size() == ft::arity //
            && "an attempt to call a function with an incorrect arity"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a function with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [function_ptr](auto&&... all_args) {
            if constexpr ( std::is_void_v<return_type> ) {
                function_ptr(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( std::is_same_v<Policy, function_policy::discard_return_t> ) {
                std::ignore = function_ptr(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( !std::is_void_v<return_type> ) {
                return_type&& result = function_ptr(META_HPP_FWD(all_args)...);
                return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
            }
        });
    }

    template < function_pointer_kind Function >
    uerror raw_function_invoke_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using argument_types = typename ft::argument_types;

        if ( args.size() != ft::arity ) {
            return uerror{error_code::arity_mismatch};
        }

        if ( !can_cast_all_uargs<argument_types>(registry, args) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < function_policy_family Policy, function_pointer_kind Function >
    function_state::invoke_impl make_function_invoke(type_registry& registry, Function function_ptr) {
        return [&registry, function_ptr](std::span<const uarg> args) { //
            return raw_function_invoke<Policy>(registry, function_ptr, args);
        };
    }

    template < function_pointer_kind Function >
    function_state::invoke_error_impl make_function_invoke_error(type_registry& registry) {
        return [&registry](std::span<const uarg_base> args) { //
            return raw_function_invoke_error<Function>(registry, args);
        };
    }

    template < function_pointer_kind Function >
    argument_list make_function_arguments() {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        using ft_argument_types = typename ft::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, ft_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }(std::make_index_sequence<ft::arity>());
    }
}

namespace meta_hpp::detail
{
    inline function_state::function_state(function_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < function_policy_family Policy, function_pointer_kind Function >
    function_state::state_ptr function_state::make(std::string name, Function function_ptr, metadata_map metadata) {
        using ft = function_traits<std::remove_pointer_t<Function>>;
        type_registry& registry{type_registry::instance()};

        function_state state{
            function_index{registry.resolve_by_traits<ft>(), std::move(name)},
            std::move(metadata),
        };

        state.invoke = make_function_invoke<Policy>(registry, function_ptr);
        state.invoke_error = make_function_invoke_error<Function>(registry);
        state.arguments = make_function_arguments<Function>();

        return std::make_shared<function_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline function_type function::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& function::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline std::size_t function::get_arity() const noexcept {
        return state_->arguments.size();
    }

    inline argument function::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& function::get_arguments() const noexcept {
        return state_->arguments;
    }

    template < typename... Args >
    uvalue function::invoke(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->invoke(vargs);
    }

    template < typename... Args >
    uresult function::try_invoke(Args&&... args) const {
        // doesn't actually move 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(META_HPP_FWD(args)...) ) {
            return err;
        }
        return invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uvalue function::operator()(Args&&... args) const {
        return invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    bool function::is_invocable_with() const {
        return !check_invocable_error<Args...>();
    }

    template < typename... Args >
    bool function::is_invocable_with(Args&&... args) const {
        return !check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror function::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return state_->invoke_error(vargs);
    }

    template < typename... Args >
    uerror function::check_invocable_error(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return state_->invoke_error(vargs);
    }

    template < typename Iter >
    uvalue function::invoke_variadic(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg) * arity);
        detail::inline_vector<uarg> vargs{static_cast<uarg*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->invoke({vargs.begin(), vargs.end()});
    }

    template < typename Iter >
    uresult function::try_invoke_variadic(Iter first, Iter last) const {
        if ( const uerror err = check_variadic_invocable_error(first, last) ) {
            return err;
        }
        return invoke_variadic(first, last);
    }

    template < typename Iter >
    bool function::is_variadic_invocable_with(Iter first, Iter last) const {
        return !check_variadic_invocable_error(first, last);
    }

    template < typename Iter >
    uerror function::check_variadic_invocable_error(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg_base) * arity);
        detail::inline_vector<uarg_base> vargs{static_cast<uarg_base*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                return uerror(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->invoke_error({vargs.begin(), vargs.end()});
    }
}

namespace meta_hpp::detail
{
    class uinst_base {
    public:
        enum class ref_types {
            lvalue,
            const_lvalue,
            rvalue,
            const_rvalue,
        };

    public:
        uinst_base() = default;
        ~uinst_base() = default;

        uinst_base(uinst_base&&) = default;
        uinst_base& operator=(uinst_base&&) = default;

        uinst_base(const uinst_base&) = delete;
        uinst_base& operator=(const uinst_base&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        // NOLINTNEXTLINE(*-missing-std-forward)
        explicit uinst_base(type_registry& registry, T&&)
        : uinst_base{registry, type_list<T&&>{}} {}

        template < inst_class_lvalue_ref_kind T >
        explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        template < inst_class_rvalue_ref_kind T >
        explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}
        , raw_type_{registry.resolve_by_type<std::remove_cvref_t<T>>()} {}

        explicit uinst_base(type_registry&, uvalue& v)
        : ref_type_{ref_types::lvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(type_registry&, const uvalue& v)
        : ref_type_{ref_types::const_lvalue}
        , raw_type_{v.get_type()} {}

        // NOLINTNEXTLINE(*-param-not-moved)
        explicit uinst_base(type_registry&, uvalue&& v)
        : ref_type_{ref_types::rvalue}
        , raw_type_{v.get_type()} {}

        explicit uinst_base(type_registry&, const uvalue&& v)
        : ref_type_{ref_types::const_rvalue}
        , raw_type_{v.get_type()} {}

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uinst_base(type_registry& registry, T&& v)
        : uinst_base{registry, *std::forward<T>(v)} {}

        [[nodiscard]] bool is_inst_const() const noexcept {
            if ( raw_type_.is_pointer() ) {
                const pointer_type& from_type_ptr = raw_type_.as_pointer();
                const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
                return from_type_ptr_readonly;
            }
            return ref_type_ == ref_types::const_lvalue //
                || ref_type_ == ref_types::const_rvalue;
        }

        [[nodiscard]] ref_types get_ref_type() const noexcept {
            return ref_type_;
        }

        [[nodiscard]] any_type get_raw_type() const noexcept {
            return raw_type_;
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

    private:
        ref_types ref_type_{};
        any_type raw_type_{};
    };
}

namespace meta_hpp::detail
{
    class uinst final : public uinst_base {
    public:
        uinst() = default;
        ~uinst() = default;

        uinst(uinst&&) = default;
        uinst& operator=(uinst&&) = default;

        uinst(const uinst&) = delete;
        uinst& operator=(const uinst&) = delete;

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uvalue>
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v.get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires std::is_same_v<Tp, uresult>
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<void*>(v->get_data())} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < typename T, typename Tp = std::decay_t<T> >
            requires(!uvalue_family<Tp>)
        explicit uinst(type_registry& registry, T&& v)
        : uinst_base{registry, std::forward<T>(v)}
        , data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} { // NOLINT(*-const-cast)
            // there is no 'use after move' here because
            // 'uinst_base' doesn't actually move 'v', just gets its type
        }

        template < inst_class_ref_kind Q >
        [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    private:
        void* data_{};
    };
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    bool uinst_base::can_cast_to(type_registry& registry) const noexcept {
        using inst_class = typename inst_traits<Q>::class_type;
        using inst_method = typename inst_traits<Q>::method_type;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<inst_class>();

        if ( from_type.is_class() ) {
            const auto is_invocable = [this]() {
                switch ( get_ref_type() ) {
                case ref_types::lvalue:
                    return std::is_invocable_v<inst_method, inst_class&>;
                case ref_types::const_lvalue:
                    return std::is_invocable_v<inst_method, const inst_class&>;
                case ref_types::rvalue:
                    return std::is_invocable_v<inst_method, inst_class&&>;
                case ref_types::const_rvalue:
                    return std::is_invocable_v<inst_method, const inst_class&&>;
                }
                return false;
            };

            return is_invocable() && is_a(to_type, from_type);
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
            const any_type& from_data_type = from_type_ptr.get_data_type();

            const auto is_invocable = [from_type_ptr_readonly]() {
                return from_type_ptr_readonly ? std::is_invocable_v<inst_method, const inst_class&>
                                              : std::is_invocable_v<inst_method, inst_class&>;
            };

            return is_invocable() && is_a(to_type, from_data_type);
        }

        return false;
    }
}

namespace meta_hpp::detail
{
    template < inst_class_ref_kind Q >
    decltype(auto) uinst::cast(type_registry& registry) const {
        META_HPP_DEV_ASSERT(can_cast_to<Q>(registry) && "bad instance cast");

        using inst_class_cv = std::remove_reference_t<Q>;
        using inst_class = std::remove_cv_t<inst_class_cv>;

        const any_type& from_type = get_raw_type();
        const any_type& to_type = registry.resolve_by_type<inst_class>();

        if ( from_type.is_class() && to_type.is_class() ) {
            void* to_ptr = pointer_upcast( //
                data_,
                from_type.as_class(),
                to_type.as_class()
            );
            META_HPP_ASSERT(to_ptr);

            if constexpr ( !std::is_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_lvalue_reference_v<Q> ) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr ( std::is_rvalue_reference_v<Q> ) {
                return std::move(*static_cast<inst_class_cv*>(to_ptr));
            }
        }

        if ( from_type.is_pointer() ) {
            const pointer_type& from_type_ptr = from_type.as_pointer();
            const any_type& from_data_type = from_type_ptr.get_data_type();

            if ( from_data_type.is_class() && to_type.is_class() ) {
                void* to_ptr = pointer_upcast( //
                    *static_cast<void**>(data_),
                    from_data_type.as_class(),
                    to_type.as_class()
                );
                META_HPP_ASSERT(to_ptr);

                if constexpr ( !std::is_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }

                if constexpr ( std::is_lvalue_reference_v<Q> ) {
                    return *static_cast<inst_class_cv*>(to_ptr);
                }
            }
        }

        throw_exception(error_code::bad_instance_cast);
    }
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    member_type_data::member_type_data(member_traits<Member>)
    : type_data_base{type_kind::member_, shared_traits_hash<member_traits<Member>>{}(this)}
    , flags{member_traits<Member>::make_flags()}
    , owner_type{resolve_type<typename member_traits<Member>::class_type>()}
    , value_type{resolve_type<typename member_traits<Member>::value_type>()} {}
}

namespace meta_hpp
{
    inline member_bitflags member_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline class_type member_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type member_type::get_value_type() const noexcept {
        return data_->value_type;
    }
}

namespace meta_hpp::detail
{
    template < member_policy_family Policy, member_pointer_kind Member >
    uvalue raw_member_getter(type_registry& registry, Member member_ptr, const uinst& inst) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        constexpr bool as_copy                                  //
            = std::is_constructible_v<uvalue, value_type>       //
           && std::is_same_v<Policy, member_policy::as_copy_t>; //

        constexpr bool as_ptr                                      //
            = std::is_same_v<Policy, member_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                           //
            = std::is_same_v<Policy, member_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        if ( inst.is_inst_const() ) {
            META_HPP_ASSERT(                                 //
                inst.can_cast_to<const class_type>(registry) //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<const class_type>(registry).*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{META_HPP_FWD(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        } else {
            META_HPP_ASSERT(                           //
                inst.can_cast_to<class_type>(registry) //
                && "an attempt to get a member with an incorrect instance type"
            );

            auto&& return_value = inst.cast<class_type>(registry).*member_ptr;

            if constexpr ( as_copy ) {
                return uvalue{META_HPP_FWD(return_value)};
            }

            if constexpr ( as_ptr ) {
                return uvalue{std::addressof(return_value)};
            }

            if constexpr ( as_ref_wrap ) {
                return uvalue{std::ref(return_value)};
            }
        }
    }

    template < member_pointer_kind Member >
    uerror raw_member_getter_error(type_registry& registry, const uinst_base& inst) noexcept {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;

        if ( inst.is_inst_const() ) {
            if ( !inst.can_cast_to<const class_type>(registry) ) {
                return uerror{error_code::bad_instance_cast};
            }
        } else {
            if ( !inst.can_cast_to<class_type>(registry) ) {
                return uerror{error_code::bad_instance_cast};
            }
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < member_pointer_kind Member >
    void raw_member_setter(type_registry& registry, Member member_ptr, const uinst& inst, const uarg& arg) {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( mt::is_readonly ) {
            (void)registry;
            (void)member_ptr;
            (void)inst;
            (void)arg;
            META_HPP_ASSERT(false && "an attempt to set a constant member");
        } else {
            META_HPP_ASSERT(          //
                !inst.is_inst_const() //
                && "an attempt to set a member with an const instance type"
            );

            META_HPP_ASSERT(                           //
                inst.can_cast_to<class_type>(registry) //
                && "an attempt to set a member with an incorrect instance type"
            );

            META_HPP_ASSERT(                          //
                arg.can_cast_to<value_type>(registry) //
                && "an attempt to set a member with an incorrect argument type"
            );

            inst.cast<class_type>(registry).*member_ptr = arg.cast<value_type>(registry);
        }
    }

    template < member_pointer_kind Member >
    uerror raw_member_setter_error(type_registry& registry, const uinst_base& inst, const uarg_base& arg) noexcept {
        using mt = member_traits<Member>;
        using class_type = typename mt::class_type;
        using value_type = typename mt::value_type;

        if constexpr ( mt::is_readonly ) {
            (void)registry;
            (void)inst;
            (void)arg;
            return uerror{error_code::bad_const_access};
        } else {
            if ( inst.is_inst_const() ) {
                return uerror{error_code::bad_const_access};
            }

            if ( !inst.can_cast_to<class_type>(registry) ) {
                return uerror{error_code::instance_type_mismatch};
            }

            if ( !arg.can_cast_to<value_type>(registry) ) {
                return uerror{error_code::argument_type_mismatch};
            }

            return uerror{error_code::no_error};
        }
    }
}

namespace meta_hpp::detail
{
    template < member_policy_family Policy, member_pointer_kind Member >
    member_state::getter_impl make_member_getter(type_registry& registry, Member member_ptr) {
        return [&registry, member_ptr](const uinst& inst) { //
            return raw_member_getter<Policy>(registry, member_ptr, inst);
        };
    }

    template < member_pointer_kind Member >
    member_state::getter_error_impl make_member_getter_error(type_registry& registry) {
        return [&registry](const uinst_base& inst) { //
            return raw_member_getter_error<Member>(registry, inst);
        };
    }

    template < member_pointer_kind Member >
    member_state::setter_impl make_member_setter(type_registry& registry, Member member_ptr) {
        return [&registry, member_ptr](const uinst& inst, const uarg& arg) { //
            return raw_member_setter(registry, member_ptr, inst, arg);
        };
    }

    template < member_pointer_kind Member >
    member_state::setter_error_impl make_member_setter_error(type_registry& registry) {
        return [&registry](const uinst_base& inst, const uarg_base& arg) { //
            return raw_member_setter_error<Member>(registry, inst, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline member_state::member_state(member_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < member_policy_family Policy, member_pointer_kind Member >
    member_state::state_ptr member_state::make(std::string name, Member member_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        member_state state{
            member_index{registry.resolve_by_type<Member>(), std::move(name)},
            std::move(metadata),
        };

        state.getter = make_member_getter<Policy>(registry, member_ptr);
        state.setter = make_member_setter(registry, member_ptr);
        state.getter_error = make_member_getter_error<Member>(registry);
        state.setter_error = make_member_setter_error<Member>(registry);

        return std::make_shared<member_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline member_type member::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& member::get_name() const noexcept {
        return state_->index.get_name();
    }

    template < typename Instance >
    uvalue member::get(Instance&& instance) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        return state_->getter(vinst);
    }

    template < typename Instance >
    uresult member::try_get(Instance&& instance) const {
        // doesn't actually move an 'instance', just checks conversion errors
        if ( const uerror err = check_gettable_error(META_HPP_FWD(instance)) ) {
            return err;
        }
        return get(META_HPP_FWD(instance));
    }

    template < typename Instance >
    uvalue member::operator()(Instance&& instance) const {
        return get(META_HPP_FWD(instance));
    }

    template < typename Instance, typename Value >
    void member::set(Instance&& instance, Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    uresult member::try_set(Instance&& instance, Value&& value) const {
        // doesn't actually move an 'instance' and a 'value', just checks conversion errors
        if ( const uerror err = check_settable_error(META_HPP_FWD(instance), META_HPP_FWD(value)) ) {
            return err;
        }
        set(META_HPP_FWD(instance), META_HPP_FWD(value));
        return uerror{error_code::no_error};
    }

    template < typename Instance, typename Value >
    void member::operator()(Instance&& instance, Value&& value) const {
        set(META_HPP_FWD(instance), META_HPP_FWD(value));
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with() const {
        return !check_gettable_error<Instance>();
    }

    template < typename Instance >
    [[nodiscard]] bool member::is_gettable_with(Instance&& instance) const {
        return !check_gettable_error(META_HPP_FWD(instance));
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with() const {
        return !check_settable_error<Instance, Value>();
    }

    template < typename Instance, typename Value >
    [[nodiscard]] bool member::is_settable_with(Instance&& instance, Value&& value) const {
        return !check_settable_error(META_HPP_FWD(instance), META_HPP_FWD(value));
    }

    template < typename Instance >
    [[nodiscard]] uerror member::check_gettable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        return state_->getter_error(vinst);
    }

    template < typename Instance >
    [[nodiscard]] uerror member::check_gettable_error(Instance&& instance) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return state_->getter_error(vinst);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] uerror member::check_settable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const uarg_base vvalue{registry, type_list<Value>{}};
        return state_->setter_error(vinst, vvalue);
    }

    template < typename Instance, typename Value >
    [[nodiscard]] uerror member::check_settable_error(Instance&& instance, Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        return state_->setter_error(vinst, vvalue);
    }
}

namespace meta_hpp::detail::method_type_data_impl
{
    template < method_pointer_kind Method >
    any_type_list make_argument_types() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<mt_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, mt_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<mt_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < method_pointer_kind Method >
    method_type_data::method_type_data(method_traits<Method>)
    : type_data_base{type_kind::method_, shared_traits_hash<method_traits<Method>>{}(this)}
    , flags{method_traits<Method>::make_flags()}
    , owner_type{resolve_type<typename method_traits<Method>::class_type>()}
    , return_type{resolve_type<typename method_traits<Method>::return_type>()}
    , argument_types(method_type_data_impl::make_argument_types<Method>()) {}
}

namespace meta_hpp
{
    inline method_bitflags method_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t method_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type method_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type method_type::get_return_type() const noexcept {
        return data_->return_type;
    }

    inline any_type method_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& method_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < method_policy_family Policy, method_pointer_kind Method >
    uvalue raw_method_invoke(type_registry& registry, Method method_ptr, const uinst& inst, std::span<const uarg> args) {
        using mt = method_traits<Method>;
        using return_type = typename mt::return_type;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        constexpr bool as_copy                             //
            = std::is_constructible_v<uvalue, return_type> //
           && std::is_same_v<Policy, method_policy::as_copy_t>;

        constexpr bool as_void            //
            = std::is_void_v<return_type> //
           || std::is_same_v<Policy, method_policy::discard_return_t>;

        constexpr bool ref_as_ptr              //
            = std::is_reference_v<return_type> //
           && std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

        static_assert(as_copy || as_void || ref_as_ptr);

        META_HPP_ASSERT(             //
            args.size() == mt::arity //
            && "an attempt to call a method with an incorrect arity"
        );

        META_HPP_ASSERT(                               //
            inst.can_cast_to<qualified_type>(registry) //
            && "an attempt to call a method with an incorrect instance type"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a method with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [method_ptr, &inst, &registry](auto&&... all_args) {
            if constexpr ( std::is_void_v<return_type> ) {
                (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( std::is_same_v<Policy, method_policy::discard_return_t> ) {
                std::ignore = (inst.cast<qualified_type>().*method_ptr)(META_HPP_FWD(all_args)...);
                return uvalue{};
            }

            if constexpr ( !std::is_void_v<return_type> ) {
                return_type&& result = (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
                return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
            }
        });
    }

    template < method_pointer_kind Method >
    uerror raw_method_invoke_error(type_registry& registry, const uinst_base& inst, std::span<const uarg_base> args) noexcept {
        using mt = method_traits<Method>;
        using qualified_type = typename mt::qualified_type;
        using argument_types = typename mt::argument_types;

        if ( args.size() != mt::arity ) {
            return uerror{error_code::arity_mismatch};
        }

        if ( !inst.can_cast_to<qualified_type>(registry) ) {
            return uerror{error_code::instance_type_mismatch};
        }

        if ( !can_cast_all_uargs<argument_types>(registry, args) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < method_policy_family Policy, method_pointer_kind Method >
    method_state::invoke_impl make_method_invoke(type_registry& registry, Method method_ptr) {
        return [&registry, method_ptr](const uinst& inst, std::span<const uarg> args) {
            return raw_method_invoke<Policy>(registry, method_ptr, inst, args);
        };
    }

    template < method_pointer_kind Method >
    method_state::invoke_error_impl make_method_invoke_error(type_registry& registry) {
        return [&registry](const uinst_base& inst, std::span<const uarg_base> args) {
            return raw_method_invoke_error<Method>(registry, inst, args);
        };
    }

    template < method_pointer_kind Method >
    argument_list make_method_arguments() {
        using mt = method_traits<Method>;
        using mt_argument_types = typename mt::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, mt_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }(std::make_index_sequence<mt::arity>());
    }
}

namespace meta_hpp::detail
{
    inline method_state::method_state(method_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < method_policy_family Policy, method_pointer_kind Method >
    method_state::state_ptr method_state::make(std::string name, Method method_ptr, metadata_map metadata) {
        using mt = method_traits<Method>;
        type_registry& registry{type_registry::instance()};

        method_state state{
            method_index{registry.resolve_by_traits<mt>(), std::move(name)},
            std::move(metadata),
        };

        state.invoke = make_method_invoke<Policy>(registry, method_ptr);
        state.invoke_error = make_method_invoke_error<Method>(registry);
        state.arguments = make_method_arguments<Method>();

        return std::make_shared<method_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline method_type method::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& method::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline std::size_t method::get_arity() const noexcept {
        return state_->arguments.size();
    }

    inline argument method::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& method::get_arguments() const noexcept {
        return state_->arguments;
    }

    template < typename Instance, typename... Args >
    uvalue method::invoke(Instance&& instance, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->invoke(vinst, vargs);
    }

    template < typename Instance, typename... Args >
    uresult method::try_invoke(Instance&& instance, Args&&... args) const {
        // doesn't actually move an 'instance' and 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...) ) {
            return err;
        }
        return invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uvalue method::operator()(Instance&& instance, Args&&... args) const {
        return invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with() const {
        return !check_invocable_error<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    bool method::is_invocable_with(Instance&& instance, Args&&... args) const {
        return !check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uerror method::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return state_->invoke_error(vinst, vargs);
    }

    template < typename Instance, typename... Args >
    uerror method::check_invocable_error(Instance&& instance, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return state_->invoke_error(vinst, vargs);
    }

    template < typename Instance, typename Iter >
    uvalue method::invoke_variadic(Instance&& instance, Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg) * arity);
        detail::inline_vector<uarg> vargs{static_cast<uarg*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        const uinst vinst{registry, META_HPP_FWD(instance)};
        return state_->invoke(vinst, {vargs.begin(), vargs.end()});
    }

    template < typename Instance, typename Iter >
    uresult method::try_invoke_variadic(Instance&& instance, Iter first, Iter last) const {
        // doesn't actually move an 'instance', just checks conversion errors
        if ( const uerror err = check_variadic_invocable_error(META_HPP_FWD(instance), first, last) ) {
            return err;
        }
        return invoke_variadic(META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter >
    bool method::is_variadic_invocable_with(Instance&& instance, Iter first, Iter last) const {
        return !check_variadic_invocable_error(META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter >
    uerror method::check_variadic_invocable_error(Instance&& instance, Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg_base) * arity);
        detail::inline_vector<uarg_base> vargs{static_cast<uarg_base*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                return uerror(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return state_->invoke_error(vinst, {vargs.begin(), vargs.end()});
    }
}

namespace meta_hpp
{
    template < typename... Args >
    uvalue invoke(const function& function, Args&&... args) {
        return function.invoke(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uresult try_invoke(const function& function, Args&&... args) {
        return function.try_invoke(META_HPP_FWD(args)...);
    }

    template < function_pointer_kind Function, typename... Args >
    uvalue invoke(Function function_ptr, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }

    template < function_pointer_kind Function, typename... Args >
    uresult try_invoke(Function function_ptr, Args&&... args) {
        // doesn't actually move 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(function_ptr, META_HPP_FWD(args)...) ) {
            return err;
        }
        return invoke(function_ptr, META_HPP_FWD(args)...);
    }

    template < typename Iter >
    uvalue invoke_variadic(const function& function, Iter first, Iter last) {
        return function.invoke_variadic(first, last);
    }

    template < typename Iter >
    uresult try_invoke_variadic(const function& function, Iter first, Iter last) {
        return function.try_invoke_variadic(first, last);
    }

    template < function_pointer_kind Function, typename Iter >
    uvalue invoke_variadic(Function function_ptr, Iter first, Iter last) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        using ft = function_traits<std::remove_pointer_t<Function>>;
        std::array<uarg, ft::arity> vargs;

        for ( std::size_t count{}; first != last; ++count, ++first ) {
            if ( count >= ft::arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs[count] = uarg{registry, *first};
        }

        return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
    }

    template < function_pointer_kind Function, typename Iter >
    uresult try_invoke_variadic(Function function_ptr, Iter first, Iter last) {
        if ( const uerror err = check_variadic_invocable_error(function_ptr, first, last) ) {
            return err;
        }
        return invoke_variadic(function_ptr, first, last);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    uvalue invoke(const member& member, Instance&& instance) {
        return member.get(META_HPP_FWD(instance));
    }

    template < typename Instance >
    uresult try_invoke(const member& member, Instance&& instance) {
        return member.try_get(META_HPP_FWD(instance));
    }

    template < member_pointer_kind Member, typename Instance >
    uvalue invoke(Member member_ptr, Instance&& instance) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
    }

    template < member_pointer_kind Member, typename Instance >
    uresult try_invoke(Member member_ptr, Instance&& instance) {
        // doesn't actually move an 'instance', just checks conversion errors
        if ( const uerror err = check_invocable_error(member_ptr, META_HPP_FWD(instance)) ) {
            return err;
        }
        return invoke(member_ptr, META_HPP_FWD(instance));
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uresult try_invoke(const method& method, Instance&& instance, Args&&... args) {
        return method.try_invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }

    template < method_pointer_kind Method, typename Instance, typename... Args >
    uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args) {
        // doesn't actually move an 'instance' and 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(method_ptr, META_HPP_FWD(instance), META_HPP_FWD(args)...) ) {
            return err;
        }
        return invoke(method_ptr, META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename Iter >
    uvalue invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last) {
        return method.invoke_variadic(META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter >
    uresult try_invoke_variadic(const method& method, Instance&& instance, Iter first, Iter last) {
        return method.try_invoke_variadic(META_HPP_FWD(instance), first, last);
    }

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uvalue invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        using mt = method_traits<Method>;
        std::array<uarg, mt::arity> vargs;

        for ( std::size_t count{}; first != last; ++count, ++first ) {
            if ( count >= mt::arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs[count] = uarg{registry, *first};
        }

        const uinst vinst{registry, META_HPP_FWD(instance)};
        return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
    }

    template < method_pointer_kind Method, typename Instance, typename Iter >
    uresult try_invoke_variadic(Method method_ptr, Instance&& instance, Iter first, Iter last) {
        // doesn't actually move an 'instance', just checks conversion errors
        if ( const uerror err = check_variadic_invocable_error(method_ptr, META_HPP_FWD(instance), first, last) ) {
            return err;
        }
        return invoke_variadic(method_ptr, META_HPP_FWD(instance), first, last);
    }
}

namespace meta_hpp
{
    template < typename... Args >
    bool is_invocable_with(const function& function) noexcept {
        return !check_invocable_error<Args...>(function);
    }

    template < typename... Args >
    bool is_invocable_with(const function& function, Args&&... args) noexcept {
        return !check_invocable_error(function, META_HPP_FWD(args)...);
    }

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr) noexcept {
        return !check_invocable_error<Args...>(function_ptr);
    }

    template < typename... Args, function_pointer_kind Function >
    bool is_invocable_with(Function function_ptr, Args&&... args) noexcept {
        return !check_invocable_error(function_ptr, META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror check_invocable_error(const function& function) noexcept {
        return function.check_invocable_error<Args...>();
    }

    template < typename... Args >
    uerror check_invocable_error(const function& function, Args&&... args) noexcept {
        return function.check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return raw_function_invoke_error<Function>(registry, vargs);
    }

    template < typename... Args, function_pointer_kind Function >
    uerror check_invocable_error(Function, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return raw_function_invoke_error<Function>(registry, vargs);
    }

    template < typename Iter >
    bool is_variadic_invocable_with(const function& function, Iter first, Iter last) {
        return function.is_variadic_invocable_with(first, last);
    }

    template < typename Iter, function_pointer_kind Function >
    bool is_variadic_invocable_with(Function function_ptr, Iter first, Iter last) {
        return !check_variadic_invocable_error(function_ptr, first, last);
    }

    template < typename Iter >
    uerror check_variadic_invocable_error(const function& function, Iter first, Iter last) {
        return function.check_variadic_invocable_error(first, last);
    }

    template < typename Iter, function_pointer_kind Function >
    uerror check_variadic_invocable_error(Function, Iter first, Iter last) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        using ft = function_traits<std::remove_pointer_t<Function>>;
        std::array<uarg_base, ft::arity> vargs;

        for ( std::size_t count{}; first != last; ++count, ++first ) {
            if ( count >= ft::arity ) {
                return uerror{error_code::arity_mismatch};
            }
            vargs[count] = uarg_base{registry, *first};
        }

        return raw_function_invoke_error<Function>(registry, vargs);
    }
}

namespace meta_hpp
{
    template < typename Instance >
    bool is_invocable_with(const member& member) noexcept {
        return !check_invocable_error<Instance>(member);
    }

    template < typename Instance >
    bool is_invocable_with(const member& member, Instance&& instance) noexcept {
        return !check_invocable_error(member, META_HPP_FWD(instance));
    }

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr) noexcept {
        return !check_invocable_error<Instance>(member_ptr);
    }

    template < typename Instance, member_pointer_kind Member >
    bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept {
        return !check_invocable_error(member_ptr, META_HPP_FWD(instance));
    }

    template < typename Instance >
    uerror check_invocable_error(const member& member) noexcept {
        return member.check_gettable_error<Instance>();
    }

    template < typename Instance >
    uerror check_invocable_error(const member& member, Instance&& instance) noexcept {
        return member.check_gettable_error(META_HPP_FWD(instance));
    }

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        return raw_member_getter_error<Member>(registry, vinst);
    }

    template < typename Instance, member_pointer_kind Member >
    uerror check_invocable_error(Member, Instance&& instance) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return raw_member_getter_error<Member>(registry, vinst);
    }
}

namespace meta_hpp
{
    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method) noexcept {
        return !check_invocable_error<Instance, Args...>(method);
    }

    template < typename Instance, typename... Args >
    bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept {
        return !check_invocable_error(method, META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr) noexcept {
        return !check_invocable_error<Instance, Args...>(method_ptr);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept {
        return !check_invocable_error(method_ptr, META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method) noexcept {
        return method.check_invocable_error<Instance, Args...>();
    }

    template < typename Instance, typename... Args >
    uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept {
        return method.check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, type_list<Instance>{}};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return raw_method_invoke_error<Method>(registry, vinst, vargs);
    }

    template < typename Instance, typename... Args, method_pointer_kind Method >
    uerror check_invocable_error(Method, Instance&& instance, Args&&... args) noexcept {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return raw_method_invoke_error<Method>(registry, vinst, vargs);
    }

    template < typename Instance, typename Iter >
    bool is_variadic_invocable_with(const method& method, Instance&& instance, Iter first, Iter last) {
        return method.is_variadic_invocable_with(META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter, method_pointer_kind Method >
    bool is_variadic_invocable_with(Method method_ptr, Instance&& instance, Iter first, Iter last) {
        return !check_variadic_invocable_error(method_ptr, META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter >
    uerror check_variadic_invocable_error(const method& method, Instance&& instance, Iter first, Iter last) {
        return method.check_variadic_invocable_error(META_HPP_FWD(instance), first, last);
    }

    template < typename Instance, typename Iter, method_pointer_kind Method >
    uerror check_variadic_invocable_error(Method, Instance&& instance, Iter first, Iter last) {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        using mt = method_traits<Method>;
        std::array<uarg_base, mt::arity> vargs;

        for ( std::size_t count{}; first != last; ++count, ++first ) {
            if ( count >= mt::arity ) {
                return uerror{error_code::arity_mismatch};
            }
            vargs[count] = uarg_base{registry, *first};
        }

        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        return raw_method_invoke_error<Method>(registry, vinst, vargs);
    }
}

namespace meta_hpp::detail
{
    inline argument_state::argument_state(argument_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < typename Argument >
    inline argument_state::state_ptr argument_state::make(std::size_t position, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        argument_state state{
            argument_index{registry.resolve_by_type<Argument>(), position},
            std::move(metadata),
        };

        return std::make_shared<argument_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline any_type argument::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline std::size_t argument::get_position() const noexcept {
        return state_->index.get_position();
    }

    inline const std::string& argument::get_name() const noexcept {
        return state_->name;
    }
}

namespace meta_hpp::detail::constructor_type_data_impl
{
    template < class_kind Class, typename... Args >
    any_type_list make_argument_types() {
        using ct = constructor_traits<Class, Args...>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ct_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ct_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ct_argument_types>>());
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class, typename... Args >
    constructor_type_data::constructor_type_data(constructor_traits<Class, Args...>)
    : type_data_base{type_kind::constructor_, shared_traits_hash<constructor_traits<Class, Args...>>{}(this)}
    , flags{constructor_traits<Class, Args...>::make_flags()}
    , owner_type{resolve_type<typename constructor_traits<Class, Args...>::class_type>()}
    , argument_types(constructor_type_data_impl::make_argument_types<Class, Args...>()) {}
}

namespace meta_hpp
{
    inline constructor_bitflags constructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t constructor_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline class_type constructor_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }

    inline any_type constructor_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const any_type_list& constructor_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    uvalue raw_constructor_create(type_registry& registry, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        constexpr bool as_object //
            = std::is_same_v<Policy, constructor_policy::as_object_t>;

        constexpr bool as_raw_ptr //
            = std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>;

        constexpr bool as_shared_ptr //
            = std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

        constexpr bool as_unique_ptr //
            = std::is_same_v<Policy, constructor_policy::as_unique_pointer_t>;

        static_assert(as_object || as_raw_ptr || as_shared_ptr || as_unique_ptr);

        META_HPP_ASSERT(             //
            args.size() == ct::arity //
            && "an attempt to call a constructor with an incorrect arity"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [](auto&&... all_args) -> uvalue {
            if constexpr ( as_object ) {
                return make_uvalue<class_type>(META_HPP_FWD(all_args)...);
            }

            if constexpr ( as_raw_ptr ) {
                return std::make_unique<class_type>(META_HPP_FWD(all_args)...).release();
            }

            if constexpr ( as_shared_ptr ) {
                return std::make_shared<class_type>(META_HPP_FWD(all_args)...);
            }

            if constexpr ( as_unique_ptr ) {
                return std::make_unique<class_type>(META_HPP_FWD(all_args)...);
            }
        });
    }

    template < class_kind Class, typename... Args >
    uvalue raw_constructor_create_at(type_registry& registry, void* mem, std::span<const uarg> args) {
        using ct = constructor_traits<Class, Args...>;
        using class_type = typename ct::class_type;
        using argument_types = typename ct::argument_types;

        META_HPP_ASSERT(             //
            args.size() == ct::arity //
            && "an attempt to call a constructor with an incorrect arity"
        );

        META_HPP_ASSERT(                                       //
            can_cast_all_uargs<argument_types>(registry, args) //
            && "an attempt to call a constructor with incorrect argument types"
        );

        return unchecked_call_with_uargs<argument_types>(registry, args, [mem](auto&&... all_args) {
            return std::construct_at(static_cast<class_type*>(mem), META_HPP_FWD(all_args)...);
        });
    }

    template < class_kind Class, typename... Args >
    uerror raw_constructor_create_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
        using ct = constructor_traits<Class, Args...>;
        using argument_types = typename ct::argument_types;

        if ( args.size() != ct::arity ) {
            return uerror{error_code::arity_mismatch};
        }

        if ( !can_cast_all_uargs<argument_types>(registry, args) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    constructor_state::create_impl make_constructor_create(type_registry& registry) {
        return [&registry](std::span<const uarg> args) { //
            return raw_constructor_create<Policy, Class, Args...>(registry, args);
        };
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_at_impl make_constructor_create_at(type_registry& registry) {
        return [&registry](void* mem, std::span<const uarg> args) { //
            return raw_constructor_create_at<Class, Args...>(registry, mem, args);
        };
    }

    template < class_kind Class, typename... Args >
    constructor_state::create_error_impl make_constructor_create_error(type_registry& registry) {
        return [&registry](std::span<const uarg_base> args) { //
            return raw_constructor_create_error<Class, Args...>(registry, args);
        };
    }

    template < class_kind Class, typename... Args >
    argument_list make_constructor_arguments() {
        using ct = constructor_traits<Class, Args...>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
                using P = type_list_at_t<I, ct_argument_types>;
                return argument{argument_state::make<P>(I, metadata_map{})};
            };
            return argument_list{make_argument(index_constant<Is>{})...};
        }(std::make_index_sequence<ct::arity>());
    }
}

namespace meta_hpp::detail
{
    inline constructor_state::constructor_state(constructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < constructor_policy_family Policy, class_kind Class, typename... Args >
    constructor_state::state_ptr constructor_state::make(metadata_map metadata) {
        using ct = constructor_traits<Class, Args...>;
        type_registry& registry{type_registry::instance()};

        constructor_state state{
            constructor_index{registry.resolve_by_traits<ct>()},
            std::move(metadata),
        };

        state.create = make_constructor_create<Policy, Class, Args...>(registry);
        state.create_at = make_constructor_create_at<Class, Args...>(registry);
        state.create_error = make_constructor_create_error<Class, Args...>(registry);
        state.arguments = make_constructor_arguments<Class, Args...>();

        return std::make_shared<constructor_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline constructor_type constructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline std::size_t constructor::get_arity() const noexcept {
        return state_->arguments.size();
    }

    inline argument constructor::get_argument(std::size_t position) const noexcept {
        return position < state_->arguments.size() ? state_->arguments[position] : argument{};
    }

    inline const argument_list& constructor::get_arguments() const noexcept {
        return state_->arguments;
    }

    template < typename... Args >
    uvalue constructor::create(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create(vargs);
    }

    template < typename... Args >
    uresult constructor::try_create(Args&&... args) const {
        // doesn't actually move 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(META_HPP_FWD(args)...) ) {
            return err;
        }
        return create(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uvalue constructor::create_at(void* mem, Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
        return state_->create_at(mem, vargs);
    }

    template < typename... Args >
    uresult constructor::try_create_at(void* mem, Args&&... args) const {
        // doesn't actually move 'args', just checks conversion errors
        if ( const uerror err = check_invocable_error(META_HPP_FWD(args)...) ) {
            return err;
        }
        return create_at(mem, META_HPP_FWD(args)...);
    }

    template < typename... Args >
    bool constructor::is_invocable_with() const {
        return !check_invocable_error<Args...>();
    }

    template < typename... Args >
    bool constructor::is_invocable_with(Args&&... args) const {
        return !check_invocable_error(META_HPP_FWD(args)...);
    }

    template < typename... Args >
    uerror constructor::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
        return state_->create_error(vargs);
    }

    template < typename... Args >
    uerror constructor::check_invocable_error(Args&&... args) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        return state_->create_error(vargs);
    }

    template < typename Iter >
    uvalue constructor::create_variadic(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg) * arity);
        detail::inline_vector<uarg> vargs{static_cast<uarg*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->create({vargs.begin(), vargs.end()});
    }

    template < typename Iter >
    uresult constructor::try_create_variadic(Iter first, Iter last) const {
        if ( const uerror err = check_variadic_invocable_error(first, last) ) {
            return err;
        }
        return create_variadic(first, last);
    }

    template < typename Iter >
    uvalue constructor::create_variadic_at(void* mem, Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg) * arity);
        detail::inline_vector<uarg> vargs{static_cast<uarg*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                throw_exception(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->create_at(mem, {vargs.begin(), vargs.end()});
    }

    template < typename Iter >
    uresult constructor::try_create_variadic_at(void* mem, Iter first, Iter last) const {
        if ( const uerror err = check_variadic_invocable_error(first, last) ) {
            return err;
        }
        return create_variadic_at(mem, first, last);
    }

    template < typename Iter >
    bool constructor::is_variadic_invocable_with(Iter first, Iter last) const {
        return !check_variadic_invocable_error(first, last);
    }

    template < typename Iter >
    uerror constructor::check_variadic_invocable_error(Iter first, Iter last) const {
        using namespace detail;

        const std::size_t arity = get_arity();
        type_registry& registry{type_registry::instance()};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_PUSH()

        void* vargs_mem = META_HPP_ALLOCA(sizeof(uarg_base) * arity);
        detail::inline_vector<uarg_base> vargs{static_cast<uarg_base*>(vargs_mem), arity};

        META_HPP_DETAIL_IGNORE_ALLOCA_WARNINGS_POP()

        for ( std::size_t i{}; first != last; ++i, ++first ) {
            if ( i >= arity ) {
                return uerror(error_code::arity_mismatch);
            }
            vargs.emplace_back(registry, *first);
        }

        return state_->create_error({vargs.begin(), vargs.end()});
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_type_data::destructor_type_data(destructor_traits<Class>)
    : type_data_base{type_kind::destructor_, shared_traits_hash<destructor_traits<Class>>{}(this)}
    , flags{destructor_traits<Class>::make_flags()}
    , owner_type{resolve_type<typename destructor_traits<Class>::class_type>()} {}
}

namespace meta_hpp
{
    inline destructor_bitflags destructor_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline class_type destructor_type::get_owner_type() const noexcept {
        return data_->owner_type;
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    void raw_destructor_destroy(type_registry& registry, const uarg& arg) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        META_HPP_ASSERT(                           //
            arg.can_cast_to<class_type*>(registry) //
            && "an attempt to call a destructor with an incorrect argument type"
        );

        std::unique_ptr<class_type>{arg.cast<class_type*>(registry)}.reset();
    }

    template < class_kind Class >
    void raw_destructor_destroy_at(void* mem) {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        std::destroy_at(static_cast<class_type*>(mem));
    }

    template < class_kind Class >
    uerror raw_destructor_destroy_error(type_registry& registry, const uarg_base& arg) noexcept {
        using dt = destructor_traits<Class>;
        using class_type = typename dt::class_type;

        if ( !arg.can_cast_to<class_type*>(registry) ) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    destructor_state::destroy_impl make_destructor_destroy(type_registry& registry) {
        return [&registry](const uarg& arg) { //
            return raw_destructor_destroy<Class>(registry, arg);
        };
    }

    template < class_kind Class >
    destructor_state::destroy_at_impl make_destructor_destroy_at() {
        return &raw_destructor_destroy_at<Class>;
    }

    template < class_kind Class >
    destructor_state::destroy_error_impl make_destructor_destroy_error(type_registry& registry) {
        return [&registry](const uarg_base& arg) { //
            return raw_destructor_destroy_error<Class>(registry, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline destructor_state::destructor_state(destructor_index nindex, metadata_map nmetadata)
    : index{nindex}
    , metadata{std::move(nmetadata)} {}

    template < class_kind Class >
    destructor_state::state_ptr destructor_state::make(metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        destructor_state state{
            destructor_index{registry.resolve_by_traits<destructor_traits<Class>>()},
            std::move(metadata),
        };

        state.destroy = make_destructor_destroy<Class>(registry);
        state.destroy_at = make_destructor_destroy_at<Class>();
        state.destroy_error = make_destructor_destroy_error<Class>(registry);

        return std::make_shared<destructor_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline destructor_type destructor::get_type() const noexcept {
        return state_->index.get_type();
    }

    template < typename Arg >
    void destructor::destroy(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg varg{registry, META_HPP_FWD(arg)};
        return state_->destroy(varg);
    }

    template < typename Arg >
    uresult destructor::try_destroy(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move an 'arg', just checks conversion errors
            const uarg_base varg{registry, META_HPP_FWD(arg)};
            if ( const uerror err = state_->destroy_error(varg) ) {
                return err;
            }
        }

        const uarg varg{registry, META_HPP_FWD(arg)};
        state_->destroy(varg);
        return uerror{error_code::no_error};
    }

    inline void destructor::destroy_at(void* mem) const {
        state_->destroy_at(mem);
    }

    inline uresult destructor::try_destroy_at(void* mem) const {
        state_->destroy_at(mem);
        return uerror{error_code::no_error};
    }

    template < typename Arg >
    bool destructor::is_invocable_with() const {
        return !check_invocable_error<Arg>();
    }

    template < typename Arg >
    bool destructor::is_invocable_with(Arg&& arg) const {
        return !check_invocable_error(META_HPP_FWD(arg));
    }

    template < typename Arg >
    uerror destructor::check_invocable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base varg{registry, type_list<Arg>{}};
        return state_->destroy_error(varg);
    }

    template < typename Arg >
    uerror destructor::check_invocable_error(Arg&& arg) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base varg{registry, META_HPP_FWD(arg)};
        return state_->destroy_error(varg);
    }
}

namespace meta_hpp::detail
{
    template < enum_kind Enum >
    enum_type_data::enum_type_data(enum_traits<Enum>)
    : type_data_base{type_kind::enum_, shared_traits_hash<enum_traits<Enum>>{}(this)}
    , flags{enum_traits<Enum>::make_flags()}
    , underlying_type{resolve_type<typename enum_traits<Enum>::underlying_type>()} {}
}

namespace meta_hpp
{
    inline enum_bitflags enum_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline number_type enum_type::get_underlying_type() const noexcept {
        return data_->underlying_type;
    }

    inline const evalue_list& enum_type::get_evalues() const noexcept {
        return data_->evalues;
    }

    inline evalue enum_type::get_evalue(std::string_view name) const noexcept {
        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_name() == name ) {
                return evalue;
            }
        }
        return evalue{};
    }

    template < enum_kind Enum >
    std::string_view enum_type::value_to_name(Enum value) const {
        if ( resolve_type<Enum>() != *this ) {
            return std::string_view{};
        }

        for ( const evalue& evalue : data_->evalues ) {
            if ( evalue.get_value().as<Enum>() == value ) {
                return evalue.get_name();
            }
        }

        return std::string_view{};
    }

    inline const uvalue& enum_type::name_to_value(std::string_view name) const noexcept {
        if ( const evalue& value = get_evalue(name) ) {
            return value.get_value();
        }
        static const uvalue empty_value;
        return empty_value;
    }
}

namespace meta_hpp::detail
{
    inline evalue_state::evalue_state(evalue_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < enum_kind Enum >
    evalue_state::state_ptr evalue_state::make(std::string name, Enum evalue, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        evalue_state state{
            evalue_index{registry.resolve_by_type<Enum>(), std::move(name)},
            std::move(metadata),
        };

        state.enum_value = uvalue{evalue};
        state.underlying_value = uvalue{to_underlying(evalue)};

        return std::make_shared<evalue_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline enum_type evalue::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& evalue::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const uvalue& evalue::get_value() const noexcept {
        return state_->enum_value;
    }

    inline const uvalue& evalue::get_underlying_value() const noexcept {
        return state_->underlying_value;
    }
}

namespace meta_hpp::detail
{
    template < pointer_kind Pointer >
    pointer_type_data::pointer_type_data(pointer_traits<Pointer>)
    : type_data_base{type_kind::pointer_, shared_traits_hash<pointer_traits<Pointer>>{}(this)}
    , flags{pointer_traits<Pointer>::make_flags()}
    , data_type{resolve_type<typename pointer_traits<Pointer>::data_type>()} {}
}

namespace meta_hpp
{
    inline pointer_bitflags pointer_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type pointer_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_family Policy, pointer_kind Pointer >
    uvalue raw_variable_getter(type_registry&, Pointer variable_ptr) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        constexpr bool as_copy                                    //
            = std::is_constructible_v<uvalue, data_type>          //
           && std::is_same_v<Policy, variable_policy::as_copy_t>; //

        constexpr bool as_ptr                                        //
            = std::is_same_v<Policy, variable_policy::as_pointer_t>; //

        constexpr bool as_ref_wrap                                             //
            = std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>; //

        static_assert(as_copy || as_ptr || as_ref_wrap);

        auto&& return_value = *variable_ptr;

        if constexpr ( as_copy ) {
            return uvalue{META_HPP_FWD(return_value)};
        }

        if constexpr ( as_ptr ) {
            return uvalue{std::addressof(return_value)};
        }

        if constexpr ( as_ref_wrap ) {
            return uvalue{std::ref(return_value)};
        }
    }

    template < pointer_kind Pointer >
    void raw_variable_setter(type_registry& registry, Pointer variable_ptr, const uarg& arg) {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( pt::is_readonly ) {
            (void)registry;
            (void)variable_ptr;
            (void)arg;
            META_HPP_ASSERT(false && "an attempt to set a constant variable");
        } else {
            META_HPP_ASSERT(                         //
                arg.can_cast_to<data_type>(registry) //
                && "an attempt to set a variable with an incorrect argument type"
            );

            *variable_ptr = arg.cast<data_type>(registry);
        }
    }

    template < pointer_kind Pointer >
    uerror raw_variable_setter_error(type_registry& registry, const uarg_base& arg) noexcept {
        using pt = pointer_traits<Pointer>;
        using data_type = typename pt::data_type;

        if constexpr ( pt::is_readonly ) {
            (void)registry;
            (void)arg;
            return uerror{error_code::bad_const_access};
        } else {
            if ( !arg.can_cast_to<data_type>(registry) ) {
                return uerror{error_code::argument_type_mismatch};
            }
            return uerror{error_code::no_error};
        }
    }
}

namespace meta_hpp::detail
{
    template < variable_policy_family Policy, pointer_kind Pointer >
    variable_state::getter_impl make_variable_getter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr]() { //
            return raw_variable_getter<Policy>(registry, variable_ptr);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_impl make_variable_setter(type_registry& registry, Pointer variable_ptr) {
        return [&registry, variable_ptr](const uarg& arg) { //
            return raw_variable_setter(registry, variable_ptr, arg);
        };
    }

    template < pointer_kind Pointer >
    variable_state::setter_error_impl make_variable_setter_error(type_registry& registry) {
        return [&registry](const uarg_base& arg) { //
            return raw_variable_setter_error<Pointer>(registry, arg);
        };
    }
}

namespace meta_hpp::detail
{
    inline variable_state::variable_state(variable_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    template < variable_policy_family Policy, pointer_kind Pointer >
    variable_state::state_ptr variable_state::make(std::string name, Pointer variable_ptr, metadata_map metadata) {
        type_registry& registry{type_registry::instance()};

        variable_state state{
            variable_index{registry.resolve_by_type<Pointer>(), std::move(name)},
            std::move(metadata),
        };

        state.getter = make_variable_getter<Policy>(registry, variable_ptr);
        state.setter = make_variable_setter(registry, variable_ptr);
        state.setter_error = make_variable_setter_error<Pointer>(registry);

        return std::make_shared<variable_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline pointer_type variable::get_type() const noexcept {
        return state_->index.get_type();
    }

    inline const std::string& variable::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline uvalue variable::get() const {
        return state_->getter();
    }

    inline uresult variable::try_get() const {
        return state_->getter();
    }

    inline uvalue variable::operator()() const {
        return get();
    }

    template < typename Value >
    void variable::set(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vvalue);
    }

    template < typename Value >
    uresult variable::try_set(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};

        {
            // doesn't actually move a 'value', just checks conversion errors
            const uarg_base vvalue{registry, META_HPP_FWD(value)};
            if ( const uerror err = state_->setter_error(vvalue) ) {
                return err;
            }
        }

        const uarg vvalue{registry, META_HPP_FWD(value)};
        state_->setter(vvalue);
        return uerror{error_code::no_error};
    }

    template < typename Value >
    void variable::operator()(Value&& value) const {
        set(META_HPP_FWD(value));
    }

    template < typename Value >
    bool variable::is_settable_with() const {
        return !check_settable_error<Value>();
    }

    template < typename Value >
    bool variable::is_settable_with(Value&& value) const {
        return !check_settable_error(META_HPP_FWD(value));
    }

    template < typename Value >
    uerror variable::check_settable_error() const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, type_list<Value>{}};
        return state_->setter_error(vvalue);
    }

    template < typename Value >
    uerror variable::check_settable_error(Value&& value) const {
        using namespace detail;
        type_registry& registry{type_registry::instance()};
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        return state_->setter_error(vvalue);
    }
}

namespace meta_hpp::detail::class_type_data_impl
{
    struct new_base_info_t final {
        class_list base_classes;
        class_type_data::deep_upcasts_t deep_upcasts;
    };

    template < class_kind Class, class_kind Target >
    void add_upcast_info(new_base_info_t& info) {
        const class_type_data::upcast_func_t::upcast_t class_to_target = []() {
            if constexpr ( requires { static_cast<Target*>(std::declval<Class*>()); } ) {
                return +[](void* from) -> void* { //
                    return static_cast<Target*>(static_cast<Class*>(from));
                };
            } else {
                return nullptr;
            }
        }();

        info.deep_upcasts.push_back(class_type_data::upcast_func_t{
            .target{resolve_type<Target>().get_id()},
            .upcast{class_to_target},
        });

        if constexpr ( check_base_info_enabled<Target> ) {
            [&info]<typename... TargetBases>(type_list<TargetBases...>) {
                (add_upcast_info<Class, TargetBases>(info), ...);
            }(get_meta_base_info<Target>{});
        }
    }

    template < class_kind Class >
    void fill_upcast_info(new_base_info_t& info) {
        if constexpr ( check_base_info_enabled<Class> ) {
            [&info]<typename... ClassBases>(type_list<ClassBases...>) {
                (info.base_classes.push_back(resolve_type<ClassBases>()), ...);
                (add_upcast_info<Class, ClassBases>(info), ...);
            }(get_meta_base_info<Class>{});
        }
    }

    template < class_kind Class >
    any_type_list make_argument_types() {
        using ct = class_traits<Class>;
        using ct_argument_types = typename ct::argument_types;

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            any_type_list argument_types;
            argument_types.reserve(type_list_arity_v<ct_argument_types>);

            [[maybe_unused]] const auto make_argument_type = []<std::size_t I>(index_constant<I>) {
                return resolve_type<type_list_at_t<I, ct_argument_types>>();
            };

            (argument_types.emplace_back(make_argument_type(index_constant<Is>{})), ...);
            return argument_types;
        }(std::make_index_sequence<type_list_arity_v<ct_argument_types>>());
    }

    template < class_kind Class >
    uvalue_list make_argument_values() {
        using ct = class_traits<Class>;
        using ct_argument_values = decltype(ct::argument_values);

        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            uvalue_list argument_values;
            argument_values.reserve(std::tuple_size_v<ct_argument_values>);

            [[maybe_unused]] const auto make_argument_value = []<std::size_t I>(index_constant<I>) {
                return overloaded{
                    [](decltype(std::ignore)) { return uvalue{}; },
                    [](auto&& value) { return uvalue{META_HPP_FWD(value)}; },
                }(std::get<I>(ct::argument_values));
            };

            (argument_values.emplace_back(make_argument_value(index_constant<Is>{})), ...);
            return argument_values;
        }(std::make_index_sequence<std::tuple_size_v<ct_argument_values>>());
    }
}

namespace meta_hpp::detail
{
    template < class_kind Class >
    class_type_data::class_type_data(class_traits<Class>)
    : type_data_base{type_kind::class_, shared_traits_hash<class_traits<Class>>{}(this)}
    , flags{class_traits<Class>::make_flags()}
    , size{class_traits<Class>::size}
    , align{class_traits<Class>::align}
    , argument_types(class_type_data_impl::make_argument_types<Class>())
    , argument_values(class_type_data_impl::make_argument_values<Class>()) {
        class_type_data_impl::new_base_info_t new_base_info;
        class_type_data_impl::fill_upcast_info<Class>(new_base_info);
        base_classes.swap(new_base_info.base_classes);
        deep_upcasts.swap(new_base_info.deep_upcasts);
    }
}

namespace meta_hpp::detail
{
    inline void* class_type_data::upcast_func_t::apply(void* ptr) const noexcept {
        return upcast(ptr);
    }

    inline const void* class_type_data::upcast_func_t::apply(const void* ptr) const noexcept {
        // NOLINTNEXTLINE(*-const-cast)
        return apply(const_cast<void*>(ptr));
    }
}

namespace meta_hpp
{
    inline class_bitflags class_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t class_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t class_type::get_align() const noexcept {
        return data_->align;
    }

    inline std::size_t class_type::get_arity() const noexcept {
        return data_->argument_types.size();
    }

    inline any_type class_type::get_argument_type(std::size_t position) const noexcept {
        return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{};
    }

    inline const uvalue& class_type::get_argument_value(std::size_t position) const noexcept {
        if ( position < data_->argument_values.size() ) {
            return data_->argument_values[position];
        }
        static const uvalue empty_value;
        return empty_value;
    }

    inline const any_type_list& class_type::get_argument_types() const noexcept {
        return data_->argument_types;
    }

    inline const uvalue_list& class_type::get_argument_values() const noexcept {
        return data_->argument_values;
    }

    inline const class_list& class_type::get_base_classes() const noexcept {
        return data_->base_classes;
    }

    inline const constructor_list& class_type::get_constructors() const noexcept {
        return data_->constructors;
    }

    inline const function_list& class_type::get_functions() const noexcept {
        return data_->functions;
    }

    inline const member_list& class_type::get_members() const noexcept {
        return data_->members;
    }

    inline const method_list& class_type::get_methods() const noexcept {
        return data_->methods;
    }

    inline const typedef_map& class_type::get_typedefs() const noexcept {
        return data_->typedefs;
    }

    inline const variable_list& class_type::get_variables() const noexcept {
        return data_->variables;
    }

    template < typename... Args >
    uvalue class_type::create(Args&&... args) const {
        for ( const constructor& constructor : data_->constructors ) {
            if ( constructor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return constructor.create(META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename... Args >
    uvalue class_type::create_at(void* mem, Args&&... args) const {
        for ( const constructor& constructor : data_->constructors ) {
            if ( constructor.is_invocable_with(META_HPP_FWD(args)...) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move 'args'
                return constructor.create_at(mem, META_HPP_FWD(args)...);
            }
        }
        return uvalue{};
    }

    template < typename Arg >
    bool class_type::destroy(Arg&& arg) const {
        if ( const destructor& destructor = get_destructor() ) {
            if ( destructor.is_invocable_with(META_HPP_FWD(arg)) ) {
                // there is no 'use after move' here because
                // 'is_invocable_with' doesn't actually move an 'arg'
                destructor.destroy(META_HPP_FWD(arg));
                return true;
            }
        }
        return false;
    }

    inline bool class_type::destroy_at(void* mem) const {
        if ( const destructor& destructor = get_destructor() ) {
            destructor.destroy_at(mem);
            return true;
        }
        return false;
    }

    template < class_kind Derived >
    bool class_type::is_base_of() const {
        return is_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        for ( const auto& upcast : derived.data_->deep_upcasts ) {
            if ( upcast.target == *this ) {
                return true;
            }
        }

        return false;
    }

    template < class_kind Derived >
    bool class_type::is_direct_base_of() const {
        return is_direct_base_of(resolve_type<Derived>());
    }

    inline bool class_type::is_direct_base_of(const class_type& derived) const noexcept {
        if ( !is_valid() || !derived.is_valid() ) {
            return false;
        }

        for ( const class_type& base_class : derived.data_->base_classes ) {
            if ( base_class == *this ) {
                return true;
            }
        }

        return false;
    }

    template < class_kind Base >
    bool class_type::is_derived_from() const {
        return is_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_derived_from(const class_type& base) const noexcept {
        return base.is_base_of(*this);
    }

    template < class_kind Base >
    bool class_type::is_direct_derived_from() const {
        return is_direct_derived_from(resolve_type<Base>());
    }

    inline bool class_type::is_direct_derived_from(const class_type& base) const noexcept {
        return base.is_direct_base_of(*this);
    }

    inline function class_type::get_function(std::string_view name, bool recursively) const noexcept {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const function& function = iter->get_function(name, recursively) ) {
                    return function;
                }
            }
        }

        return function{};
    }

    inline member class_type::get_member(std::string_view name, bool recursively) const noexcept {
        for ( const member& member : data_->members ) {
            if ( member.get_name() == name ) {
                return member;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const member& member = iter->get_member(name, recursively) ) {
                    return member;
                }
            }
        }

        return member{};
    }

    inline method class_type::get_method(std::string_view name, bool recursively) const noexcept {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() == name ) {
                return method;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const method& method = iter->get_method(name, recursively) ) {
                    return method;
                }
            }
        }

        return method{};
    }

    inline any_type class_type::get_typedef(std::string_view name, bool recursively) const noexcept {
        if ( auto iter{data_->typedefs.find(name)}; iter != data_->typedefs.end() ) {
            return iter->second;
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const any_type& type = iter->get_typedef(name, recursively) ) {
                    return type;
                }
            }
        }

        return any_type{};
    }

    inline variable class_type::get_variable(std::string_view name, bool recursively) const noexcept {
        for ( const variable& variable : data_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const variable& variable = iter->get_variable(name, recursively) ) {
                    return variable;
                }
            }
        }

        return variable{};
    }

    //
    // get_constructor_with
    //

    template < typename... Args >
    constructor class_type::get_constructor_with() const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_constructor_with({registry.resolve_by_type<Args>()...});
    }

    template < typename Iter >
    constructor class_type::get_constructor_with(Iter first, Iter last) const {
        for ( const constructor& constructor : data_->constructors ) {
            const constructor_type& constructor_type = constructor.get_type();
            const any_type_list& constructor_args = constructor_type.get_argument_types();

            if ( std::equal(first, last, constructor_args.begin(), constructor_args.end()) ) {
                return constructor;
            }
        }
        return constructor{};
    }

    inline constructor class_type::get_constructor_with(std::span<const any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    inline constructor class_type::get_constructor_with(std::initializer_list<any_type> args) const noexcept {
        return get_constructor_with(args.begin(), args.end());
    }

    //
    // get_destructor
    //

    inline destructor class_type::get_destructor() const noexcept {
        if ( data_->destructors.empty() ) {
            return destructor{};
        }
        return *data_->destructors.begin();
    }

    //
    // get_function_with
    //

    template < typename... Args >
    function class_type::get_function_with( //
        std::string_view name,
        bool recursively
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_by_type<Args>()...}, recursively);
    }

    template < typename Iter >
    function class_type::get_function_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const {
        for ( const function& function : data_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const function_type& function_type = function.get_type();
            const any_type_list& function_args = function_type.get_argument_types();

            if ( std::equal(first, last, function_args.begin(), function_args.end()) ) {
                return function;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const function& function = iter->get_function_with(name, first, last, recursively) ) {
                    return function;
                }
            }
        }

        return function{};
    }

    inline function class_type::get_function_with( //
        std::string_view name,
        std::span<const any_type> args,
        bool recursively
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end(), recursively);
    }

    inline function class_type::get_function_with( //
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end(), recursively);
    }

    //
    // get_method_with
    //

    template < typename... Args >
    method class_type::get_method_with( //
        std::string_view name,
        bool recursively
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_method_with(name, {registry.resolve_by_type<Args>()...}, recursively);
    }

    template < typename Iter >
    method class_type::get_method_with( //
        std::string_view name,
        Iter first,
        Iter last,
        bool recursively
    ) const {
        for ( const method& method : data_->methods ) {
            if ( method.get_name() != name ) {
                continue;
            }

            const method_type& method_type = method.get_type();
            const any_type_list& method_args = method_type.get_argument_types();

            if ( std::equal(first, last, method_args.begin(), method_args.end()) ) {
                return method;
            }
        }

        if ( recursively ) {
            for ( auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter ) {
                if ( const method& method = iter->get_method_with(name, first, last, recursively) ) {
                    return method;
                }
            }
        }

        return method{};
    }

    inline method class_type::get_method_with( //
        std::string_view name,
        std::span<const any_type> args,
        bool recursively
    ) const noexcept {
        return get_method_with(name, args.begin(), args.end(), recursively);
    }

    inline method class_type::get_method_with( //
        std::string_view name,
        std::initializer_list<any_type> args,
        bool recursively
    ) const noexcept {
        return get_method_with(name, args.begin(), args.end(), recursively);
    }
}

namespace meta_hpp::detail
{
    inline scope_state::scope_state(scope_index nindex, metadata_map nmetadata)
    : index{std::move(nindex)}
    , metadata{std::move(nmetadata)} {}

    inline scope_state::state_ptr scope_state::make(std::string name, metadata_map metadata) {
        scope_state state{
            scope_index{std::move(name)},
            std::move(metadata),
        };
        return std::make_shared<scope_state>(std::move(state));
    }
}

namespace meta_hpp
{
    inline const std::string& scope::get_name() const noexcept {
        return state_->index.get_name();
    }

    inline const function_list& scope::get_functions() const noexcept {
        return state_->functions;
    }

    inline const typedef_map& scope::get_typedefs() const noexcept {
        return state_->typedefs;
    }

    inline const variable_list& scope::get_variables() const noexcept {
        return state_->variables;
    }

    inline function scope::get_function(std::string_view name) const noexcept {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() == name ) {
                return function;
            }
        }
        return function{};
    }

    inline any_type scope::get_typedef(std::string_view name) const noexcept {
        if ( auto iter{state_->typedefs.find(name)}; iter != state_->typedefs.end() ) {
            return iter->second;
        }
        return any_type{};
    }

    inline variable scope::get_variable(std::string_view name) const noexcept {
        for ( const variable& variable : state_->variables ) {
            if ( variable.get_name() == name ) {
                return variable;
            }
        }
        return variable{};
    }

    template < typename... Args >
    function scope::get_function_with( //
        std::string_view name
    ) const {
        detail::type_registry& registry{detail::type_registry::instance()};
        return get_function_with(name, {registry.resolve_by_type<Args>()...});
    }

    template < typename Iter >
    function scope::get_function_with( //
        std::string_view name,
        Iter first,
        Iter last
    ) const {
        for ( const function& function : state_->functions ) {
            if ( function.get_name() != name ) {
                continue;
            }

            const function_type& function_type = function.get_type();
            const any_type_list& function_args = function_type.get_argument_types();

            if ( std::equal(first, last, function_args.begin(), function_args.end()) ) {
                return function;
            }
        }
        return function{};
    }

    inline function scope::get_function_with( //
        std::string_view name,
        std::span<const any_type> args
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }

    inline function scope::get_function_with( //
        std::string_view name,
        std::initializer_list<any_type> args
    ) const noexcept {
        return get_function_with(name, args.begin(), args.end());
    }
}

namespace meta_hpp
{
    template < type_family Type >
    any_type::any_type(const Type& other) noexcept
    : any_type{detail::type_access(other)} {}

    template < type_family Type >
    bool any_type::is() const noexcept {
        if constexpr ( std::is_same_v<Type, any_type> ) {
            return data_ != nullptr;
        } else {
            return is(detail::type_traits<Type>::kind);
        }
    }

    [[nodiscard]] inline bool any_type::is(type_kind kind) const noexcept {
        return data_ != nullptr && data_->kind == kind;
    }

    template < type_family Type >
    Type any_type::as() const noexcept {
        if constexpr ( std::is_same_v<Type, any_type> ) {
            return *this;
        } else {
            using as_data_ptr = typename detail::type_traits<Type>::data_ptr;
            return is<Type>() ? Type{static_cast<as_data_ptr>(data_)} : Type{};
        }
    }

    template < typename F >
    bool any_type::match(F&& f) const {
        if ( !is_valid() ) {
            return false;
        }

        switch ( get_kind() ) {
        case type_kind::array_:
            std::invoke(std::forward<F>(f), as_array());
            return true;
        case type_kind::class_:
            std::invoke(std::forward<F>(f), as_class());
            return true;
        case type_kind::constructor_:
            std::invoke(std::forward<F>(f), as_constructor());
            return true;
        case type_kind::destructor_:
            std::invoke(std::forward<F>(f), as_destructor());
            return true;
        case type_kind::enum_:
            std::invoke(std::forward<F>(f), as_enum());
            return true;
        case type_kind::function_:
            std::invoke(std::forward<F>(f), as_function());
            return true;
        case type_kind::member_:
            std::invoke(std::forward<F>(f), as_member());
            return true;
        case type_kind::method_:
            std::invoke(std::forward<F>(f), as_method());
            return true;
        case type_kind::nullptr_:
            std::invoke(std::forward<F>(f), as_nullptr());
            return true;
        case type_kind::number_:
            std::invoke(std::forward<F>(f), as_number());
            return true;
        case type_kind::pointer_:
            std::invoke(std::forward<F>(f), as_pointer());
            return true;
        case type_kind::reference_:
            std::invoke(std::forward<F>(f), as_reference());
            return true;
        case type_kind::void_:
            std::invoke(std::forward<F>(f), as_void());
            return true;
        }

        META_HPP_ASSERT(false);
        return false;
    }

    inline bool any_type::is_array() const noexcept {
        return is<array_type>();
    }

    inline bool any_type::is_class() const noexcept {
        return is<class_type>();
    }

    inline bool any_type::is_constructor() const noexcept {
        return is<constructor_type>();
    }

    inline bool any_type::is_destructor() const noexcept {
        return is<destructor_type>();
    }

    inline bool any_type::is_enum() const noexcept {
        return is<enum_type>();
    }

    inline bool any_type::is_function() const noexcept {
        return is<function_type>();
    }

    inline bool any_type::is_member() const noexcept {
        return is<member_type>();
    }

    inline bool any_type::is_method() const noexcept {
        return is<method_type>();
    }

    inline bool any_type::is_nullptr() const noexcept {
        return is<nullptr_type>();
    }

    inline bool any_type::is_number() const noexcept {
        return is<number_type>();
    }

    inline bool any_type::is_pointer() const noexcept {
        return is<pointer_type>();
    }

    inline bool any_type::is_reference() const noexcept {
        return is<reference_type>();
    }

    inline bool any_type::is_void() const noexcept {
        return is<void_type>();
    }

    inline array_type any_type::as_array() const noexcept {
        return as<array_type>();
    }

    inline class_type any_type::as_class() const noexcept {
        return as<class_type>();
    }

    inline constructor_type any_type::as_constructor() const noexcept {
        return as<constructor_type>();
    }

    inline destructor_type any_type::as_destructor() const noexcept {
        return as<destructor_type>();
    }

    inline enum_type any_type::as_enum() const noexcept {
        return as<enum_type>();
    }

    inline function_type any_type::as_function() const noexcept {
        return as<function_type>();
    }

    inline member_type any_type::as_member() const noexcept {
        return as<member_type>();
    }

    inline method_type any_type::as_method() const noexcept {
        return as<method_type>();
    }

    inline nullptr_type any_type::as_nullptr() const noexcept {
        return as<nullptr_type>();
    }

    inline number_type any_type::as_number() const noexcept {
        return as<number_type>();
    }

    inline pointer_type any_type::as_pointer() const noexcept {
        return as<pointer_type>();
    }

    inline reference_type any_type::as_reference() const noexcept {
        return as<reference_type>();
    }

    inline void_type any_type::as_void() const noexcept {
        return as<void_type>();
    }
}

namespace meta_hpp::detail
{
    template < array_kind Array >
    array_type_data::array_type_data(array_traits<Array>)
    : type_data_base{type_kind::array_, shared_traits_hash<array_traits<Array>>{}(this)}
    , flags{array_traits<Array>::make_flags()}
    , extent{array_traits<Array>::extent}
    , data_type{resolve_type<typename array_traits<Array>::data_type>()} {}
}

namespace meta_hpp
{
    inline array_bitflags array_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t array_type::get_extent() const noexcept {
        return data_->extent;
    }

    inline any_type array_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < nullptr_kind Nullptr >
    nullptr_type_data::nullptr_type_data(nullptr_traits<Nullptr>)
    : type_data_base{type_kind::nullptr_, shared_traits_hash<nullptr_traits<Nullptr>>{}(this)} {}
}

namespace meta_hpp::detail
{
    template < number_kind Number >
    number_type_data::number_type_data(number_traits<Number>)
    : type_data_base{type_kind::number_, shared_traits_hash<number_traits<Number>>{}(this)}
    , flags{number_traits<Number>::make_flags()}
    , size{number_traits<Number>::size}
    , align{number_traits<Number>::align} {}
}

namespace meta_hpp
{
    inline number_bitflags number_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline std::size_t number_type::get_size() const noexcept {
        return data_->size;
    }

    inline std::size_t number_type::get_align() const noexcept {
        return data_->align;
    }
}

namespace meta_hpp::detail
{
    template < reference_kind Reference >
    reference_type_data::reference_type_data(reference_traits<Reference>)
    : type_data_base{type_kind::reference_, shared_traits_hash<reference_traits<Reference>>{}(this)}
    , flags{reference_traits<Reference>::make_flags()}
    , data_type{resolve_type<typename reference_traits<Reference>::data_type>()} {}
}

namespace meta_hpp
{
    inline reference_bitflags reference_type::get_flags() const noexcept {
        return data_->flags;
    }

    inline any_type reference_type::get_data_type() const noexcept {
        return data_->data_type;
    }
}

namespace meta_hpp::detail
{
    template < void_kind Void >
    void_type_data::void_type_data(void_traits<Void>)
    : type_data_base{type_kind::void_, shared_traits_hash<void_traits<Void>>{}(this)} {}
}

namespace meta_hpp::detail
{
    template <
        typename To,
        typename From,
        typename ToDT = std::remove_pointer_t<To>,
        typename FromDT = std::remove_pointer_t<From> >
    concept ucast_as_pointers                                                       //
        = (std::is_pointer_v<From> && std::is_class_v<FromDT>)                      //
       &&(std::is_pointer_v<To> && (std::is_class_v<ToDT> || std::is_void_v<ToDT>)) //
       && (std::is_const_v<ToDT> >= std::is_const_v<FromDT>)                        //
       &&(std::is_volatile_v<ToDT> >= std::is_volatile_v<FromDT>);                  //

    template <
        typename To,
        typename From,
        typename ToDT = std::remove_reference_t<To>,
        typename FromDT = std::remove_reference_t<From> >
    concept ucast_as_references                                         //
        = (std::is_lvalue_reference_v<From> && std::is_class_v<FromDT>) //
        &&(std::is_lvalue_reference_v<To> && std::is_class_v<ToDT>)     //
        &&(std::is_const_v<ToDT> >= std::is_const_v<FromDT>)            //
        &&(std::is_volatile_v<ToDT> >= std::is_volatile_v<FromDT>);     //
}

namespace meta_hpp
{
    template < typename To, typename From >
        requires detail::ucast_as_pointers<To, From>
    To ucast(From from);

    template < typename To, typename From >
        requires detail::ucast_as_references<To, From>
    To ucast(From&& from);
}

namespace meta_hpp
{
    template < typename To, typename From >
        requires detail::ucast_as_pointers<To, From>
    To ucast(From from) {
        using from_cv_data_type = std::remove_pointer_t<From>;
        using from_data_type = std::remove_cv_t<from_cv_data_type>;

        using to_cv_data_type = std::remove_pointer_t<To>;
        using to_data_type = std::remove_cv_t<to_cv_data_type>;

        static_assert(
            detail::poly_info_enabled<from_data_type>,
            "The type doesn't support ucasts. "
            "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
        );

        if ( from == nullptr ) {
            return nullptr;
        }

        if constexpr ( std::is_same_v<from_data_type, to_data_type> ) {
            return from;
        } else {
            detail::type_registry& registry{detail::type_registry::instance()};
            const detail::poly_info& meta_info{detail::get_meta_poly_info(registry, *from)};

            // NOLINTNEXTLINE(*-const-cast)
            void* most_derived_object_ptr = const_cast<void*>(meta_info.ptr);

            if constexpr ( std::is_void_v<to_data_type> ) {
                return most_derived_object_ptr;
            } else {
                const class_type& to_class_type = registry.resolve_by_type<to_data_type>();
                return static_cast<To>(detail::pointer_upcast(most_derived_object_ptr, meta_info.type, to_class_type));
            }
        }
    }

    template < typename To, typename From >
        requires detail::ucast_as_references<To, From>
    // NOLINTNEXTLINE(*-missing-std-forward)
    To ucast(From&& from) {
        using from_cv_data_type = std::remove_reference_t<From>;
        using to_cv_data_type = std::remove_reference_t<To>;

        static_assert(
            detail::poly_info_enabled<from_cv_data_type>,
            "The type doesn't support ucasts. "
            "Use the META_HPP_ENABLE_POLY_INFO macro to fix it."
        );

        if ( to_cv_data_type* ptr = ucast<to_cv_data_type*>(std::addressof(from)) ) {
            return *ptr;
        }

        throw_exception(error_code::bad_cast);
    }
}

namespace meta_hpp::detail
{
    template < typename T >
    struct copy_traits;

    template < typename T >
    concept has_copy_traits //
        = requires(const T& v) { copy_traits<T>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires requires(const T& v) { uvalue{v}; }
    struct copy_traits<T> {
        uvalue operator()(const T& v) const {
            return uvalue{v};
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct deref_traits;

    template < typename T >
    concept has_deref_traits //
        = requires(const T& v) { deref_traits<T>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<T*> {
        uvalue operator()(T* v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };

    template < typename T, typename Deleter >
        requires std::is_copy_constructible_v<T>
    struct deref_traits<std::unique_ptr<T, Deleter>> {
        uvalue operator()(const std::unique_ptr<T, Deleter>& v) const {
            return v != nullptr ? uvalue{*v} : uvalue{};
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct index_traits;

    template < typename T >
    concept has_index_traits //
        = requires(const T& v, std::size_t i) { index_traits<T>{}(v, i); };
}

namespace meta_hpp::detail
{
    template < typename T >
        requires std::is_copy_constructible_v<T>
    struct index_traits<T*> {
        uvalue operator()(T* v, std::size_t i) const {
            // NOLINTNEXTLINE(*-pointer-arithmetic)
            return v != nullptr ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, std::size_t Size >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::array<T, Size>> {
        uvalue operator()(const std::array<T, Size>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Traits, typename Allocator >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::basic_string<T, Traits, Allocator>> {
        uvalue operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Traits >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::basic_string_view<T, Traits>> {
        uvalue operator()(const std::basic_string_view<T, Traits>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, std::size_t Extent >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::span<T, Extent>> {
        uvalue operator()(const std::span<T, Extent>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };

    template < typename T, typename Allocator >
        requires std::is_copy_constructible_v<T>
    struct index_traits<std::vector<T, Allocator>> {
        uvalue operator()(const std::vector<T, Allocator>& v, std::size_t i) const {
            return i < v.size() ? uvalue{v[i]} : uvalue{};
        }
    };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct unmap_traits;

    template < typename T >
    concept has_unmap_traits //
        = requires(const T& v) { unmap_traits<T>{}(v); };
}

namespace meta_hpp::detail
{
    template < typename T >
    struct unmap_traits<std::shared_ptr<T>> {
        uvalue operator()(const std::shared_ptr<T>& v) const {
            return uvalue{v.get()};
        }
    };

    template < typename T, typename D >
    struct unmap_traits<std::unique_ptr<T, D>> {
        uvalue operator()(const std::unique_ptr<T, D>& v) const {
            return uvalue{v.get()};
        }
    };

    template < typename T >
    struct unmap_traits<std::reference_wrapper<T>> {
        uvalue operator()(const std::reference_wrapper<T>& v) const {
            return uvalue{std::addressof(v.get())};
        }
    };
}

namespace meta_hpp
{
    struct uvalue::vtable_t final {
        // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
        const any_type type;

        void (*const move)(uvalue&& self, uvalue& to) noexcept;
        void (*const reset)(uvalue& self) noexcept;

        uvalue (*const copy)(const storage_u& self);
        uvalue (*const deref)(const storage_u& self);
        uvalue (*const index)(const storage_u& self, std::size_t i);
        uvalue (*const unmap)(const storage_u& self);
        // NOLINTEND(*-avoid-const-or-ref-data-members)

        template < typename T >
        inline static constexpr bool in_internal_v = //
            (sizeof(T) <= sizeof(internal_storage_t)) && (alignof(internal_storage_t) % alignof(T) == 0)
            && std::is_nothrow_destructible_v<T> && std::is_nothrow_move_constructible_v<T>;

        template < typename T >
        inline static constexpr bool in_trivial_internal_v = //
            in_internal_v<T> && std::is_trivially_copyable_v<T>;

        static std::pair<storage_e, const vtable_t*> unpack_vtag(const uvalue& self) noexcept {
            constexpr std::uintptr_t tag_mask{0b11};
            const std::uintptr_t vtag{self.storage_.vtag};
            return std::make_pair(
                static_cast<storage_e>(vtag & tag_mask),
                // NOLINTNEXTLINE(*-no-int-to-ptr, *-reinterpret-cast)
                reinterpret_cast<const vtable_t*>(vtag & ~tag_mask)
            );
        }

        template < typename T >
        static T* storage_cast(storage_u& storage) noexcept {
            if constexpr ( in_internal_v<T> ) {
                // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
                return std::launder(reinterpret_cast<T*>(storage.internal.data));
            } else {
                // NOLINTNEXTLINE(*-union-access)
                return static_cast<T*>(storage.external.ptr);
            }
        }

        template < typename T >
        static const T* storage_cast(const storage_u& storage) noexcept {
            if constexpr ( in_internal_v<T> ) {
                // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
                return std::launder(reinterpret_cast<const T*>(storage.internal.data));
            } else {
                // NOLINTNEXTLINE(*-union-access)
                return static_cast<const T*>(storage.external.ptr);
            }
        }

        template < typename T, typename... Args, typename Tp = std::decay_t<T> >
        static Tp& do_ctor(uvalue& dst, Args&&... args) {
            META_HPP_DEV_ASSERT(!dst);

            if constexpr ( in_internal_v<Tp> ) {
                std::construct_at(storage_cast<Tp>(dst.storage_), std::forward<Args>(args)...);
                dst.storage_.vtag = in_trivial_internal_v<Tp> ? detail::to_underlying(storage_e::trivial)
                                                              : detail::to_underlying(storage_e::internal);
            } else {
                // NOLINTNEXTLINE(*-union-access, *-owning-memory)
                dst.storage_.external.ptr = new Tp(std::forward<Args>(args)...);
                dst.storage_.vtag = detail::to_underlying(storage_e::external);
            }

            // NOLINTNEXTLINE(*-reinterpret-cast)
            dst.storage_.vtag |= reinterpret_cast<std::uintptr_t>(vtable_t::get<Tp>());
            return *storage_cast<Tp>(dst.storage_);
        }

        static void do_move(uvalue&& self, uvalue& to) noexcept {
            META_HPP_DEV_ASSERT(!to);

            auto&& [tag, vtable] = unpack_vtag(self);

            switch ( tag ) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                to.storage_ = self.storage_;
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->move(std::move(self), to);
                break;
            }
        }

        static void do_reset(uvalue& self) noexcept {
            auto&& [tag, vtable] = unpack_vtag(self);

            switch ( tag ) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->reset(self);
                break;
            }
        }

        static void do_swap(uvalue& l, uvalue& r) noexcept {
            if ( (&l == &r) || (!l && !r) ) {
                return;
            }

            if ( l && r ) {
                if ( unpack_vtag(l).first == storage_e::external ) {
                    uvalue o;
                    do_move(std::move(l), o);
                    do_move(std::move(r), l);
                    do_move(std::move(o), r);
                } else {
                    uvalue o;
                    do_move(std::move(r), o);
                    do_move(std::move(l), r);
                    do_move(std::move(o), l);
                }
            } else {
                if ( l ) {
                    do_move(std::move(l), r);
                } else {
                    do_move(std::move(r), l);
                }
            }
        }

        template < typename Tp >
        // NOLINTNEXTLINE(*-cognitive-complexity)
        static vtable_t* get() {
            static_assert(std::is_same_v<Tp, std::decay_t<Tp>>);

            static vtable_t table{
                .type = resolve_type<Tp>(),

                // NOLINTNEXTLINE(*-param-not-moved)
                .move{[](uvalue&& self, uvalue& to) noexcept {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        do_ctor<Tp>(to, std::move(*src));
                        do_reset(self);
                    } else {
                        // NOLINTNEXTLINE(*-union-access)
                        to.storage_.external.ptr = src;
                        std::swap(to.storage_.vtag, self.storage_.vtag);
                    }
                }},

                .reset{[](uvalue& self) noexcept {
                    META_HPP_DEV_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr ( in_internal_v<Tp> ) {
                        std::destroy_at(src);
                    } else {
                        // NOLINTNEXTLINE(*-owning-memory)
                        delete src;
                    }

                    self.storage_.vtag = 0;
                }},

                .copy{[]() {
                    if constexpr ( detail::has_copy_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::copy_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .deref{[]() {
                    if constexpr ( detail::has_deref_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::deref_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .index{[]() {
                    if constexpr ( detail::has_index_traits<Tp> ) {
                        return +[](const storage_u& self, std::size_t i) -> uvalue {
                            return detail::index_traits<Tp>{}(*storage_cast<Tp>(self), i);
                        };
                    } else {
                        return nullptr;
                    }
                }()},

                .unmap{[]() {
                    if constexpr ( detail::has_unmap_traits<Tp> ) {
                        return +[](const storage_u& self) -> uvalue {
                            return detail::unmap_traits<Tp>{}(*storage_cast<Tp>(self));
                        };
                    } else {
                        return nullptr;
                    }
                }()},
            };

            return &table;
        }
    };
}

namespace meta_hpp
{
    inline uvalue::~uvalue() noexcept {
        reset();
    }

    inline uvalue::uvalue(uvalue&& other) noexcept {
        vtable_t::do_move(std::move(other), *this);
    }

    inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
        if ( this != &other ) {
            uvalue{std::move(other)}.swap(*this);
        }
        return *this;
    }

    template < typename T, typename >
    uvalue::uvalue(T&& val) {
        vtable_t::do_ctor<T>(*this, std::forward<T>(val));
    }

    template < typename T, typename >
    uvalue& uvalue::operator=(T&& val) {
        uvalue{std::forward<T>(val)}.swap(*this);
        return *this;
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, Args&&... args) {
        vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    uvalue::uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...>
    std::decay_t<T>& uvalue::emplace(Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...>
    std::decay_t<T>& uvalue::emplace(std::initializer_list<U> ilist, Args&&... args) {
        vtable_t::do_reset(*this);
        return vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
    }

    inline bool uvalue::has_value() const noexcept {
        return storage_.vtag != 0;
    }

    inline uvalue::operator bool() const noexcept {
        return has_value();
    }

    inline void uvalue::reset() noexcept {
        vtable_t::do_reset(*this);
    }

    inline void uvalue::swap(uvalue& other) noexcept {
        vtable_t::do_swap(*this, other);
    }

    inline any_type uvalue::get_type() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag == storage_e::nothing ? any_type{} : vtable->type;
    }

    inline void* uvalue::get_data() noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline const void* uvalue::get_data() const noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline const void* uvalue::get_cdata() const noexcept {
        switch ( vtable_t::unpack_vtag(*this).first ) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
        }

        META_HPP_ASSERT(false);
        return nullptr;
    }

    inline uvalue uvalue::operator*() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->deref != nullptr //
                 ? vtable->deref(storage_)
                 : uvalue{};
    }

    inline bool uvalue::has_deref_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->deref != nullptr;
    }

    inline uvalue uvalue::operator[](std::size_t index) const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->index != nullptr //
                 ? vtable->index(storage_, index)
                 : uvalue{};
    }

    inline bool uvalue::has_index_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->index != nullptr;
    }

    inline uvalue uvalue::copy() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->copy != nullptr //
                 ? vtable->copy(storage_)
                 : uvalue{};
    }

    inline bool uvalue::has_copy_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->copy != nullptr;
    }

    inline uvalue uvalue::unmap() const {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->unmap != nullptr //
                 ? vtable->unmap(storage_)
                 : uvalue{};
    }

    inline bool uvalue::has_unmap_op() const noexcept {
        auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
        return tag != storage_e::nothing && vtable->unmap != nullptr;
    }

    template < typename T >
    bool uvalue::is() const noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);
        return detail::is_a(resolve_type<T>(), get_type());
    }

    template < any_pointer_family T >
    T uvalue::as() {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T ptr = try_as<T>(); ptr || get_type().is_nullptr() ) {
            return ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < any_pointer_family T >
    T uvalue::as() const {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T ptr = try_as<T>(); ptr || get_type().is_nullptr() ) {
            return ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    T& uvalue::as() & {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T* ptr = try_as<T>() ) {
            return *ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    const T& uvalue::as() const& {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( const T* ptr = try_as<T>() ) {
            return *ptr;
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    T uvalue::as() && {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( T* ptr = try_as<T>() ) {
            return std::move(*ptr);
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < not_any_pointer_family T >
    const T&& uvalue::as() const&& {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        if ( const T* ptr = try_as<T>() ) {
            return std::move(*ptr);
        }

        throw_exception(error_code::bad_uvalue_access);
    }

    template < any_pointer_family T >
    T uvalue::try_as() noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T>(registry) ) {
            return varg.cast<T>(registry);
        }

        return nullptr;
    }

    template < any_pointer_family T >
    T uvalue::try_as() const noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T>(registry) ) {
            return varg.cast<T>(registry);
        }

        return nullptr;
    }

    template < not_any_pointer_family T >
    T* uvalue::try_as() & noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<T&>(registry) ) {
            return std::addressof(varg.cast<T&>(registry));
        }

        return nullptr;
    }

    template < not_any_pointer_family T >
    const T* uvalue::try_as() const& noexcept {
        static_assert(std::is_same_v<T, std::decay_t<T>>);

        using namespace detail;
        type_registry& registry{type_registry::instance()};

        if ( const uarg varg{registry, *this}; varg.can_cast_to<const T&>(registry) ) {
            return std::addressof(varg.cast<const T&>(registry));
        }

        return nullptr;
    }
}

namespace meta_hpp
{
    inline uerror::uerror(error_code error) noexcept
    : error_{error} {}

    inline uerror& uerror::operator=(error_code error) noexcept {
        error_ = error;
        return *this;
    }

    inline bool uerror::has_error() const noexcept {
        return error_ != error_code::no_error;
    }

    inline uerror::operator bool() const noexcept {
        return has_error();
    }

    inline error_code uerror::operator*() const noexcept {
        return error_;
    }

    inline error_code uerror::get_error() const noexcept {
        return error_;
    }

    inline void uerror::reset() noexcept {
        error_ = error_code::no_error;
    }

    inline void uerror::swap(uerror& other) noexcept {
        using std::swap;
        swap(error_, other.error_);
    }

    inline std::size_t uerror::get_hash() const noexcept {
        return detail::hash_composer{} << error_;
    }
}

namespace meta_hpp
{
    inline uresult::uresult(uerror error) noexcept
    : error_{error.get_error()} {}

    inline uresult::uresult(uvalue value) noexcept
    : value_{std::move(value)} {}

    inline uresult& uresult::operator=(uerror error) noexcept {
        value_ = uvalue{};
        error_ = error.get_error();
        return *this;
    }

    inline uresult& uresult::operator=(uvalue value) noexcept {
        value_ = std::move(value);
        error_ = error_code::no_error;
        return *this;
    }

    template < typename T, typename >
    uresult::uresult(T&& val)
    : value_{std::forward<T>(val)} {}

    template < typename T, typename >
    uresult& uresult::operator=(T&& val) {
        value_ = std::forward<T>(val);
        error_ = error_code::no_error;
        return *this;
    }

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...> //
    uresult::uresult(std::in_place_type_t<T>, Args&&... args)
    : value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...> //
    uresult::uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args)
    : value_{std::in_place_type<T>, ilist, std::forward<Args>(args)...} {}

    template < typename T, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, Args...> //
    std::decay_t<T>& uresult::emplace(Args&&... args) {
        std::decay_t<T>& val{value_.emplace<std::decay_t<T>>(std::forward<Args>(args)...)};
        error_ = error_code::no_error;
        return val;
    }

    template < typename T, typename U, typename... Args >
        requires std::is_constructible_v<std::decay_t<T>, std::initializer_list<U>&, Args...> //
    std::decay_t<T>& uresult::emplace(std::initializer_list<U> ilist, Args&&... args) {
        std::decay_t<T>& val{value_.emplace<std::decay_t<T>>(ilist, std::forward<Args>(args)...)};
        error_ = error_code::no_error;
        return val;
    }

    inline bool uresult::has_error() const noexcept {
        return error_ != error_code::no_error;
    }

    inline bool uresult::has_value() const noexcept {
        return error_ == error_code::no_error;
    }

    inline uresult::operator bool() const noexcept {
        return has_value();
    }

    inline uvalue* uresult::operator->() noexcept {
        return std::addressof(value_);
    }

    inline const uvalue* uresult::operator->() const noexcept {
        return std::addressof(value_);
    }

    inline uvalue& uresult::operator*() & noexcept {
        return value_;
    }

    inline uvalue&& uresult::operator*() && noexcept {
        return std::move(value_);
    }

    inline const uvalue& uresult::operator*() const& noexcept {
        return value_;
    }

    inline const uvalue&& uresult::operator*() const&& noexcept {
        // NOLINTNEXTLINE(*-move-const-arg)
        return std::move(value_);
    }

    inline uvalue& uresult::get_value() & noexcept {
        return value_;
    }

    inline uvalue&& uresult::get_value() && noexcept {
        return std::move(value_);
    }

    inline const uvalue& uresult::get_value() const& noexcept {
        return value_;
    }

    inline const uvalue&& uresult::get_value() const&& noexcept {
        // NOLINTNEXTLINE(*-move-const-arg)
        return std::move(value_);
    }

    inline error_code uresult::get_error() const noexcept {
        return error_;
    }

    inline void uresult::reset() noexcept {
        value_ = uvalue{};
        error_ = error_code::no_error;
    }

    inline void uresult::swap(uresult& other) noexcept {
        using std::swap;
        swap(value_, other.value_);
        swap(error_, other.error_);
    }
}
