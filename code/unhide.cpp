//code by Jason West

//  This program is meant for basic educational purposes.
//  It is to help give a basic understanding of LSB Stenography Works


// *  Dependencies:
// *        g++ v9
// *        FreeImage API
// *            install with "sudo apt-get install -y libfreeimage-dev"
// *        C++17
// *
// *
// *   This program will unhide a message inside a png that 
// *   used the ./hide program to inbed it. Otherwise it will report no such message exists.
// *
// *    In the case there is no message I decided not to delete the junk txt file so it can be used
// *    for demonstration purposes. Also did it incase you accidentally entered a text file you like then.


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <filesystem>
#include <stdio.h>
#include "FreeImage.h"
using namespace std;

int main(int argc, char* argv[]){
    //validate command line arguments entered correctly

    if (argc != 3){
        cout<< "\nInput incorrect.\nUse Format: ./unhide input.png message.txt" << endl;
        exit(1);
    }
    
    std::filesystem::path png_in = argv[1];
    std::filesystem::path msg_out = argv[2];

    if ( png_in.extension() != ".png"){
        cout << "\nYour input file is not a png" << endl;
        exit(2);
    } else if ( msg_out.extension() != ".txt"){
        cout << "\nYour output file is not a text file" <<endl;
        exit(3);
    }


    //open text file
    ofstream message_file(argv[2]);
    if (!message_file.is_open()){
        cout << "\nOutput file failed to open" << endl;
        exit(4);
    }

    //set up png stuff
            //initalize some stuff
            FREE_IMAGE_FORMAT fif_in = FIF_PNG;
            //pointer to image
            FIBITMAP *dib_in(0);
            //image width and height
            unsigned int width_in(0), height_in(0);

            //load image
            dib_in = FreeImage_Load(fif_in,argv[1]);

            //check if load failed
            if (!dib_in){
                cout<< endl<<"\nFailed to open png file"<<endl;
                exit(5);
            }

            //get image width and height
            width_in = FreeImage_GetWidth(dib_in);
            height_in = FreeImage_GetHeight(dib_in);

            //check to make sure height/width did not fail to load
            if (width_in == 0 || height_in == 0){
                cout << endl << "\nYour png is empty" << endl;
                exit(6);
            }


    //set up stuff to unhide the message
        //color values of png in
        RGBQUAD curr_color;
        BYTE a_in;
        BYTE red_in;
        BYTE green_in;
        BYTE blue_in;

        //txt file bits that will contain hidden message
        uint8_t red_bits;
        uint8_t blue_bits;
        uint8_t green_bits;
        uint8_t alpha_bits;

        //bitmask to retreave last 2 bits of each color
        uint8_t bitmask = 0x03;

        //used for checking if end of message is reached
        unsigned char curr_char;

    //load pixels
    for (unsigned int y = 0 ; y < height_in; y++){
        for (unsigned int x = 0; x<width_in; x++){
            //get next pixel color
            FreeImage_GetPixelColor(dib_in,x,y,&curr_color);

            //get color bits
            red_in = curr_color.rgbRed;
            green_in = curr_color.rgbGreen;
            blue_in = curr_color.rgbBlue;
            a_in = curr_color.rgbReserved;

            //get text bits
            red_bits = (red_in & bitmask) << 6;
            green_bits = (green_in & bitmask)<< 4;
            blue_bits = (blue_in & bitmask)<< 2;
            alpha_bits = a_in& bitmask;

            //combine bits
            curr_char = red_bits | green_bits | blue_bits | alpha_bits;

            //cout << "\ncurr: " << curr_char << " last: " << last_char << endl;

            //check if end of text
            if ( curr_char == 0b00000011){
                    //close message file
                    message_file.close();
                    cout << "\nYour message has been extracted" << endl;
                    return 0;
            } else{ //otherwise write to file 
                message_file << curr_char;
            }

        }
    }


    //close message file
    message_file.close();

    cout << "\nThere does not seem to be a message in this file using our algorithm" << endl;

    
    return 0;
}