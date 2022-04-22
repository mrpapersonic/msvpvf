# msvpvf
C/C++ port of [msvpvf](https://archive.org/details/msvpvf-1.3-movie-studio-vegas-pro-version-faker-msvpvf-updated-to-1.31-focus-on-vegas)

## What is this??
msvpvf is a tool used to "downgrade" VEGAS Pro project files, written entirely in C. It has no runtime libraries, making it incredibly lightweight. Binaries for OS X and Windows are less than a megabyte in size.

Currently, the GUI doesn't look that great. Making pretty apps with `windows.h` is not the easiest job in the world; if someone is willing enough to make it look prettier, be my guest.

It is based on an old tool under the same name that was written in C#/.NET, but the original website is now down and it hasn't been maintained in over 2 years.

## Compilation
```
git clone https://github.com/mrpapersonic/msvpvf
cd msvpvf
make
```

NOTE: if you are using Windows, there is a GUI available. To compile it, use `make gui`.
