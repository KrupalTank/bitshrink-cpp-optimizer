#include "resize.cpp"

//filename.c_str() converts the C++ string to a C-style string (i.e., a const char*) that fopen can use.
bool fileExists(const string& filename) {
    FILE* file = fopen(filename.c_str(), "rb");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

long getFileSizeKB(const string& filename)
{
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) return -1;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    //ftell returns the current position of the file pointer (which is now at the end), giving the size in bytes.

    fclose(file);
    return size / 1024;
}

// Extracts and returns the lowercase file extension from filename.
string toLowerExt(const string& filename) {
    size_t dotPos = filename.find_last_of(".");
    //Finds the last dot (.) in the filename using find_last_of.

    if (dotPos == string::npos) return "";
    // If no dot is found (npos means “not found”), it means the file has no extension.
    // The function re turns an empty string in this case.

    string ext = filename.substr(dotPos + 1);
    // Extracts the substring that starts after the last dot, which is the file extension.
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}


// Compresses a PDF file using Ghostscript.
// Uses -dPDFSETTINGS=/screen and JPEG quality to shrink file size.
// Checks if final size is within target.

void resizepdf(const string& inputFile, long targetSizeKB)
{
    string outputFile = "resized_" + inputFile;
    int quality = 50;
    cout<<"PDF Will Be Reduced To Its Minimum Possible Size."<<endl;

    string command = gsCmd + " -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dNOPAUSE -dQUIET -dBATCH "
                        "-dPDFSETTINGS=/screen "
                        "-dAutoFilterColorImages=false -dAutoFilterGrayImages=false "
                        "-dColorImageFilter=/DCTEncode -dGrayImageFilter=/DCTEncode "
                        "-dColorImageDownsampleType=/Average "
                        "-dColorImageResolution=72 "
                        "-dJPEGQ=" + to_string(quality) +
                        " -sOutputFile=\"" + outputFile + "\" \"" + inputFile + "\"";

    int result = system(command.c_str());

    if (result == 0 && fileExists(outputFile)) {
        long sizeKB = getFileSizeKB(outputFile);
        cout << "\nOutput PDF size: " << sizeKB << " KB\n";

        if(sizeKB <= targetSizeKB)
        {
            cout << "Successfully compressed to target size.\n";
        }
        else
        {
            cout << "We cant reduce this PDF to size specified by you.\n";
        }
    }
    else
    {
        cout << "\nGhostscript failed or output not created.\n";
    }
}

// Compresses an image using ImageMagick.
// Tries multiple quality levels from 90% to 10% until it meets the target size.

void resizeImage(const string& inputFile, long targetSizeKB) {
    string outputFile = "resized_" + inputFile;

    int qualityLevels[] = {90, 70, 50, 30, 10};
    bool success = false;

    for(int quality : qualityLevels)
    {
        cout << "Trying image compression with quality: " << quality << "%...\n";

        string command = imgCmd + " \"" + inputFile + "\" -quality " + to_string(quality) + " \"" + outputFile + "\"";

        int result = system(command.c_str());

        if (result == 0 && fileExists(outputFile)) {
            long sizeKB = getFileSizeKB(outputFile);
            cout << "\nOutput image size: " << sizeKB << " KB\n";

            if (sizeKB <= targetSizeKB) {
                cout << "Successfully compressed to target size.\n";
                success = true;
                break;
            }
        } else {
            cout << "ImageMagick failed or output not created.\n";
            break;
        }
    }
    if(!success)
    {
        cout<<"We cant reduce this image to size specified by you."<<endl;
    }
}

// Main interface function. Asks user for input file and target size, calls appropriate function based on extension.
void resizeFile() {
    string inputFile;
    cout << "Enter the path to the input file (PDF/image): ";
    cin >> inputFile;

    if (!fileExists(inputFile)) {
        cerr << "File does not exist.\n";
        return;
    }

    string sizeUnit;
    double targetSize;

    cout << "Enter the desired size: ";
    cin >> targetSize;
    cout << "Is this in KB or MB? (Enter 'KB' or 'MB'): ";
    cin >> sizeUnit;

    long targetSizeKB = (sizeUnit == "MB" || sizeUnit == "mb") ? static_cast<long>(targetSize * 1024) : static_cast<long>(targetSize);
                                                               
    string ext = toLowerExt(inputFile);
    if (ext == "pdf") {
        resizepdf(inputFile, targetSizeKB);
    } else if (ext == "jpg" || ext == "jpeg" || ext == "png") {
        resizeImage(inputFile, targetSizeKB);
    } else {
        cout << "Unsupported file type: " << ext << "\n";
    }
}
