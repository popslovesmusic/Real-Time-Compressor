#include "compressor.h"
#include <cmath>
#include <numeric>
#include <vector>
#include <algorithm> // For std::copy

/**
 * @file compressor.cpp
 * @brief Implements the Compressor class and the C-style wrapper for Python.
 */

// Constructor initializes default parameters
Compressor::Compressor() {
    threshold = 0.0f;
    ratio = 1.0f;
    attack_time = 0.05f;
    release_time = 0.2f;
    gain = 1.0f;
    envelope = 0.0f;
}

// The main audio processing function
void Compressor::process_audio(std::vector<float>& buffer) {
    // This is the core loop that processes each sample in the buffer
    for (float& sample : buffer) {
        // Step 1: Detect the signal's envelope using a simple follower.
        float absolute_sample = std::abs(sample);
        if (absolute_sample > envelope) {
            // Fast attack: immediately jump to the new peak.
            // A more advanced implementation would use attack_time here.
            envelope = absolute_sample;
        } else {
            // Slow decay (release): gradually fall back down.
            envelope = envelope * (1.0f - release_time) + absolute_sample * release_time;
        }

        // Step 2: Calculate the gain reduction based on the envelope.
        if (envelope > threshold) {
            // Standard gain reduction formula.
            gain = 1.0f - (envelope - threshold) / (envelope * ratio);
        } else {
            // No gain reduction if the signal is below the threshold.
            gain = 1.0f;
        }
        
        // Step 3: Apply the calculated gain to the current sample.
        sample *= gain;
    }
}

// Implement setters
void Compressor::set_threshold(float threshold) {
    this->threshold = threshold;
}

void Compressor::set_ratio(float ratio) {
    this->ratio = ratio;
}

void Compressor::set_attack(float attack) {
    this->attack_time = attack;
}

void Compressor::set_release(float release) {
    this->release_time = release;
}


// --- C-style Wrapper for Python Interoperability ---

/**
 * @brief A C-style wrapper function that acts as the entry point for Python.
 * * This function creates and configures a Compressor object, processes the audio data,
 * and handles the data type conversion between Python's raw buffer and C++'s std::vector.
 * * @param buffer A pointer to the start of the audio buffer from Python.
 * @param size The number of samples in the buffer.
 * @param threshold The compression threshold (0.0 to 1.0).
 * @param ratio The compression ratio.
 * @param attack The attack time in seconds.
 * @param release The release time in seconds.
 */
extern "C" {
    __declspec(dllexport) void process_audio_buffer(float* buffer, int size, float threshold, float ratio, float attack, float release) {
        // 1. Create an instance of our advanced Compressor class
        Compressor compressor;

        // 2. Configure the compressor with parameters from Python
        compressor.set_threshold(threshold);
        compressor.set_ratio(ratio);
        compressor.set_attack(attack);
        compressor.set_release(release);

        // 3. Create a std::vector from the raw C-style array for compatibility
        std::vector<float> audio_vector(buffer, buffer + size);

        // 4. Call the advanced processing method
        compressor.process_audio(audio_vector);

        // 5. Copy the processed audio from the vector back into the original buffer
        std::copy(audio_vector.begin(), audio_vector.end(), buffer);
    }
}