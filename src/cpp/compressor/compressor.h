#pragma once
#include <vector>
#include <string>
#include <deque>

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
 * @see FPSEE_ANALYSIS.md for a detailed analysis of this module against the F.P.S.E.E. framework.
 */
class Compressor {
public:
    Compressor();

#ifdef _DEBUG
    void process_audio(std::vector<float>& buffer, std::vector<DebugSample>& debug_data, float sample_rate);
#else
    void process_audio(std::vector<float>& buffer, float sample_rate);
#endif

    void set_threshold(float threshold);
    void set_ratio(float ratio);
    void set_attack(float attack_ms);
    void set_release(float release_ms);
    void set_lookahead(float lookahead_ms);

private:
    float threshold;
    float ratio;
    float attack_time;
    float release_time;
    float lookahead_time_ms;

    float gain;
    float envelope;
    std::deque<float> lookahead_buffer;
    std::deque<float> gain_buffer;      ///< Buffer to delay the gain values.
};