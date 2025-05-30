//
// Created by Matthew McCall on 8/10/23.
//

#include <map>
#include <vector>

#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Imaginary.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Log.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

namespace Oasis {

Divide<Expression>::Divide(const Expression& dividend, const Expression& divisor)
    : BinaryExpression(dividend, divisor)
{
}
/**
 *Simplifies a division expression
 */
auto Divide<Expression>::Simplify() const -> std::unique_ptr<Expression>
{
    auto simplifiedDividend = mostSigOp->Simplify(); // numerator
    auto simplifiedDivider = leastSigOp->Simplify(); // denominator
    Divide simplifiedDivide { *simplifiedDividend, *simplifiedDivider };

    if (auto realCase = RecursiveCast<Divide<Real>>(simplifiedDivide); realCase != nullptr) {
        const Real& dividend = realCase->GetMostSigOp();
        const Real& divisor = realCase->GetLeastSigOp();
        return std::make_unique<Real>(dividend.GetValue() / divisor.GetValue());
    }
    /**
     *log(a)/log(b)=log[b](a)
     */
    if (auto logCase = RecursiveCast<Divide<Log<Expression, Expression>, Log<Expression, Expression>>>(simplifiedDivide); logCase != nullptr) {
        if (logCase->GetMostSigOp().GetMostSigOp().Equals(logCase->GetLeastSigOp().GetMostSigOp())) {
            const IExpression auto& base = logCase->GetLeastSigOp().GetLeastSigOp();
            const IExpression auto& argument = logCase->GetMostSigOp().GetLeastSigOp();
            return std::make_unique<Log<Expression>>(base, argument);
        }
    }
    /**
     * convert the terms in numerator and denominator into a vector to make manipulations easier
     */
    std::vector<std::unique_ptr<Expression>> numerator;
    std::vector<std::unique_ptr<Expression>> denominator;
    std::vector<std::unique_ptr<Expression>> result;
    std::vector<std::unique_ptr<Expression>> numeratorVals;
    std::vector<std::unique_ptr<Expression>> denominatorVals;

    if (auto multipliedNumerator = RecursiveCast<Multiply<Expression>>(*simplifiedDividend); multipliedNumerator != nullptr) {
        multipliedNumerator->Flatten(numerator);
    } else {
        numerator.push_back(simplifiedDividend->Copy());
    }

    if (auto multipliedDenominator = RecursiveCast<Multiply<Expression>>(*simplifiedDivider); multipliedDenominator != nullptr) {
        multipliedDenominator->Flatten(denominator);
    } else {
        denominator.push_back(simplifiedDivider->Copy());
    }

    /**
     * now that we have the terms in a vector, we have to cancel like terms and simplify them
     */
    result.reserve(numerator.size());
    for (const auto& num : numerator) {
        result.push_back(num->Copy());
    }

    for (const auto& denom : denominator) {
        size_t i = 0;
        if (auto real = RecursiveCast<Real>(*denom); real != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = RecursiveCast<Real>(*result[i]); resI != nullptr) {
                    result[i] = Real { resI->GetValue() / real->GetValue() }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Real { 1 / real->GetValue() }.Generalize());
            }
            continue;
        }
        if (auto img = RecursiveCast<Imaginary>(*denom); img != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resI = RecursiveCast<Imaginary>(*result[i]); resI != nullptr) {
                    result[i] = Real { 1.0 }.Generalize();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { Imaginary {}, Real { -1.0 } }.Generalize());
            }
            continue;
        }
        if (auto var = RecursiveCast<Variable>(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = RecursiveCast<Exponent<Variable, Expression>>(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(*var)) {
                        result[i] = Exponent<Expression> { *var, *(Subtract<Expression> { resIexp->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Generalize();
                        break;
                    }
                } else if (auto resI = RecursiveCast<Variable>(*result[i]); resI != nullptr) {
                    if (resI->Equals(*var)) {
                        result[i] = Real { 1.0 }.Generalize();
                        break;
                    }
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { *var, Real { -1.0 } }.Generalize());
            }
            continue;
        }
        if (auto var = RecursiveCast<Exponent<Variable, Expression>>(*denom); var != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resIexp = RecursiveCast<Exponent<Variable, Expression>>(*result[i]); resIexp != nullptr) {
                    if (resIexp->GetMostSigOp().Equals(var->GetMostSigOp())) {
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(Subtract<Expression> { resIexp->GetLeastSigOp(), var->GetLeastSigOp() }.Simplify()) }.Generalize();
                        break;
                    }
                } else if (auto resI = RecursiveCast<Variable>(*result[i]); resI != nullptr) {
                    if (resI->Equals(*var)) {
                        result[i] = Exponent<Expression> { var->GetMostSigOp(), *(Subtract<Expression> { Real { 1.0 }, var->GetLeastSigOp() }.Simplify()) }.Generalize();
                    }
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { var->GetMostSigOp(), Multiply { Real { -1.0 }, var->GetLeastSigOp() } }.Generalize());
            }
            continue;
        }
        if (auto expExpr = RecursiveCast<Exponent<Expression>>(*denom); expExpr != nullptr) {
            for (; i < result.size(); i++) {
                if (auto resExpr = RecursiveCast<Exponent<Expression, Expression>>(*result[i]); resExpr != nullptr) {
                    if (expExpr->GetMostSigOp().Equals(resExpr->GetMostSigOp())) {
                        result[i] = Exponent { expExpr->GetMostSigOp(), *(Subtract { resExpr->GetLeastSigOp(), expExpr->GetLeastSigOp() }.Simplify()) }.Simplify();
                        break;
                    }
                } else if (result[i]->Equals(expExpr->GetMostSigOp())) {
                    result[i] = Exponent { expExpr->GetMostSigOp(), *(Subtract { Real { 1.0 }, resExpr->GetLeastSigOp() }.Simplify()) }.Simplify();
                    break;
                }
            }
            if (i >= result.size()) {
                result.push_back(Exponent<Expression> { expExpr->GetMostSigOp(), Multiply { Real { -1.0 }, expExpr->GetLeastSigOp() } }.Simplify());
            }
            continue;
        }
        for (; i < result.size(); i++) {
            if (auto resExpr = RecursiveCast<Exponent<Expression, Expression>>(*result[i]); resExpr != nullptr) {
                if (denom->Equals(resExpr->GetMostSigOp())) {
                    result[i] = Exponent { *denom, *(Subtract { resExpr->GetLeastSigOp(), Real { 1.0 } }.Simplify()) }.Simplify();
                    break;
                }
            } else if (result[i]->Equals(*denom)) {
                result[i] = Real { 1.0 }.Generalize();
                break;
            }
        }
        if (i >= result.size()) {
            result.push_back(Exponent<Expression> { *denom, Real { -1.0 } }.Generalize());
        }
    }

    /**
     * rebuild into tree
     */
    for (const auto& val : result) {
        if (auto valI = RecursiveCast<Real>(*val); valI != nullptr) {
            if (valI->GetValue() != 1.0) {
                numeratorVals.push_back(valI->Generalize());
            }
        } else if (auto var = RecursiveCast<Variable>(*val); var != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto img = RecursiveCast<Imaginary>(*val); img != nullptr) {
            numeratorVals.push_back(val->Generalize());
        } else if (auto expR = RecursiveCast<Exponent<Expression, Real>>(*val); expR != nullptr) {
            if (expR->GetLeastSigOp().GetValue() < 0.0) {
                denominatorVals.push_back(Exponent { expR->GetMostSigOp(), Real { expR->GetLeastSigOp().GetValue() * -1.0 } }.Generalize());
            } else {
                numeratorVals.push_back(val->Generalize());
            }
        } else if (auto exp = RecursiveCast<Exponent<Expression, Multiply<Real, Expression>>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetMostSigOp().GetValue() < 0.0) {
                denominatorVals.push_back(Exponent { exp->GetMostSigOp(), exp->GetLeastSigOp().GetLeastSigOp() }.Generalize());
            } else {
                numeratorVals.push_back(val->Generalize());
            }
        } else {
            numeratorVals.push_back(val->Generalize());
        }
    }

    /**
     * makes expr^1 into expr
     */
    for (auto& val : numeratorVals) {
        if (auto exp = RecursiveCast<Exponent<Expression, Real>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    for (auto& val : denominatorVals) {
        if (auto exp = RecursiveCast<Exponent<Expression, Real>>(*val); exp != nullptr) {
            if (exp->GetLeastSigOp().GetValue() == 1.0) {
                val = exp->GetMostSigOp().Generalize();
            }
        }
    }

    auto dividend = numeratorVals.size() == 1 ? std::move(numeratorVals.front()) : BuildFromVector<Multiply>(numeratorVals);
    auto divisor = denominatorVals.size() == 1 ? std::move(denominatorVals.front()) : BuildFromVector<Multiply>(denominatorVals);

    /**
     *rebuild subtrees
     */
    if (!dividend && divisor)
        return Divide { Real { 1.0 }, *divisor }.Copy();

    if (dividend && !divisor)
        return dividend;

    if (!dividend && !divisor)
        return Real { 1.0 }.Copy();

    return Divide { *dividend, *divisor }.Copy();
}

/**
 * Performs integration on division expression
 */
auto Divide<Expression>::Integrate(const Expression& integrationVariable) const -> std::unique_ptr<Expression>
{
    // Single integration variable
    if (auto variable = RecursiveCast<Variable>(integrationVariable); variable != nullptr) {
        auto simplifiedDiv = this->Simplify();

        // Constant case - Integrand over a divisor
        if (auto constant = RecursiveCast<Multiply<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            return constant->Integrate(integrationVariable)->Simplify();
        }
    }

    Integral<Expression, Expression> integral { *(this->Copy()), *(integrationVariable.Copy()) };

    return integral.Copy();
}
/**
 * Performs Differentiation on division expression
 */
auto Divide<Expression>::Differentiate(const Oasis::Expression& differentiationVariable) const -> std::unique_ptr<Expression>
{
    /**
     * Single differentiation variable
     */
    if (auto variable = RecursiveCast<Variable>(differentiationVariable); variable != nullptr) {
        auto simplifiedDiv = this->Simplify();

        /**
         * Constant case - differentiation over a divisor
         */
        if (auto constant = RecursiveCast<Divide<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                return std::make_unique<Divide<Expression, Real>>(Divide<Expression, Real> { *(add->Simplify()), Real { num.GetValue() } })->Simplify();
            }
        }
        /**
         * In case of simplify turning divide into mult
         */
        if (auto constant = RecursiveCast<Multiply<Expression, Real>>(*simplifiedDiv); constant != nullptr) {
            auto exp = constant->GetMostSigOp().Copy();
            auto num = constant->GetLeastSigOp();
            auto differentiate = (*exp).Differentiate(differentiationVariable);
            if (auto add = RecursiveCast<Expression>(*differentiate); add != nullptr) {
                return std::make_unique<Multiply<Expression, Real>>(Multiply<Expression, Real> { *(add->Simplify()), Real { num.GetValue() } })->Simplify();
            }
        }
        /**
         * Quotient Rule: d/dx (f(x)/g(x)) = (g(x)f'(x)-f(x)g'(x))/(g(x)^2)
         */
        if (auto quotient = RecursiveCast<Divide<Expression, Expression>>(*simplifiedDiv); quotient != nullptr) {
            auto leftexp = quotient->GetMostSigOp().Copy();
            auto rightexp = quotient->GetLeastSigOp().Copy();
            auto leftDiff = leftexp->Differentiate(differentiationVariable);
            auto rightDiff = rightexp->Differentiate(differentiationVariable);
            auto mult1 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Simplify()), *(leftDiff->Simplify()) }).Simplify()->Simplify();
            auto mult2 = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(leftexp->Simplify()), *(rightDiff->Simplify()) }).Simplify()->Simplify();
            auto numerator = Subtract<Expression, Expression>(Subtract<Expression, Expression> { *mult1, *mult2 }).Simplify();
            auto denominator = Multiply<Expression, Expression>(Multiply<Expression, Expression> { *(rightexp->Simplify()), *(rightexp->Simplify()) }).Simplify();
            return Divide<Expression, Expression>({ *(numerator->Simplify()), *(denominator->Simplify()) }).Simplify();
        }
    }

    return Copy();
}

} // Oasis