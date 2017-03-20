#define MAX_PIXELS 1000
#define NUM_THREADS 4
#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
 
/* Global variables, Look at their usage in main() */
int image_height;
int image_width;
int image_maxShades;
int inputImage[MAX_PIXELS][MAX_PIXELS];
int outputImage[MAX_PIXELS][MAX_PIXELS];
int chunkSize;
int maxChunk;

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

void init_indices(int image_height, int maxChunk, int chunkSize, int start_indices[], int end_indices[])
{
    int lastChunkSize = image_height - ((maxChunk - 1) * chunkSize);
    int index = 0;
    for (int i = 0; i < maxChunk - 1; ++i)
    {
        start_indices[i] = index;
        end_indices[i] = index + chunkSize;
        index += chunkSize;
    }
    start_indices[maxChunk-1] = index;
    end_indices[maxChunk-1] = index + lastChunkSize;
}

void output(int start_indices[], int end_indices[], int thread_id[], int maxChunk)
{

    printf("%-15s %-15s %-15s\n", "thread_id", "start_index", "end_index");
    for (int i = 0; i < maxChunk; ++i)
    {
        printf("%-15d %-15d %-15d\n", thread_id[i], start_indices[i], end_indices[i]);
    }
}

int find_chunk(bool is_computed[], int maxChunk)
{
    for (int i = 0; i < maxChunk; ++i)
    {
        if (!is_computed[i])
        {
            is_computed[i] = true;
            return i;
        }
    }
    return -1;
}

void compute_sobel_static(int start_indices[], int end_indices[], int thread_id[])
{
    #pragma omp parallel 
    {
        #pragma omp for schedule(static, 1)
        for (int i = 0; i < maxChunk; ++i)
        {
            thread_id[i] = omp_get_thread_num();
            detect_edges(start_indices[i], end_indices[i], inputImage, outputImage);
        }
        
    }

}




void compute_sobel_dynamic(int start_indices[], int end_indices[], bool is_computed[], int thread_id[])
{
    #pragma omp parallel
    {


        #pragma omp for schedule(dynamic, 1)
        for (int i = 0; i < maxChunk; ++i)
        {

            int index = -1;
            #pragma omp critical
            {
                index = find_chunk(is_computed, maxChunk);
            }

            if (index != -1)
            {
                thread_id[index] = omp_get_thread_num();
                detect_edges(start_indices[index], end_indices[index], inputImage, outputImage);
            }
        }
    }
}


/* **************** Change the function below if you need to ***************** */

int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename> <Chunk size> <a1/a2>" << std::endl;
        return 0;
    }
 
    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }
    chunkSize  = std::atoi(argv[3]);

    std::cout << "Detect edges in " << argv[1] << " using OpenMP threads\n" << std::endl;

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
    maxChunk = ceil((double) image_height/chunkSize);

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

    /************ Call functions to process image *********/
    std::string opt = argv[4];
    omp_set_num_threads(NUM_THREADS);

    int start_indices[maxChunk];
    int end_indices[maxChunk];
    int thread_id[maxChunk];
    bool is_computed[maxChunk];
    
    for (int i = 0; i < maxChunk; ++i)
        is_computed[i] = false;

    init_indices(image_height, maxChunk, chunkSize, start_indices, end_indices);

    printf("THREADS: %d CHUNKSIZE: %d\n", NUM_THREADS, chunkSize);
    if( !opt.compare("a1") )
    {   
        double dtime_static = omp_get_wtime();
        compute_sobel_static(start_indices, end_indices, thread_id);
        dtime_static = omp_get_wtime() - dtime_static;
        printf("LAB3 STATIC TIME: %f\n", dtime_static);
    } else {
        double dtime_dyn = omp_get_wtime();
        compute_sobel_dynamic(start_indices, end_indices, is_computed, thread_id);
        dtime_dyn = omp_get_wtime() - dtime_dyn;
        printf("LAB3 DYNAMIC TIME: %f\n", dtime_dyn);
    }
    output(start_indices, end_indices, thread_id, maxChunk);
    std::cout << "---------------------------------------------------------------" << std::endl;

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
