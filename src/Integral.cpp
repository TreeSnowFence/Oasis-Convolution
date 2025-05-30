//
// Created by Levy Lin on 2/09/2024.
//

#include "Oasis/Integral.hpp"

#include "Oasis/Add.hpp"
// #include "Oasis/Log.hpp"
// #include "Oasis/Imaginary.hpp"

namespace Oasis {

Integral<Expression>::Integral(const Expression& integrand, const Expression& differential)
    : BinaryExpression(integrand, differential)
{
}
/**
 * Returns a simplified integral
 */
auto Integral<Expression>::Simplify() const -> std::unique_ptr<Expression>
{

    auto simplifiedIntegrand = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedDifferential = leastSigOp ? leastSigOp->Simplify() : nullptr;

    return simplifiedIntegrand->Integrate(*simplifiedDifferential);
}

/**
 * Returns a simplified integral
 */
auto Integral<Expression>::Simplify(const Expression& upper, const Expression& lower) const -> std::unique_ptr<Expression>
{
    // Returns simplified Integral

    auto simplifiedIntegrand = mostSigOp ? mostSigOp->Simplify() : nullptr;
    auto simplifiedDifferential = leastSigOp ? leastSigOp->Simplify() : nullptr;

    return simplifiedIntegrand->IntegrateWithBounds(*simplifiedDifferential, upper, lower);
    /*
        Integral simplifiedIntegrate { *simplifiedIntegrand, *simplifiedDifferential };

        // Bounded Integration Rules

        // Constant Rule
        if (auto constCase = RecursiveCast<Integral<Real, Variable>>(simplifiedIntegrate); constCase != nullptr) {
            const Real& constant = constCase->GetMostSigOp();

            auto upper_bound = RecursiveCast<Real>(upper);
            auto lower_bound = RecursiveCast<Real>(lower);

            if (upper_bound != nullptr && lower_bound != nullptr) {
                return std::make_unique<Real>((constant.GetValue() * upper_bound->GetValue()) - (constant.GetValue() * lower_bound->GetValue()));
            } else if (upper_bound != nullptr && lower_bound == nullptr) {
                return std::make_unique<Subtract<Real, Expression>>(Real { constant.GetValue() * upper_bound->GetValue() }, *Multiply<Expression> { constant, lower }.Simplify());
            } else if (upper_bound == nullptr && lower_bound != nullptr) {
                return std::make_unique<Subtract<Expression, Real>>(*Multiply<Expression> { constant, upper }.Simplify(), Real { constant.GetValue() * lower_bound->GetValue() });
            }
            return std::make_unique<Subtract<Expression>>(*Multiply<Expression> { constant, upper }.Simplify(), *Multiply<Expression> { constant, lower }.Simplify());
        }

        if (auto constCase = RecursiveCast<Integral<Divide<Real>, Variable>>(simplifiedIntegrate); constCase != nullptr) {
            Oasis::Divide constant { constCase->GetMostSigOp() };
            auto constReal = RecursiveCast<Real>(*constant.Simplify());

            auto upper_bound = RecursiveCast<Real>(upper);
            auto lower_bound = RecursiveCast<Real>(lower);

            if (upper_bound != nullptr && lower_bound != nullptr) {
                return std::make_unique<Real>((Real { *constReal }.GetValue() * upper_bound->GetValue()) - (Real { *constReal }.GetValue() * lower_bound->GetValue()));
            } else if (upper_bound != nullptr && lower_bound == nullptr) {
                return std::make_unique<Subtract<Real, Expression>>(Real { Real { *constReal }.GetValue() * upper_bound->GetValue() }, *Multiply<Expression> { *(constReal), lower }.Simplify());
            } else if (upper_bound == nullptr && lower_bound != nullptr) {
                return std::make_unique<Subtract<Expression, Real>>(*Multiply<Expression> { *(constReal), upper }.Simplify(), Real { Real { *constReal }.GetValue() * lower_bound->GetValue() });
            }
            return std::make_unique<Subtract<Expression>>(*Multiply<Expression> { *(constReal), upper }.Simplify(), *Multiply<Expression> { *(constReal), lower }.Simplify());
        }

        // Power Rule
        if (auto powerCase = RecursiveCast<Integral<Exponent<Expression>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
            const Variable& differential = powerCase->GetLeastSigOp();
            Oasis::Exponent exponent { powerCase->GetMostSigOp() };
            auto integral = RecursiveCast<Add<Divide<Exponent<Variable, Real>, Real>, Variable>>(*exponent.Integrate(differential));

            auto upper_bound = RecursiveCast<Real>(upper);
            auto lower_bound = RecursiveCast<Real>(lower);

            if (upper_bound != nullptr && lower_bound != nullptr) {

                //            const Real& divisor = integral->GetMostSigOp().GetLeastSigOp();
                //            const Real& power = integral->GetMostSigOp().GetMostSigOp().GetLeastSigOp();
                //            const IExpression<Real> auto& left = Exponent<Real>{ Real{upper_bound->GetValue()}, power };
                //            const IExpression<Real> auto& right = *Exponent<Real>{ Real{lower_bound->GetValue()}, power }.Simplify();

                //            return make_unique<Real>(  *Subtract<Real>{ *Divide<Real>{ left, divisor }.Simplify(), *Divide<Real>{ right, divisor }.Simplify() }.Simplify() );
            } else if (upper_bound != nullptr && lower_bound == nullptr) {

            } else if (upper_bound == nullptr && lower_bound != nullptr) {
            }
            //        return
        }
        //
        //    // Constant + Power Rule
        //    if (auto powerCase = RecursiveCast<Integrate<Multiply<Expression, Exponent<Expression>>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
        //        const Variable& differential = powerCase->GetLeastSigOp();
        //        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetLeastSigOp()};
        //        auto integration = exponent.Integrate(differential);
        //
        //        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetMostSigOp(), (*integration));
        //    }
        //
        //    if (auto powerCase = RecursiveCast<Integrate<Multiply<Exponent<Expression>, Expression>, Variable>>(simplifiedIntegrate); powerCase != nullptr) {
        //        const Variable& differential = powerCase->GetLeastSigOp();
        //        Oasis::Exponent exponent{powerCase->GetMostSigOp().GetMostSigOp()};
        //        auto integration = exponent.Integrate(differential);
        //
        //        return std::make_unique<Multiply<Expression>>(powerCase->GetMostSigOp().GetLeastSigOp(), (*integration));
        //    }

        // U Substitution Rule
        // Needs Differentiation Implementation for U-Sub

        // Exponential Rule - a^x
        // Needs Euler's Number for Exponential Functions

        // Natural Log Rule
        // Needs Euler's Number for Exponential Functions

        // Euler's Number
        // Needs Euler's Number for Exponential Functions

        // Trigonometric Rules
        // Needs Trig for Exponential Functions

        // Powers of Sin and Cos
        // Needs Trig for Exponential Functions

        // Integration by Parts
        // Needs Trig and Euler's Number

        // Partial Fraction Decomposition
        // Work in Progress

        //    return simplifiedIntegrate.Copy();
        return Copy();
        */
}

} // Oasis
