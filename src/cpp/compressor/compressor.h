#pragma once
#include <vector>

/**
 * @file compressor.h
 * @brief Defines the interface for a stateful audio dynamics compressor.
 */

/**
 * @class Compressor
 * @brief A stateful audio compressor that applies gain reduction based on a set of parameters.
 *
 * This class holds the state (threshold, ratio, envelope, etc.) and processes
 * audio buffers passed to it.
 */
class Compressor {
public:
    /**
     * @brief Construct a new Compressor object with default parameters.
     */
    Compressor();

    /**
     * @brief Processes an audio buffer, applying compression in-place.
     * @param buffer A vector of floating-point audio samples. This buffer is modified directly.
     */
    void process_audio(std::vector<float>& buffer);

    /**
     * @brief Sets the compression threshold.
     * @param threshold The level (0.0 to 1.0) above which compression is applied.
     */
    void set_threshold(float threshold);

    /**
     * @brief Sets the compression ratio.
     * @param ratio The amount of gain reduction (e.g., 4.0 for a 4:1 ratio).
     */
    void set_ratio(float ratio);

    /**
     * @brief Sets the attack time for the compressor's envelope detector.
     * @param attack The time in seconds for the compressor to react to peaks.
     */
    void set_attack(float attack);

    /**
     * @brief Sets the release time for the compressor's envelope detector.
     * @param release The time in seconds for the compressor to return to zero gain reduction.
     */
    void set_release(float release);

private:
    // Core compression parameters
    float threshold;    ///< The threshold for applying compression.
    float ratio;        ///< The compression ratio.
    float attack_time;  ///< The attack time.
    float release_time; ///< The release time.

    // Internal state variables
    float gain;         ///< The current gain reduction factor.
    float envelope;     ///< The current value of the envelope follower.
};