#include <iostream>
#include <fstream>
#include "huff.cpp"
#include <conio.h>
#include "resize.cpp"

using namespace std;
bool compareFiles(const string& file1, const string& file2) {
    ifstream f1(file1, ios::binary);
    ifstream f2(file2, ios::binary);

    cout<<"opening files...."<<endl;
    if (!f1 || !f2) return false;

    char ch1, ch2;
    int lineNumber = 1;
    int byteIndex = 0;

    while (f1.get(ch1) && f2.get(ch2)) {
        byteIndex++;
        if (ch1 != ch2) {
            cout << "Mismatch at byte " << byteIndex;
            cout << " (likely at line " << lineNumber << ")";

            cout << "\n";
            return false;
        }

        if (ch1 == '\n') {
            lineNumber++;
        }
    }

    return true;
}


void stop()
{
    cout<<endl<<"Press Any Key To Continue..."<<endl;
    _getch();
    system("cls");
}

int main() {
    int n;
    string inputFile, compressedFile, decompressedFile;
    system("cls");
    do{
        cout<<"1 : file compression"<<endl;
        cout<<"2 : file decompression"<<endl;
        cout<<"3 : compare orignal with decompressed file"<<endl;
        cout<<"4 : Reduce Size Of PDF Or Image"<<endl;
        cout<<"5 : exit"<<endl;
        cin>>n;
        system("cls");
        switch(n)
        {
            case 1:
            cout << "Enter file to compress (e.g. file.txt) : ";
            cin >> inputFile;
            cout<<"Enter name for saving file after compression (e.g. file.bin) : ";
            cin>>compressedFile;
        
            cout << "Compressing...\n";
            compress(inputFile, compressedFile);
            stop();
            break;

            case 2:
            cout<<"enter name of compressed file(e.g. file.bin) and name for saving decopmressed file(e.g. data.txt) : "<<endl;
            cin>>compressedFile;
            cin>>decompressedFile;
            cout << "Decompressing...\n";
            decompress(compressedFile, decompressedFile);
        
            cout << "Done!\n";
            cout << "Compressed file: " << compressedFile << "\n";
            cout << "Decompressed file: " << decompressedFile << "\n";
            stop();
            break;

            case 3:
            cout<<"enter name of decopmressed file and original file : "<<endl;
            cin>>inputFile;
            cin>>decompressedFile;
            cout << "Verifying...\n";
            if (compareFiles(inputFile, decompressedFile)) {
                cout << "Success! The original and decompressed files are identical.\n";
            } else {
                cout << "Mismatch detected. The files are different.\n";
            }
            stop();
            break;

            case 4:
            resizeFile();
            stop();
            break;
           
            case 5:
            cout<<"Thank You...."<<endl;
            break;

            default:
            cout<<"Enter Valid Expression!"<<endl;
        }
        

    }while(n != 5);

    return 0;
}
