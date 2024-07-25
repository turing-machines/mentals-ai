#ifndef META_POLICY_HPP
#define META_POLICY_HPP


namespace meta {


/*! @brief Empty class type used to request the _as alias_ policy. */
struct as_alias_t {};


/*! @brief Disambiguation tag. */
constexpr as_alias_t as_alias;


/*! @brief Empty class type used to request the _as-is_ policy. */
struct as_is_t {};


/*! @brief Empty class type used to request the _as void_ policy. */
struct as_void_t {};


}


#endif // META_POLICY_HPP
