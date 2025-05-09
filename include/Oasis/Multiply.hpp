/**
 * Created by Matthew McCall on 8/10/23.
 */

#ifndef OASIS_MULTIPLY_HPP
#define OASIS_MULTIPLY_HPP

#include "BinaryExpression.hpp"
#include "Real.hpp"

namespace Oasis {

template <IExpression MultiplicandT, IExpression MultiplierT>
class Multiply;

/// @cond
template <>
class Multiply<Expression, Expression> : public BinaryExpression<Multiply> {
public:
    using BinaryExpression::BinaryExpression;

    [[nodiscard]] auto Simplify() const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Differentiate(const Expression& differentiationVariable) const -> std::unique_ptr<Expression> final;

    [[nodiscard]] auto Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression> final;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};
/// @endcond

/**
 * The Multiply expression multiplies two expressions.
 *
 * @tparam MultiplicandT The expression to be multiplied by.
 * @tparam MultiplierT The expression to multiply the multiplicand by.
 */
template <IExpression MultiplicandT = Expression, IExpression MultiplierT = MultiplicandT>
class Multiply : public BinaryExpression<Multiply, MultiplicandT, MultiplierT> {
public:
    Multiply() = default;
    Multiply(const Multiply<MultiplicandT, MultiplierT>& other)
        : BinaryExpression<Multiply, MultiplicandT, MultiplierT>(other)
    {
    }

    Multiply(const MultiplicandT& addend1, const MultiplierT& addend2)
        : BinaryExpression<Multiply, MultiplicandT, MultiplierT>(addend1, addend2)
    {
    }

    auto operator=(const Multiply& other) -> Multiply& = default;

    EXPRESSION_TYPE(Multiply)
    EXPRESSION_CATEGORY(Associative | Commutative | BinExp)
};

} // Oasis

#endif // OASIS_MULTIPLY_HPP
