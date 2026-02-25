#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <algorithm>
using namespace std;

/*
ImageMagic :-
ImageMagick is a free and open-source software suite used for creating, editing, converting, and processing images — all from the command line or within scripts.
Resize, crop, rotate, flip, or convert a bunch of images at once.
Example: Convert .png to .jpg or .pdf to .png.
Add text, draw shapes, apply filters, effects (blur, sharpen, grayscale, etc.).
*/

/*
Ghostscript :- 
Ghostscript can compress or optimize PDF files for size reduction.
Merging, splitting, watermarking, or rotating PDFs.
*/

#ifdef _WIN32
const string gsCmd = "gswin64c";
const string imgCmd = "magick"; // for ImageMagick (Windows)
#else
const string gsCmd = "gs";
const string imgCmd = "convert"; // or "magick" depending on system
#endif

// Function declarations
bool fileExists(const string& filename);
long getFileSizeKB(const string& filename);
void resizepdf(const string& inputFile, long targetSizeKB);
void resizeImage(const string& inputFile, long targetSizeKB);
string toLowerExt(const string& filename);
void resizeFile();  // entry point