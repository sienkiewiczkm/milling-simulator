#pragma once

namespace fw
{

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
class INewtonIterable
{
public:
    virtual ~INewtonIterable() = default;

    virtual void setInitialParameters(const TDomain& parameters) = 0;
    virtual void setCurrentParameters(const TDomain& parameters) = 0;
    virtual bool hasParameterConverged(const TDomain& newParameters) const = 0;
    virtual bool areParametersValid(const TDomain& parameters) const = 0;
    virtual TCodomain evaluateFunction() const = 0;
    virtual TDomainTransformation getJacobianInverse() const = 0;
    virtual TDomain correctParametrisation(const TDomain& parameters) const = 0;
};

enum class NewtonIterationExitStatus
{
    Unknown,
    Success,
    IterationLimitReached,
    InvalidParameterReached,
};

template <typename TDomain>
struct NewtonIterationResult
{
public:
    NewtonIterationResult();
    NewtonIterationResult(
        NewtonIterationExitStatus exitStatus,
        TDomain parameters
    );

    NewtonIterationExitStatus exitStatus;
    TDomain parameters;
};

template <typename TDomain>
NewtonIterationResult<TDomain>::NewtonIterationResult():
    exitStatus{NewtonIterationExitStatus::Unknown},
    parameters{}
{
}

template <typename TDomain>
NewtonIterationResult<TDomain>::NewtonIterationResult(
    NewtonIterationExitStatus exitStatus,
    TDomain parameters
):
    exitStatus{exitStatus},
    parameters{parameters}
{
}

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
class NewtonIterator
{
public:
    NewtonIterator();
    virtual ~NewtonIterator() = default;

    using IMatchingIterable =
        INewtonIterable<TDomain, TCodomain, TDomainTransformation>;

    int getIterationLimit() const;
    void setIterationLimit(int iterationLimit);

    NewtonIterationResult<TDomain> iterate(
        IMatchingIterable& iterable,
        TDomain startParameters
    ) const;

private:
    int _iterationLimit;
};

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
NewtonIterator<TDomain, TCodomain, TDomainTransformation>::NewtonIterator():
    _iterationLimit{500}
{
}

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
int NewtonIterator<TDomain, TCodomain, TDomainTransformation>::
        getIterationLimit() const
{
    return _iterationLimit;
}

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
void NewtonIterator<TDomain, TCodomain, TDomainTransformation>::
        setIterationLimit(int iterationLimit)
{
    _iterationLimit = iterationLimit;
}

template <typename TDomain, typename TCodomain, typename TDomainTransformation>
NewtonIterationResult<TDomain>
        NewtonIterator<TDomain, TCodomain, TDomainTransformation>::iterate(
    IMatchingIterable& iterable,
    TDomain startParameters
) const
{
    auto iteration = 0;
    auto currentParameters = startParameters;

    iterable.setInitialParameters(currentParameters);

    bool lastParameterValidity = true;

    do
    {
        iterable.setCurrentParameters(currentParameters);
        auto functionValue = iterable.evaluateFunction();
        auto jacobianInv = iterable.getJacobianInverse();
        currentParameters -= jacobianInv * functionValue;
        currentParameters = iterable.correctParametrisation(currentParameters);
    }
    while (
        (lastParameterValidity = iterable.areParametersValid(currentParameters))
        && !iterable.hasParameterConverged(currentParameters)
        && (++iteration) < _iterationLimit
    );

    NewtonIterationExitStatus exitStatus{NewtonIterationExitStatus::Success};

    if (iteration >= _iterationLimit)
    {
        exitStatus = NewtonIterationExitStatus::IterationLimitReached;
    }
    else if (!lastParameterValidity)
    {
        exitStatus = NewtonIterationExitStatus::InvalidParameterReached;
    }

    return {exitStatus, currentParameters};
}

}
