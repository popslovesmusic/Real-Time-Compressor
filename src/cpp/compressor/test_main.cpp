#include <iostream>
#include <vector>
#include <iomanip> // For formatting output
#include "compressor.h"

// The main function for our test harness
int main() {
    // 1. Create an instance of our Compressor
    Compressor compressor;

    // 2. Define test parameters
    float sample_rate = 1000.0f; // Using a simple sample rate for easy math
    float threshold = 0.5f;
    float ratio = 4.0f;
    float release = 0.5f;
    float lookahead_ms = 2.0f; // Set a 2ms lookahead

    // 3. Configure the compressor
    compressor.set_threshold(threshold);
    compressor.set_ratio(ratio);
    compressor.set_release(release);
    compressor.set_lookahead(lookahead_ms);

    // 4. Create a sharp transient signal to test the lookahead
    std::vector<float> audio_buffer = { 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f };

    // Print info
    std::cout << "--- Lookahead Test ---" << std::endl;
    std::cout << "Sample Rate: " << sample_rate << " Hz" << std::endl;
    std::cout << "Lookahead: " << lookahead_ms << " ms" << std::endl;
    std::cout << "Threshold: " << threshold << ", Ratio: " << ratio << std::endl << std::endl;

    // 5. Create a vector to hold our debug data
    std::vector<DebugSample> debug_results;

    // 6. Process the audio
    compressor.process_audio(audio_buffer, debug_results, sample_rate);

    // 7. Print the results in a formatted table
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