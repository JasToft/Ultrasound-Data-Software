#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

float **createDataMatrix(int numElement, int numSample)
{
    // 2D data matrix of size numElement and numSample
    float **RFData = new float*[numElement]; // First dimension of length numElement
    for (int i=0; i<numElement; i++) {
        RFData[i] = new float[numSample]; // Second dimension of length numSample to each element
    }
    
    return RFData; // Return the RFData
}

int loadRFData(float **RFData, const char *fileName, int numElement, int numSample)
{
    // Open the text file fileName
    ifstream input(fileName);
    
    // Check to see if the input failed
    if (!input){
        return -1;
    }

    // Read the data and store into RFData
    char tmp[100]; // Temporary location to store the info from input

    for (int i = 0; i<numElement; i++){ // Go through each element in array
        
        for (int j = 0; j<numSample; j++){ // Go through each sample in array
 
            if (!input.eof()){ // Check to see if there is more data to read
                input.getline(tmp, 99); // Store line in tmp array
                RFData[i][j] = atof(tmp); // Make input char array a float and store in RFData
            }
        }
    }

    // close ifstream
    input.close();
    return 0;

}

// Create an array containing the depth location (in z-direction) for each pixel on the scanline
float *genScanlineLocation(int &numPixel)
{
    // Take in and store the imaging depth from the user
    cout << "Please enter the imaging depth: " << endl;
    float imagedepth;
    cin >> imagedepth;

    // Take in and store the number of pixels from the user
    cout << "Please enter the number of pixels: " << endl;
    cin >> numPixel;

    // Create and set scanline location 1D array
    float *scanlineLocation = new float [numPixel];

    // Fill the scanline location array
    for (int i=0; i<numPixel; i++){
        scanlineLocation[i] = (i * (imagedepth / (numPixel-1)));
    }

    // Return the scanline location array
    return scanlineLocation;
}

// Create an array containing the element location (in x-direction) of the ultrasound transducer
float *genElementLocation(int numElement, float PITCH)
{
  
    // Create element location 1D array
    float *eleLocation = new float [numElement];

    // Set element location 1D array
    for (int i=0; i<numElement; i++){
        eleLocation[i] = ((i - ((float)numElement-1)/2) * PITCH);
    }

    // Return the element location array
    return eleLocation;
}

// Allocate memory to store the beamformed scanline
float *createScanline(int numPixel)
{
    float *scanline = new float [numPixel];
    return scanline;
}

// Beamform the A-mode scanline
void beamform(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel, float FS, float SoS)
{
    float tforward; // Forward time of flight
    float tbackward; // Backward time of flight
    float ttotal; // Total time of flight
    int s; // Sample

    for (int i=0; i<numPixel; i++){ // Go through each scanline position

        float Preal = 0; // Real part of the pulse signal
        float Pimag = 0; // Imaginary part of the pulse signal
        
        tforward = (scanlinePosition[i])/SoS; // Set tforward

        for (int k=0; k<numElement; k++){ // Go through each element

            // Set tbackward and ttotal
            tbackward = (sqrt(pow(scanlinePosition[i], 2) + pow(elementPosition[k], 2)) / SoS);
            ttotal = tforward + tbackward;

            // Determine sample
            s = floor(ttotal * FS);

            // Determine Preal and Pimag
            Preal += realRFData[k][s];
            Pimag += imagRFData[k][s];
            
        }

        // Set scanline[i] to the echo magnitude at the i scanline location
        scanline[i] = sqrt(pow(Preal, 2) + pow(Pimag, 2));
    }
}

// Write the scanline to a csv file
int outputScanline(const char *fileName, float *scanlinePosition, float *scanline, int numPixel)
{
    ofstream output(fileName);
    
    // Check to see if the output function failed
    if (!output){
        return -1;
    }

    // Output the calculated echo magnitude at each scanline location
    for (int i=0; i<numPixel; i++){
        output << scanlinePosition[i] << "," << scanline[i] << endl;
    }

    // Close file
    output.close();

    // Return 0 for function
    return 0;
}

// Destroy all the allocated memory
void destroyAllArrays(float *scanline, float **realRFData, float **imagRFData, float *scanlinePosition, float *elementPosition, int numElement, int numSample, int numPixel)
{
    // Release the memory allocated for scanline, scanlinePosition, elementPosition
    delete[] scanline;
    delete[] scanlinePosition;
    delete[] elementPosition;

    // Release the memory allocated for realRFData, imagRFData
    for (int i=0; i<numPixel; i++){
        delete[] realRFData[i];
        delete[] imagRFData[i];
    }
    delete[] realRFData;
    delete[] imagRFData;
}