"""The third-party libraries the port links statically, built from pinned source releases.

usage: python3 port/deps.py <target> <prefix>      (CMake runs it when configuring)

- SDL3: the window, input and sound of the desktop host (platform/host/sdl).
- FFmpeg, reduced to what the game's media need: Bink video and audio (the movies),
  Smacker (RAD's other player, implemented by the platform), and the sounds' WAV
  codecs (IMA ADPCM, 8- and 16-bit PCM, MP3 for the music), with their demuxers.

The sources are downloaded once to out/deps/src and checked against their SHA-256; the
libraries and headers are installed to <prefix> (lib/, include/). A stamp in <prefix>
skips the build when nothing changed.
"""
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tarfile
import urllib.request

PORT = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(PORT)
SRC = os.path.join(ROOT, 'out', 'deps', 'src')

SOURCES = {
    'sdl3': ('https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz',
             '7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68'),
    'ffmpeg': ('https://ffmpeg.org/releases/ffmpeg-9.0.2.tar.xz',
               '8c3850283eb25fa026482078a04051e0be17347b09ef81a0849bec15a96e002e'),
}

# The libraries of each target (the web host has its own window, input and sound).
TARGET_DEPS = {'macos': ['sdl3', 'ffmpeg'], 'web': ['ffmpeg']}

FFMPEG_FEATURES = [
    '--disable-everything', '--disable-autodetect', '--disable-programs', '--disable-doc',
    '--disable-network', '--disable-avdevice', '--disable-avfilter', '--disable-debug',
    '--enable-avcodec', '--enable-avformat', '--enable-avutil', '--enable-swresample', '--enable-swscale',
    '--enable-decoder=bink,binkaudio_rdft,binkaudio_dct,smacker,smackaud,'
    'adpcm_ima_wav,pcm_u8,pcm_s16le,mp3float',
    '--enable-demuxer=bink,smacker,wav,mp3', '--enable-parser=mpegaudio',
    '--enable-static', '--disable-shared', '--enable-pic',
]
FFMPEG_TARGET = {
    # Apple's archiver and tools by path: a GNU ar first in PATH writes archives Apple's
    # linker rejects
    'macos': ['--arch=arm64', '--extra-cflags=-mmacosx-version-min=13.0',
              '--extra-ldflags=-mmacosx-version-min=13.0', '--ar=/usr/bin/ar', '--nm=/usr/bin/nm',
              '--ranlib=/usr/bin/ranlib', '--strip=/usr/bin/strip'],
    # WebAssembly: no assembly, threads like the rest of the port
    'web': ['--target-os=none', '--arch=x86_32', '--enable-cross-compile', '--disable-asm',
            '--disable-inline-asm', '--disable-stripping', '--cc=emcc', '--cxx=em++', '--ar=emar',
            '--ranlib=emranlib', '--nm=emnm', '--extra-cflags=-pthread -O2', '--extra-ldflags=-pthread'],
}


def fetch(name):
    """The unpacked source of a library (downloaded and checked on first use)."""
    url, sha = SOURCES[name]
    archive = os.path.join(SRC, os.path.basename(url))
    os.makedirs(SRC, exist_ok=True)
    if not os.path.isfile(archive):
        print('deps: downloading %s' % url, flush=True)
        with urllib.request.urlopen(url) as r, open(archive + '.part', 'wb') as f:
            shutil.copyfileobj(r, f)
        os.replace(archive + '.part', archive)
    h = hashlib.sha256(open(archive, 'rb').read()).hexdigest()
    if h != sha:
        sys.exit('deps: %s has SHA-256 %s, expected %s' % (archive, h, sha))
    top = os.path.join(SRC, name)
    if not os.path.isdir(top):
        with tarfile.open(archive) as t:
            first = t.getnames()[0].split('/')[0]
            t.extractall(SRC, filter='data')
        os.replace(os.path.join(SRC, first), top)
    return top


def run(cmd, cwd):
    print('deps: ' + ' '.join(cmd), flush=True)
    subprocess.run(cmd, cwd=cwd, check=True)


def jobs():
    return str(os.cpu_count() or 4)


def build_sdl3(target, prefix, work):
    src = fetch('sdl3')
    build = os.path.join(work, 'sdl3')
    run(['cmake', '-S', src, '-B', build, '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release',
         '-DCMAKE_INSTALL_PREFIX=' + prefix, '-DSDL_SHARED=OFF', '-DSDL_STATIC=ON', '-DSDL_TEST_LIBRARY=OFF',
         '-DSDL_EXAMPLES=OFF', '-DCMAKE_OSX_ARCHITECTURES=arm64', '-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0'], ROOT)
    run(['cmake', '--build', build], ROOT)
    run(['cmake', '--install', build], ROOT)


def build_ffmpeg(target, prefix, work):
    src = fetch('ffmpeg')
    build = os.path.join(work, 'ffmpeg')
    shutil.rmtree(build, ignore_errors=True)
    os.makedirs(build)
    cfg = [os.path.join(src, 'configure'), '--prefix=' + prefix] + FFMPEG_FEATURES
    cfg += FFMPEG_TARGET[target]
    if target == 'web':
        cfg = ['emconfigure'] + cfg
    run(cfg, build)
    run(['make', '-j', jobs()], build)
    run(['make', 'install'], build)


BUILDERS = {'sdl3': build_sdl3, 'ffmpeg': build_ffmpeg}


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in TARGET_DEPS:
        sys.exit(__doc__)
    target, prefix = sys.argv[1], os.path.abspath(sys.argv[2])
    stamp = os.path.join(prefix, 'deps.json')
    want = {'target': target, 'sources': {n: SOURCES[n] for n in TARGET_DEPS[target]},
            'ffmpeg': FFMPEG_FEATURES + FFMPEG_TARGET[target]}
    if os.path.isfile(stamp) and json.load(open(stamp)) == want:
        return
    shutil.rmtree(prefix, ignore_errors=True)
    work = prefix + '-build'
    for name in TARGET_DEPS[target]:
        BUILDERS[name](target, prefix, work)
    shutil.rmtree(work, ignore_errors=True)
    with open(stamp, 'w') as f:
        json.dump(want, f, indent=1)


if __name__ == '__main__':
    main()
