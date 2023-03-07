# msvpvf
C port of [msvpvf](https://archive.org/details/msvpvf-1.3-movie-studio-vegas-pro-version-faker-msvpvf-updated-to-1.31-focus-on-vegas)

![msvpvf CI](https://github.com/mrpapersonic/msvpvf/actions/workflows/build.yml/badge.svg)

## What is this??
msvpvf is a tool used to "downgrade" VEGAS Pro project files, written entirely in C. It has no runtime libraries, making it incredibly lightweight. Binaries for OS X and Windows are less than a megabyte in size.

Currently, the GUI doesn't look that great. Making pretty apps with `windows.h` is not the easiest job in the world; if someone is willing enough to make it look prettier, be my guest.

It is based on an old tool under the same name that was written in C#/.NET, but the original website is now down and it hasn't been maintained in over 2 years.

# Compatibility
There are 3 generations of modern Vegas project files:

Gen 1 | Gen 2 | Gen 3
--- | --- | ---
8.0 - 11.0 | 12.0 - 14.0 | 15.0 - Now

Any version in Gen 3 cannot be ported to Gen 2, and Gen 2 cannot be ported to Gen 1. That's just too complicated for a simple bit modifier. **However**, any file created in an older Vegas will work in a newer one.

## Compilation
```
git clone https://github.com/mrpapersonic/msvpvf
cd msvpvf
make
```

NOTE: if you are using Windows, there is a GUI available. To compile it, use `make gui`.
