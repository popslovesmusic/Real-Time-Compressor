#pragma once
#include <vector>
#include <string> // Required for std::string

/**
 * @file compressor.h
 * @brief Defines the interface for a stateful audio dynamics compressor.
 */

#ifdef _DEBUG
/**
 * @struct DebugSample
 * @brief Holds the internal state of the compressor for a single sample frame.
 * @note This struct only exists in Debug builds.
 */
struct DebugSample {
    int sample_index;
    float input_value;
    float envelope_value;
    float gain_value;
    std::string state;
    float output_value;
};
#endif

/**
 * @class Compressor
 * @brief A stateful audio compressor that applies gain reduction based on a set of parameters.
 */
class Compressor {
public:
    Compressor();

    /**
     * @brief Processes an audio buffer, applying compression in-place.
     * @param buffer A vector of floating-point audio samples. This buffer is modified directly.
     */
#ifdef _DEBUG
    void process_audio(std::vector<float>& buffer, std::vector<DebugSample>& debug_data);
#else
    void process_audio(std::vector<float>& buffer);
#endif

    void set_threshold(float threshold);
    void set_ratio(float ratio);
    void set_attack(float attack);
    void set_release(float release);

private:
    float threshold;
    float ratio;
    float attack_time;
    float release_time;
    float gain;
    float envelope;
};