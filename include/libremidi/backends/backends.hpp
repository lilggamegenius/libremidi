#pragma once
#include <tuple>

#if !__has_include(<weak_libjack.h>) && !__has_include(<jack/jack.h>)
  #if defined(LIBREMIDI_JACK)
    #undef LIBREMIDI_JACK
  #endif
#endif
#if !defined(LIBREMIDI_ALSA) && !defined(LIBREMIDI_JACK) && !defined(LIBREMIDI_COREAUDIO) \
    && !defined(LIBREMIDI_WINMM)
  #define LIBREMIDI_DUMMY
#endif

#if defined(LIBREMIDI_ALSA)
  #include <libremidi/backends/alsa.hpp>
  #include <libremidi/backends/raw_alsa.hpp>
#endif

#if defined(LIBREMIDI_JACK)
  #include <libremidi/backends/jack.hpp>
#endif

#if defined(LIBREMIDI_COREAUDIO)
  #include <libremidi/backends/coreaudio.hpp>
#endif

#if defined(LIBREMIDI_WINMM)
  #include <libremidi/backends/winmm.hpp>
#endif

#if defined(LIBREMIDI_WINUWP)
  #include <libremidi/backends/winuwp.hpp>
#endif

#if defined(LIBREMIDI_EMSCRIPTEN)
  #include <libremidi/backends/emscripten.hpp>
#endif

#include <libremidi/backends/dummy.hpp>

namespace libremidi
{

namespace
{
// The order here will control the order of the API search in
// the constructor.
template <typename unused, typename... Args>
constexpr auto make_tl(unused, Args...)
{
  return std::tuple<Args...>{};
}
static constexpr auto available_backends = make_tl(
    0
#if defined(LIBREMIDI_ALSA)
    ,
    raw_alsa_backend{}, alsa_backend{}
#endif
#if defined(LIBREMIDI_COREAUDIO)
    ,
    core_backend{}
#endif
#if defined(LIBREMIDI_JACK)
    ,
    jack_backend{}
#endif
#if defined(LIBREMIDI_WINMM)
    ,
    winmm_backend{}
#endif
#if defined(LIBREMIDI_WINUWP)
    ,
    winuwp_backend{}
#endif
#if defined(LIBREMIDI_EMSCRIPTEN)
    ,
    emscripten_backend{}
#endif
    ,
    dummy_backend{});

// There should always be at least one back-end.
static_assert(std::tuple_size_v<decltype(available_backends)> >= 1);

template <typename F>
auto for_all_backends(F&& f)
{
  std::apply([&](auto&&... x) { (f(x), ...); }, available_backends);
}

template <typename F>
auto for_backend(libremidi::API api, F&& f)
{
  static constexpr auto is_api
      = [](auto& backend, libremidi::API api) { return backend.API == api; };
  std::apply([&](auto&&... b) { ((is_api(b, api) && (f(b), true)) || ...); }, available_backends);
}
}
}