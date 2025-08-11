#include "compressor.h"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> // For std::copy

/**
 * @file compressor.cpp
 * @brief Implements the Compressor class and the C-style wrapper for Python.
 */

Compressor::Compressor() {
    threshold = 0.0f;
    ratio = 1.0f;
    attack_time = 0.05f;
    release_time = 0.2f;
    gain = 1.0f;
    envelope = 0.0f;
}

// In debug mode, we use the version that populates the debug_data vector.
#ifdef _DEBUG
void Compressor::process_audio(std::vector<float>& buffer, std::vector<DebugSample>& debug_data) {
    debug_data.clear(); // Clear previous debug info
    int sample_index = 0;

    for (float& sample : buffer) {
        DebugSample current_sample_debug;
        current_sample_debug.sample_index = sample_index + 1;
        current_sample_debug.input_value = sample;
        std::string state = "Idle";

        // Step 1: Envelope Detection
        float absolute_sample = std::abs(sample);
        if (absolute_sample > envelope) {
            envelope = absolute_sample;
            if (envelope > threshold) {
                state = "Attack";
            }
        } else {
            envelope = envelope * (1.0f - release_time) + absolute_sample * release_time;
            if (envelope > threshold) {
                state = "Release";
            }
        }
        current_sample_debug.envelope_value = envelope;
        current_sample_debug.state = state;

        // Step 2: Gain Calculation
        if (envelope > threshold) {
            gain = 1.0f - (envelope - threshold) / (envelope * ratio);
        } else {
            gain = 1.0f;
        }
        current_sample_debug.gain_value = gain;

        // Step 3: Apply Gain
        sample *= gain;
        current_sample_debug.output_value = sample;

        debug_data.push_back(current_sample_debug);
        sample_index++;
    }
}
#else
// In release mode, we use the efficient version without any debugging.
void Compressor::process_audio(std::vector<float>& buffer) {
    for (float& sample : buffer) {
        float absolute_sample = std::abs(sample);
        if (absolute_sample > envelope) {
            envelope = absolute_sample;
        } else {
            envelope = envelope * (1.0f - release_time) + absolute_sample * release_time;
        }

        if (envelope > threshold) {
            gain = 1.0f - (envelope - threshold) / (envelope * ratio);
        } else {
            gain = 1.0f;
        }
        
        sample *= gain;
    }
}
#endif


void Compressor::set_threshold(float threshold) { this->threshold = threshold; }
void Compressor::set_ratio(float ratio) { this->ratio = ratio; }
void Compressor::set_attack(float attack) { this->attack_time = attack; }
void Compressor::set_release(float release) { this->release_time = release; }


extern "C" {
    __declspec(dllexport) void process_audio_buffer(float* buffer, int size, float threshold, float ratio, float attack, float release) {
        Compressor compressor;
        compressor.set_threshold(threshold);
        compressor.set_ratio(ratio);
        compressor.set_attack(attack);
        compressor.set_release(release);

        std::vector<float> audio_vector(buffer, buffer + size);
#ifdef _DEBUG
        // In debug mode, we need a dummy vector to pass to the function, but we don't use it here.
        std::vector<DebugSample> dummy_debug_data;
        compressor.process_audio(audio_vector, dummy_debug_data);
#else
        compressor.process_audio(audio_vector);
#endif
        std::copy(audio_vector.begin(), audio_vector.end(), buffer);
    }
}