/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_NUMERIC_INNER_PRODUCT_HPP
#define RANGES_V3_NUMERIC_INNER_PRODUCT_HPP

#include <meta/meta.hpp>

#include <range/v3/functional/arithmetic.hpp>
#include <range/v3/functional/concepts.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/concepts.hpp>
#include <range/v3/iterator/traits.hpp>
#include <range/v3/iterator/unreachable_sentinel.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    /// \addtogroup group-numerics
    /// @{
    // clang-format off
    CPP_def
    (
        template(typename I1, typename I2, typename T, typename BOp1 = plus,
            typename BOp2 = multiplies, typename P1 = identity, typename P2 = identity)
        (concept inner_product_constraints)(I1, I2, T, BOp1, BOp2, P1, P2),
            input_iterator<I1> &&
            input_iterator<I2> &&
            invocable<P1&, iter_value_t<I1>> &&
            invocable<P2&, iter_value_t<I2>> &&
            invocable<
                BOp2&,
                invoke_result_t<P1&, iter_value_t<I1>>,
                invoke_result_t<P2&, iter_value_t<I2>>> &&
            invocable<
                BOp1&,
                T,
                invoke_result_t<
                    BOp2&,
                    invoke_result_t<P1&, iter_value_t<I1>>,
                    invoke_result_t<P2&, iter_value_t<I2>>>> &&
            assignable_from<
                T&,
                invoke_result_t<
                    BOp1&,
                    T,
                    invoke_result_t<
                        BOp2&,
                        invoke_result_t<P1&, iter_value_t<I1>>,
                        invoke_result_t<P2&, iter_value_t<I2>>>>>
    );
    // clang-format on

    struct inner_product_fn
    {
        template<typename I1, typename S1, typename I2, typename S2, typename T,
                 typename BOp1 = plus, typename BOp2 = multiplies, typename P1 = identity,
                 typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, S2 end2, T init,
                        BOp1 bop1 = BOp1{}, BOp2 bop2 = BOp2{}, P1 proj1 = P1{},
                        P2 proj2 = P2{}) const -> CPP_ret(T)( //
            requires sentinel_for<S1, I1> && sentinel_for<S2, I2> &&
                inner_product_constraints<I1, I2, T, BOp1, BOp2, P1, P2>)
        {
            for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                init =
                    invoke(bop1,
                           init,
                           invoke(bop2, invoke(proj1, *begin1), invoke(proj2, *begin2)));
            return init;
        }

        template<typename I1, typename S1, typename I2, typename T, typename BOp1 = plus,
                 typename BOp2 = multiplies, typename P1 = identity,
                 typename P2 = identity>
        auto operator()(I1 begin1, S1 end1, I2 begin2, T init, BOp1 bop1 = BOp1{},
                        BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            -> CPP_ret(T)( //
                requires sentinel_for<S1, I1> &&
                    inner_product_constraints<I1, I2, T, BOp1, BOp2, P1, P2>)
        {
            return (*this)(std::move(begin1),
                           std::move(end1),
                           std::move(begin2),
                           unreachable,
                           std::move(init),
                           std::move(bop1),
                           std::move(bop2),
                           std::move(proj1),
                           std::move(proj2));
        }

        template<typename Rng1, typename I2Ref, typename T, typename BOp1 = plus,
                 typename BOp2 = multiplies, typename P1 = identity,
                 typename P2 = identity, typename I1 = iterator_t<Rng1>,
                 typename I2 = uncvref_t<I2Ref>>
        auto operator()(Rng1 && rng1, I2Ref && begin2, T init, BOp1 bop1 = BOp1{},
                        BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            -> CPP_ret(T)( //
                requires range<Rng1> &&
                    inner_product_constraints<I1, I2, T, BOp1, BOp2, P1, P2>)
        {
            return (*this)(begin(rng1),
                           end(rng1),
                           static_cast<I2Ref &&>(begin2),
                           std::move(init),
                           std::move(bop1),
                           std::move(bop2),
                           std::move(proj1),
                           std::move(proj2));
        }

        template<typename Rng1, typename Rng2, typename T, typename BOp1 = plus,
                 typename BOp2 = multiplies, typename P1 = identity,
                 typename P2 = identity, typename I1 = iterator_t<Rng1>,
                 typename I2 = iterator_t<Rng2>>
        auto operator()(Rng1 && rng1, Rng2 && rng2, T init, BOp1 bop1 = BOp1{},
                        BOp2 bop2 = BOp2{}, P1 proj1 = P1{}, P2 proj2 = P2{}) const
            -> CPP_ret(T)( //
                requires range<Rng1> && range<Rng2> &&
                    inner_product_constraints<I1, I2, T, BOp1, BOp2, P1, P2>)
        {
            return (*this)(begin(rng1),
                           end(rng1),
                           begin(rng2),
                           end(rng2),
                           std::move(init),
                           std::move(bop1),
                           std::move(bop2),
                           std::move(proj1),
                           std::move(proj2));
        }
    };

    RANGES_INLINE_VARIABLE(inner_product_fn, inner_product)
    /// @}
} // namespace ranges

#endif
