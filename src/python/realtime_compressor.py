# python/realtime_compressor.py
import ctypes
import numpy as np
import pyaudio
import os
from pydub import AudioSegment
from io import BytesIO

# --- Configuration ---
# Your C++ engine (the .dll file you just compiled)
DLL_PATH = os.path.join(os.path.dirname(__file__), "..", "..", "build", "compressor.dll")

# Add the path to the Visual C++ Redistributable libraries.
os.add_dll_directory(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.44.35112\x64\Microsoft.VC143.CRT")

# The audio file you want to compress
INPUT_FILE = "C:\\hpcf\\assets\\test_audio.mp3" # Changed to .mp3

# --- Load the C++ Engine ---
try:
    compressor_dll = ctypes.CDLL(DLL_PATH)
    print("Successfully loaded C++ engine.")

    # Define the C++ function signature to ensure correct data types
    compressor_dll.process_audio_buffer.argtypes = [
        np.ctypeslib.ndpointer(dtype=np.float32, flags="C_CONTIGUOUS"),
        ctypes.c_int,
        ctypes.c_float,
        ctypes.c_float,
        ctypes.c_float,
        ctypes.c_float
    ]
    compressor_dll.process_audio_buffer.restype = None

except OSError as e:
    print(f"Error loading DLL: {e}")
    print("Please ensure compressor.dll is in the correct directory.")
    exit()

# --- Real-time Audio Stream from File ---
# We'll put all the main variables in a class to make them accessible to the callback
class AudioData:
    def __init__(self, wf):
        self.wf = wf
        self.data = wf.readframes(1024)

    def callback(self, in_data, frame_count, time_info, status):
        # We process a chunk of audio from our wave file
        chunk = self.wf.readframes(frame_count)
        
        if not chunk:
            return (chunk, pyaudio.paComplete)
        
        # Convert the raw WAV data into a numpy array (our "workbook array")
        audio_buffer = np.frombuffer(chunk, dtype=np.float32)

        # Call the C++ function to process the audio buffer
        compressor_dll.process_audio_buffer(
            np.ascontiguousarray(audio_buffer),
            len(audio_buffer),
            0.5, # threshold
            4.0, # ratio
            0.05, # attack
            0.5 # release
        )
        
        # Return the processed audio
        return (audio_buffer.tobytes(), pyaudio.paContinue)

def main():
    p = pyaudio.PyAudio()
    
    # --- Load Audio File with pydub ---
    try:
        audio = AudioSegment.from_file(INPUT_FILE)
        
        buffer = BytesIO()
        audio.export(buffer, format="wav")
        buffer.seek(0)
        
        import wave
        wf = wave.open(buffer, 'rb')
        
        audio_data = AudioData(wf)
        
        # Ensure the output stream matches the file's format
        # Increased buffer size to 4096 for better stability
        stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                        channels=wf.getnchannels(),
                        rate=wf.getframerate(),
                        output=True,
                        stream_callback=audio_data.callback)
        
        print(f"Successfully loaded audio file: {INPUT_FILE}")

    except Exception as e:
        print(f"Error loading audio file: {e}")
        print("Please ensure the file exists and pydub/ffmpeg are installed.")
        exit()

    print("\nStarting real-time file processing...")
    print("Press Ctrl+C to stop.")

    try:
        # We start the stream and let PyAudio call the callback function
        stream.start_stream()
        
        # Keep the main thread alive until the stream finishes
        while stream.is_active():
            pass

    except KeyboardInterrupt:
        print("\nStopping audio stream...")
    
    finally:
        # Clean up the audio stream and PyAudio instance
        stream.stop_stream()
        stream.close()
        p.terminate()

if __name__ == "__main__":
    main()
