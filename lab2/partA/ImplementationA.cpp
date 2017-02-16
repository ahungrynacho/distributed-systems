#include "mpi.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

//***************** Add/Change the functions(including processImage) here ********************* 

// NOTE: Some arrays are linearized in the skeleton below to ease the MPI Data Communication 
// i.e. A[x][y] become A[x*total_number_of_columns + y]
int* processImage(int* inputImage, int processId, int num_processes, int image_height, int image_width){
     int x, y, sum, sumx, sumy;
     int GX[3][3], GY[3][3];
     /* 3x3 Sobel masks. */
     GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
     GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
     GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;
     
     GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
     GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
     GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;
	
	//chunkSize is the number of rows to be computed by this process
    //int height_start = processId * chunkSize;
    //int height_end = height_start + chunkSize;
    int* partialOutputImage = new int[image_height * image_width];
	
	for( x = 0; x < image_height; x++ ){
		 for( y = 0; y < image_width; y++ ){
			 sumx = 0;
			 sumy = 0;
			
			//Change boundary cases as required
			 if(x==0 || x==(image_height-1) || y==0 || y==(image_width-1))
				 sum = 0;
			 else{

				 for(int i=-1; i<=1; i++)  {
					 for(int j=-1; j<=1; j++){
						 sumx += (inputImage[(x+i)*image_width+ (y+j)] * GX[i+1][j+1]);
					 }
				 }

				 for(int i=-1; i<=1; i++)  {
					 for(int j=-1; j<=1; j++){
						 sumy += (inputImage[(x+i)*image_width+ (y+j)] * GY[i+1][j+1]);
					 }
				 }

				 sum = (abs(sumx) + abs(sumy));
			 }
			 partialOutputImage[x*image_width + y] = sum;
		 }
	}
	return partialOutputImage;
}

// dummy test function
int * paint(int * subimage, int image_height, int image_width) {
    int length = image_height * image_width;
    int * output = new int[length];

    for (int i = 0; i < length; ++i) {
        output[i] = 100;
    }
    return output;
}

int main(int argc, char* argv[])
{
	int processId, num_processes, image_height, image_width, image_maxShades;
	int *inputImage, *outputImage;
    int chunkSize, subheight, subwidth, remainder;
	
	// Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes);
	
    if(argc != 3)
    {
		if(processId == 0)
			std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename>" << std::endl;
		MPI_Finalize();
        return 0;
    }
	
	if(processId == 0)
	{
		std::ifstream file(argv[1]);
		if(!file.is_open())
		{
			std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
			MPI_Finalize();
			return 0;
		}

		std::cout << "Detect edges in " << argv[1] << " using " << num_processes << " processes\n" << std::endl;

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
		inputImage = new int[image_height*image_width];

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
					inputImage[(i*image_width)+j] = pixel_val;
				}
			} else {
				continue;
			}
		}

        for (int i = 1; i < num_processes; ++i) {
            MPI_Send(&image_height, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&image_width, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

	}
    else {
        MPI_Recv(&image_height, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&image_width, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
	
//	***************** Add code as per your requirement below ********************* 

    
    //int chunkSize = floor((double) (image_height * image_width)/num_processes);
    /*
    std::cout << image_height << std::endl;
    std::cout << num_processes << std::endl;
    std::cout << chunkSize << std::endl;
    */
    chunkSize = floor((double) (image_height * image_width) / num_processes);
    subheight = floor((double) (image_height / num_processes));
    remainder = (image_height * image_width) - (chunkSize * num_processes);

    int * recv_buf = new int[chunkSize];
    MPI_Scatter(inputImage, chunkSize, MPI_INT, recv_buf, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    int * subimage = processImage(recv_buf, processId, num_processes, subheight, image_width); // error found here
    //int * subimage = paint(recv_buf, 1, chunkSize);

    if (processId == 0) {
        std::cout << "chunkSize: " << chunkSize << std::endl;
        std::cout << "subheight: " << subheight << std::endl;
        std::cout << "image_width: " << image_width << std::endl;
        outputImage = new int[chunkSize * num_processes];
    }
    MPI_Gather(subimage, chunkSize, MPI_INT, outputImage, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (processId == 0)
	{
		/* Start writing output to your file */
        std::cout << "writing outputImage to pgm file" << std::endl;
		std::ofstream ofile(argv[2]);
		if( ofile.is_open() )
		{
			ofile << "P2" << "\n" << image_width << " " << image_height << "\n" << image_maxShades << "\n";
			for( int i = 0; i < image_height; i++ )
			{
				for( int j = 0; j < image_width; j++ ){
					ofile << outputImage[(i*image_width)+j] << " ";
				}
				ofile << "\n";
			}
		} else {
			std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
			return 0;
		}	
	}
	
    MPI_Finalize();
    return 0;
}
