#include "compressor.h"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <deque>

/**
 * @file compressor.cpp
 * @brief Implements the Compressor class and the C-style wrapper for Python.
 */

Compressor::Compressor() {
    threshold = 0.0f;
    ratio = 1.0f;
    attack_time = 0.05f;
    release_time = 0.2f;
    lookahead_time_ms = 0.0f;
    gain = 1.0f;
    envelope = 0.0f;
}

#ifdef _DEBUG
void Compressor::process_audio(std::vector<float>& buffer, std::vector<DebugSample>& debug_data, float sample_rate) {
    debug_data.clear();
    size_t lookahead_samples = static_cast<size_t>((lookahead_time_ms / 1000.0f) * sample_rate);
    
    if (lookahead_buffer.size() != lookahead_samples) {
        lookahead_buffer.assign(lookahead_samples, 0.0f);
        gain_buffer.assign(lookahead_samples, 1.0f);
    }
    
    int sample_index = 0;
    for (float& sample : buffer) {
        float current_sample = sample;
        float delayed_sample = lookahead_buffer.front();
        float delayed_gain = gain_buffer.front();
        lookahead_buffer.pop_front();
        gain_buffer.pop_front();
        lookahead_buffer.push_back(current_sample);

        DebugSample current_sample_debug;
        current_sample_debug.sample_index = sample_index + 1;
        current_sample_debug.input_value = current_sample;
        std::string state = "Idle";

        float absolute_sample = std::abs(current_sample);
        if (absolute_sample > envelope) {
            envelope = absolute_sample;
            if (envelope > threshold) state = "Attack";
        } else {
            envelope = envelope * (1.0f - release_time) + absolute_sample * release_time;
            if (envelope > threshold) state = "Release";
        }
        current_sample_debug.envelope_value = envelope;
        current_sample_debug.state = state;

        if (envelope > threshold) {
            gain = 1.0f - (envelope - threshold) / (envelope * ratio);
        } else {
            gain = 1.0f;
        }
        current_sample_debug.gain_value = gain;
        gain_buffer.push_back(gain);
        
        delayed_sample *= delayed_gain;
        current_sample_debug.output_value = delayed_sample;
        sample = delayed_sample;

        debug_data.push_back(current_sample_debug);
        sample_index++;
    }
}
#else
void Compressor::process_audio(std::vector<float>& buffer, float sample_rate) {
    size_t lookahead_samples = static_cast<size_t>((lookahead_time_ms / 1000.0f) * sample_rate);
    if (lookahead_buffer.size() != lookahead_samples) {
        lookahead_buffer.assign(lookahead_samples, 0.0f);
        gain_buffer.assign(lookahead_samples, 1.0f);
    }

    for (float& sample : buffer) {
        float current_sample = sample;
        float delayed_sample = lookahead_buffer.front();
        float delayed_gain = gain_buffer.front();
        lookahead_buffer.pop_front();
        gain_buffer.pop_front();
        lookahead_buffer.push_back(current_sample);

        float absolute_sample = std::abs(current_sample);
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
        
        gain_buffer.push_back(gain);
        delayed_sample *= delayed_gain;
        sample = delayed_sample;
    }
}
#endif

void Compressor::set_threshold(float threshold) { this->threshold = threshold; }
void Compressor::set_ratio(float ratio) { this->ratio = ratio; }
void Compressor::set_attack(float attack_ms) { this->attack_time = attack_ms; }
void Compressor::set_release(float release_ms) { this->release_time = release_ms; }
void Compressor::set_lookahead(float lookahead_ms) { this->lookahead_time_ms = lookahead_ms; }

extern "C" {
    __declspec(dllexport) void process_audio_buffer(float* buffer, int size, float threshold, float ratio, float attack, float release) {
        Compressor compressor;
        compressor.set_threshold(threshold);
        compressor.set_ratio(ratio);
        compressor.set_attack(attack);
        compressor.set_release(release);
        
        std::vector<float> audio_vector(buffer, buffer + size);
        float sample_rate = 44100.0f;

#ifdef _DEBUG
        std::vector<DebugSample> dummy_debug_data;
        compressor.process_audio(audio_vector, dummy_debug_data, sample_rate);
#else
        compressor.process_audio(audio_vector, sample_rate);
#endif
        std::copy(audio_vector.begin(), audio_vector.end(), buffer);
    }
}