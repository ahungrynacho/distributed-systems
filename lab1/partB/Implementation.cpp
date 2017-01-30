// Brian Huynh 57641580
#define MAX_PIXELS 1000
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
 
/* Global variables, Look at their usage in main() */
int image_height;
int image_width;
int image_maxShades;
int inputImage[MAX_PIXELS][MAX_PIXELS];
int outputImage[MAX_PIXELS][MAX_PIXELS];
int num_threads; 
int chunkSize;
int maxChunk;
bool * is_computed;
std::mutex mtx;

/* ****************Change and add functions below ***************** */
void detect_edges(int height_start, int height_end, int input[MAX_PIXELS][MAX_PIXELS], int output[MAX_PIXELS][MAX_PIXELS]) {

    int maskX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int maskY[3][3] = {{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
    for (int x = height_start; x < height_end; ++x) {
        for (int y = 0; y < image_width; ++y) {
            int sumX = 0;
            int sumY = 0;
            int sum = 0;

            /* For handling image boundaries. */
            if (x == 0 || x == (image_height - 1) || y == 0 || y == (image_width - 1))
                sum = 0;
            else {
                /* Gradient calculation in X Dimension. */
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j<= 1; j++) {
                        sumX += input[x+i][y+j] * maskX[i+1][j+1];
                    }
                }
                /* Gradient calculation in Y Dimension. */
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        sumY += input[x+i][y+j] * maskY[i+1][j+1];
                    }
                }
                sum = abs(sumX) + abs(sumY);
                if (sum >= 255)
                    sum = 255;
                else if (sum <= 0)
                    sum = 0;
            }
            output[x][y] = sum;
        }
    }
}

bool all_computed(bool is_computed[], int length) {
    for (int i = 0; i < length; ++i) {
        if (!is_computed[i])
            return false;
    }
    return true;
}
void start(int maxChunk, int start_indices[], int end_indices[]) {
    /* Function that std::thread() calls. */
    int index = -1;
    mtx.lock();
    for (int i = 0; i < maxChunk; ++i) {
        if (!is_computed[i]) {
            index = i;
            is_computed[i] = true;
            break;
        }
    }
    mtx.unlock();

    if (index != -1) {
        detect_edges(start_indices[index], end_indices[index], inputImage, outputImage);
    }

}


void dispatch_threads(int num_threads, int maxChunk, int start_indices[], int end_indices[]) {
    std::vector<std::thread> threads;

    while (!all_computed(is_computed, maxChunk)) {
        for (int i = 0; i < num_threads; ++i) {
            threads.push_back(std::thread(start, maxChunk, start_indices, end_indices));
        }
        for (int i = 0; i < num_threads; ++i) {
            threads[i].join();
        }
        threads.clear();
    }


}
void init_indices(int image_height, int maxChunk, int chunkSize, int start_indices[], int end_indices[])
{
    int lastChunkSize = image_height - ((maxChunk - 1) * chunkSize);
    int index = 0;
    for (int i = 0; i < maxChunk-1; ++i) {
        start_indices[i] = index;
        end_indices[i] = index + chunkSize;
        index += chunkSize;
    }
    start_indices[maxChunk-1] = index;
    end_indices[maxChunk-1] = index + lastChunkSize;

/* 
    for (int i = 0; i < maxChunk; ++i) {
        std::cout << start_indices[i] << " " << end_indices[i] << std::endl;
    }
*/  

}



/* ****************Need not to change the function below ***************** */

int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename> <Threads#> <Chunk size>" << std::endl;
        return 0;
    }
 
    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }
    num_threads = std::atoi(argv[3]);
    chunkSize  = std::atoi(argv[4]);

    std::cout << "Detect edges in " << argv[1] << " using " << num_threads << " threads\n" << std::endl;

    /* ******Reading image into 2-D array below******** */

    std::string workString;
    /* Remove comments '#' and check image format */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            if( workString.at(1) != '2' ){
                std::cout << "Input image is not a valid PGM image" << std::endl;
                return 0;
            } else {
                break;
            }       
        } else {
            continue;
        }
    }
    /* Check image size */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            std::stringstream stream(workString);
            int n;
            stream >> n;
            image_width = n;
            stream >> n;
            image_height = n;
            break;
        } else {
            continue;
        }
    }

    /* maxChunk is total number of chunks to process */
    maxChunk = ceil((float)image_height/chunkSize);

    /* Check image max shades */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            std::stringstream stream(workString);
            stream >> image_maxShades;
            break;
        } else {
            continue;
        }
    }
    /* Fill input image matrix */ 
    int pixel_val;
    for( int i = 0; i < image_height; i++ )
    {
        if( std::getline(file,workString) && workString.at(0) != '#' ){
            std::stringstream stream(workString);
            for( int j = 0; j < image_width; j++ ){
                if( !stream )
                    break;
                stream >> pixel_val;
                inputImage[i][j] = pixel_val;
            }
        } else {
            continue;
        }
    }

    /************ Function that creates threads and manage dynamic allocation of chunks *********/
    int start_indices[maxChunk];
    int end_indices[maxChunk];
    is_computed = new bool[maxChunk];

    // Initliaze array
    for (int i = 0; i < maxChunk; i++)
        is_computed[i] = false;

    init_indices(image_height, maxChunk, chunkSize, start_indices, end_indices);
    dispatch_threads(num_threads, maxChunk, start_indices, end_indices);
    delete[] is_computed;

    /* ********Start writing output to your file************ */
    std::ofstream ofile(argv[2]);
    if( ofile.is_open() )
    {
        ofile << "P2" << "\n" << image_width << " " << image_height << "\n" << image_maxShades << "\n";
        for( int i = 0; i < image_height; i++ )
        {
            for( int j = 0; j < image_width; j++ ){
                ofile << outputImage[i][j] << " ";
            }
            ofile << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
        return 0;
    }
    return 0;
}
