# Third-party code

The game ships these open-source libraries. This repository uses their original,
unmodified source releases instead of decompiled versions; their licences are in
`licenses/`.

| Library | Version | Authors | Licence | Used in | Source here |
|---|---|---|---|---|---|
| zlib | 1.1.3 | Jean-loup Gailly, Mark Adler | zlib licence (`licenses/zlib.txt`) | `zlib.dll` | `src/zlib/` (from zlib.net) |
| libpng | 1.0.6 | Guy Eric Schalnat, Andreas Dilger, Glenn Randers-Pehrson and contributors | libpng licence (`licenses/libpng.txt`) | `SteamboatApp.dll` | `src/SteamboatApp/libpng/` (from libpng.org) |
| IJG libjpeg | 6b | Thomas G. Lane, Independent JPEG Group | IJG licence (`licenses/libjpeg.txt`) | `MfImageLib.dll` | `src/MfImageLib/jpeg/` (matched by decompilation before this file existed) |
| regexpr | 1991 (Python's first copy) | Tatu Ylonen | permissive notice, kept at the top of `src/mfutil/regexpr.cpp` | `mfutil.dll` | `src/mfutil/regexpr.cpp`, `regexpr.h` (the original source with Motion Factory's changes, verified by matching) |
| RAPID | (as built into MfActorLib) | Stefan Gottschalk et al., University of North Carolina at Chapel Hill | UNC licence: free for educational, research and non-profit use; this project is non-commercial | `MfActorLib.dll` | routines in `src/MfActorLib/MfTriangleSet.cpp` (`tri_contact` and helpers) and `MfOrientedBox` (`obb_disjoint`), reconstructed by matching |

This software is based in part on the work of the Independent JPEG Group.

The port links these libraries statically, built from their source releases by
`port/deps.py` (pinned versions, checked by SHA-256); they are not in this repository:

| Library | Version | Licence | Used for |
|---|---|---|---|
| SDL3 | 3.4.16 | zlib licence (`licenses/SDL3.txt`) | the desktop host: window, input, sound (`port/platform/host/sdl`) |
| FFmpeg | 9.0.2, LGPL build with only the decoders the game needs | LGPL 2.1 or later (`licenses/ffmpeg-LGPL-2.1.txt`) | the movies (Bink, Smacker) and sounds (`port/platform/media`) |

FFmpeg is linked statically under the LGPL: this repository's complete source and
`port/deps.py` let anyone rebuild the program against a changed FFmpeg.
