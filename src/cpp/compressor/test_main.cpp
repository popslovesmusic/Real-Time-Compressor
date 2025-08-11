#include <iostream>
#include <vector>
#include <iomanip> // For formatting output
#include "compressor.h"

// The main function for our test harness
int main() {
    // 1. Create an instance of our Compressor
    Compressor compressor;

    // 2. Set parameters
    compressor.set_threshold(0.5f);
    compressor.set_ratio(4.0f);
    compressor.set_release(0.2f); // Adjusted for a more illustrative release state

    // 3. Create a sample audio buffer
    std::vector<float> audio_buffer = { 0.1f, 0.2f, 0.8f, 0.4f };
    std::cout << "Input Buffer: 0.1 0.2 0.8 0.4" << std::endl;
    std::cout << "Threshold: 0.5, Ratio: 4.0, Release: 0.2" << std::endl << std::endl;

    // 4. Create a vector to hold our debug data
    std::vector<DebugSample> debug_results;

    // 5. Process the audio
    compressor.process_audio(audio_buffer, debug_results);

    // 6. Print the results in a formatted table
    std::cout << std::left
              << std::setw(10) << "Sample"
              << std::setw(10) << "Input"
              << std::setw(12) << "Envelope"
              << std::setw(10) << "Gain"
              << std::setw(10) << "State"
              << std::setw(10) << "Output" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    for (const auto& d : debug_results) {
        std::cout << std::left << std::fixed << std::setprecision(3)
                  << std::setw(10) << d.sample_index
                  << std::setw(10) << d.input_value
                  << std::setw(12) << d.envelope_value
                  << std::setw(10) << d.gain_value
                  << std::setw(10) << d.state
                  << std::setw(10) << d.output_value << std::endl;
    }

    return 0;
}