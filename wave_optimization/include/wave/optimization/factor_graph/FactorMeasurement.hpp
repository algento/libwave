/**
 * @file
 * @ingroup optimization
 */

#ifndef WAVE_OPTIMIZATION_FACTOR_GRAPH_MEASUREMENT_HPP
#define WAVE_OPTIMIZATION_FACTOR_GRAPH_MEASUREMENT_HPP

#include "wave/optimization/factor_graph/FactorVariable.hpp"
#include "wave/optimization/factor_graph/Noise.hpp"
#include "wave/optimization/factor_graph/OutputMap.hpp"

namespace wave {
/** @addtogroup optimization
 *  @{ */

/**
 * A measurement, with associated noise, associated with a Factor
 * @tparam V the type of ValueView representing the measurement's value
 * @tparam NoiseTmpl the type of noise
 */
template <template <typename...> class V, typename N = DiagonalNoise<V>>
class FactorMeasurement : public FactorVariable<V> {
    using Base = FactorVariable<V>;

 public:
    using VarType = Base;
    using ValueType = typename Base::ValueType;
    using NoiseType = N;

    /** Construct with initial value and initial noise value*/
    explicit FactorMeasurement(ValueType initial,
                               typename NoiseType::ValueType noise_value)
        : Base{std::move(initial)}, noise{std::move(noise_value)} {}

    /** Construct with initial value, only for variables of size one*/
    explicit FactorMeasurement(double initial, double noise_value)
        : Base{initial}, noise{std::move(noise_value)} {}

    /** Construct with initial value and initial noise object*/
    explicit FactorMeasurement(ValueType initial, NoiseType noise)
        : Base{std::move(initial)}, noise{std::move(noise)} {}

    /** Construct with initial value and no noise
     * Only allowed when NoiseType is void*/
    explicit FactorMeasurement(ValueType initial) : Base{std::move(initial)} {
        static_assert(std::is_void<NoiseType>::value,
                      "A noise value must be provided as the second argument");
    }

    NoiseType noise;
};


/**
 * Partially specialized FactorMeasurement with zero noise
 *
 * This specialization can be constructed with a measured value only, without
 * needing to specify noise.
 */
template <template <typename...> class V>
class FactorMeasurement<V, void> : public FactorVariable<V> {
    using Base = FactorVariable<V>;

 public:
    using VarType = Base;
    using ValueType = typename Base::ValueType;
    using NoiseType = void;
    constexpr static int Size = Base::Size;

    /** Construct with initial value and no noise */
    explicit FactorMeasurement(V<double> initial) : Base{std::move(initial)} {}

    /** Construct with initial value, only for variables of size one*/
    explicit FactorMeasurement(double initial) : Base{initial} {}
};

/** @} group optimization */
}  // namespace wave

#endif  // WAVE_OPTIMIZATION_FACTOR_GRAPH_MEASUREMENT_HPP
