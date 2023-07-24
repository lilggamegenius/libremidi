// midiprobe.cpp
//
// Simple program to check MIDI inputs and outputs.
//
// by Gary Scavone, 2003-2012.

#include <cstdlib>
#include <iostream>
#include <map>
#include <libremidi/libremidi.hpp>

#if defined(__APPLE__)
  #include <CoreFoundation/CoreFoundation.h>
#endif

int main()
try
{
  // Create an api map.
  static std::map<libremidi::API, std::string> apiMap{
      {libremidi::API::MACOSX_CORE, "OS-X CoreMidi"},
      {libremidi::API::WINDOWS_MM, "Windows MultiMedia"},
      {libremidi::API::WINDOWS_UWP, "Windows UWP"},
      {libremidi::API::UNIX_JACK, "Jack Client"},
      {libremidi::API::LINUX_ALSA_SEQ, "Linux ALSA (Seq)"},
      {libremidi::API::LINUX_ALSA_RAW, "Linux ALSA (Raw)"},
      {libremidi::API::DUMMY, "Dummy (no driver)"},
  };

  std::vector<std::unique_ptr<libremidi::observer>> observers;
  std::cout << "\nCompiled APIs:\n";
  for (auto api : libremidi::available_apis())
  {
    libremidi::observer_configuration cbs;
    cbs.input_added = [=](int i, std::string n) {
      std::cerr << apiMap[api] << " : input added " << i << " => " << n << "\n";
    };
    cbs.input_removed = [=](int i, std::string n) {
      std::cerr << apiMap[api] << " : input removed " << i << " => " << n << "\n";
    };
    cbs.output_added = [=](int i, std::string n) {
      std::cerr << apiMap[api] << " : output added " << i << " => " << n << "\n";
    };
    cbs.output_removed = [=](int i, std::string n) {
      std::cerr << apiMap[api] << " : output removed " << i << " => " << n << "\n";
    };
    std::cout << "  " << apiMap[api] << std::endl;
    observers.push_back(std::make_unique<libremidi::observer>(api, cbs));
  }

#if defined(__APPLE__)
  // On macOS, observation can *only* be done in the main thread
  // with an active CFRunLoop.
  CFRunLoopRun();
#else
  getchar();
#endif
  return 0;
}
catch (const libremidi::midi_exception& error)
{
  std::cerr << error.what() << std::endl;
  return EXIT_FAILURE;
}
