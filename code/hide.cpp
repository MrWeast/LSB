//code by Jason West

//  This program is meant for basic educational purposes.
//  It is to help give a basic understanding of LSB Stenography Works

// *  Dependencies:
// *        g++ v9
// *        FreeImage API
// *            install with "sudo apt-get install -y libfreeimage-dev"
// *        C++17
// *
// *   This program will only hide messages in a png and output a png that 
// *   has your messgae hidden in it. 
// *
// *    To do so it will:
// *        This program will hide 2 bits in each RGBA value for each pixel.
// *        This way 1 char of your message will be hidden in 1 pixel.
// *
// *        It will append a terminating sequence of "#####" to your message 
// *        so when unhiding we will know when.
// *
// *    This program will also determine if your message + terminating sequence is 
// *    too large for your provided png.
// *    
// *    your message is done being retrieved.
// *


//code to hide text in png




#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <filesystem>
#include <stdio.h>
#include "FreeImage.h"


using namespace std;

int main(int argc, char *argv[]){

    ifstream msg_file;

    //validate command line arguments entered correctly
    if (argc != 4){

        cout<< endl<<"Use format: ./hide message.txt image.png output.png" << endl;
        exit(1);
    }

    //open text file
        //validate file is correct type
        std::filesystem::path filepath_txt = argv[1];
        if (filepath_txt.extension() != ".txt"){
            cout<< endl << "the message you are trying to hide in a png is not a text file" << endl;
            exit(2);
            //if correct type open file
        } else {
            msg_file.open(argv[1]);

            //make sure file was opened
            if ( !msg_file.is_open()){
                cout << "\nfailed to open message file"<<endl;
                exit(3);
            }
        }

    //open png file to hide text in

        //initalize some stuff
            FREE_IMAGE_FORMAT fif_in = FIF_PNG;
            //pointer to image
            FIBITMAP *dib_in(0);

            //image width and height
            unsigned int width_in(0), height_in(0);

            std::filesystem::path filepath_in = argv[2];

            //validate file is correct type
        if (filepath_in.extension() != ".png"){
            cout<< endl << "the file you are trying hide your message in is not a png file" << endl;
            exit(4);
        } else {


            //load image
            dib_in = FreeImage_Load(fif_in,argv[2]);

            //check if load failed
            if (!dib_in){
                cout<< endl<<"\nfailed to open png file"<<endl;
                exit(5);
            }
        }

        //get image data
        unsigned int BPP_in = FreeImage_GetBPP(dib_in);    //bits per pixel

        //if not 32 bpp then convert it to a 32 bpp bitmap
        if (BPP_in != 32){
            FreeImage_ConvertTo32Bits(dib_in);
            BPP_in = FreeImage_GetBPP(dib_in); 
        }

        //get image width and height
        width_in = FreeImage_GetWidth(dib_in);
        height_in = FreeImage_GetHeight(dib_in);


        //check to make sure height/width did not fail to load
        if (width_in == 0 || height_in == 0){
            cout << endl << "\nYour png is empty" << endl;
            exit(6);
        }

    //check output file tpye
        std::filesystem::path filepath_out = argv[3];
        if (filepath_out.extension() != ".png"){
            cout<< endl << "the output file you provided is not a png" << endl;
            exit(7);
        } 


    //set up output file data
        FreeImage_Initialise();

        //output bitmap with with same dimensions and bpp as original png
        FIBITMAP* dib_out = FreeImage_Allocate(width_in,height_in,BPP_in);

        //color values for pixels to be output
        RGBQUAD color_out;

        //check to make sure dib_out worked
        if (!dib_out){
            cout << "\nFailed to set up output file" << endl;
            exit(9);
        }

    
    //check if text can be hidden in png
        //are there too many bits to hide?
        int size_txt = filesystem::file_size(filepath_txt); //text file size in bytes
        //can bits be hidden in file
        int num_pixels= width_in * height_in;

        //number of bits that must be hidden per pixel 
        //+1 is for terminating sequence bits
        int num_hidden_bits_per_pixel = (( (size_txt) + 1)/ num_pixels);

    //at most 8 bits can be safely hidden per pixel
    //so if more are hidden then the msg is too big or png too small
    if (num_hidden_bits_per_pixel > 8){
        cout << "Your message is too long for your image.\n There are too many bits to hide per pixel." << endl;
        exit(10);
    } 

    /*
        NOTE: we will assume 2 bits will be changed in each rgba range
    */

    
    //init some stuff

        //color values of png in
        RGBQUAD curr_color;
        BYTE a_in;
        BYTE red_in;
        BYTE green_in;
        BYTE blue_in;

        //txt file bits that will be hidden in respective color values of pixel
        uint8_t red_bits;
        uint8_t blue_bits;
        uint8_t green_bits;
        uint8_t alpha_bits;

        //masks to get bits for respective color values of pixel
        uint8_t red_bits_mask = 0xc0;
        uint8_t green_bits_mask = 0x30;
        uint8_t blue_bits_mask = 0x0c;
        uint8_t alpha_bits_mask = 0x03;

        //used to clear last 2 bits in rgba value in png in so it can be replaced by text bits
        uint8_t bitmask = 0xfc; 

        //the raw bits read from msg
        uint8_t read_bits;

        //used to determine end of message
        bool eof = false;

    //for size of png
    for (unsigned int y = 0 ; y < height_in; y++){
        for (unsigned int x = 0; x<width_in; x++){

            //get next pixel color
            FreeImage_GetPixelColor(dib_in,x,y,&curr_color);
            
            

            //set corresponding bits
            read_bits = msg_file.get();

            //if these is still a char in msg then get bits to hide in png
            if (msg_file){
                
                
                //clear unneeded bits and logical shift needed bits so they are the least signficant
                red_bits = ((read_bits & red_bits_mask) >> 6) & 0x03;
                green_bits = ((read_bits & green_bits_mask) >> 4) & 0x03;
                blue_bits = ((read_bits & blue_bits_mask) >> 2) & 0x03;
                alpha_bits = (read_bits & alpha_bits_mask) & 0x03;
            } else if (eof){ //if end of message do not change pixels
                red_bits=0b00000000;
                blue_bits=0b00000000;
                green_bits=0b00000000;
                alpha_bits=0b00000000;
                bitmask = 0b11111111;
            } else { //set bits to ETX = 0b 00 00 00 11 (end of text)
                red_bits = 0b00000000;
                green_bits = 0b00000000; 
                blue_bits = 0b00000000;
                alpha_bits = 0b00000011;

                eof = true;

            }

            //cout << "red bits: " << +red_bits << " green bits: "<< +green_bits  << " blue bits: " << +blue_bits<< " alpha bits: "   << +alpha_bits << endl;

            //load in png color values
            red_in = curr_color.rgbRed;
            green_in = curr_color.rgbGreen; 
            blue_in =curr_color.rgbBlue;
            a_in = curr_color.rgbReserved;

            //cout << " before msg insertion " << +red_in << " " << +green_in << " " << +blue_in << " " << +a_in<< endl;
            

            //inbed current char of message into output color rgba values
            color_out.rgbRed = (red_in & bitmask) | red_bits;
            color_out.rgbGreen = (green_in & bitmask) | green_bits;
            color_out.rgbBlue = (blue_in & bitmask) | blue_bits;
            color_out.rgbReserved = (a_in & bitmask) | alpha_bits; 
            
            //cout << " after msg insertion " << +(color_out.rgbRed) << " " << +(color_out.rgbGreen) << " " << +(color_out.rgbBlue)<<" " << +(color_out.rgbReserved)<< endl<<endl;

            FreeImage_SetPixelColor(dib_out,x,y,&color_out);
        }
    }
    
    if (FreeImage_Save(FIF_PNG,dib_out,argv[3],PNG_DEFAULT)){
        cout<< "\n Message Successfully Hidden" << endl;
    } else{
        cout << "\nImage Save Failure"<<endl;
        exit(8);
    }


    FreeImage_DeInitialise();
    FreeImage_Unload(dib_in);


    FreeImage_Unload(dib_out);


return 0;
}

