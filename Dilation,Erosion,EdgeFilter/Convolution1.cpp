
//Program written by Anantha Natarajan
//Opengl part referenced from sample program 

#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <glut.h>
#include <math.h>
#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <cmath>


using namespace std;

#define maximum(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z))) 
#define minimum(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
int width, height, n;
int wK, hK;
unsigned char *testmap;
double *convolutedMap;
unsigned char *convMap;
double** kernel;
double** pixelMatrix;



void readImage() {

	string token;
	ifstream filePPM("lion.ppm", ios::binary);

	if (filePPM.is_open())
	{

		filePPM >> token;
		if (token != "P6") {
			cout << "Incorrect file!\n";
			filePPM.close();
			system("pause");
		}

		filePPM >> token;
		width = atoi(token.c_str());

		filePPM >> token;
		height = atoi(token.c_str());
		filePPM >> token;



		testmap = new unsigned char[width * height * 3];


		for (int y = height - 1; y >= 0; y--) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				testmap[i++] = colors[0];
				testmap[i++] = colors[1];
				testmap[i] = colors[2];
			}
		}

		filePPM.close();

	}

}


void makePPM() {

	width = 5;
	height = 5;
	n = 1;
	testmap = new unsigned char[75];
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			testmap[i++] = n++;
			testmap[i++] = n++;
			testmap[i] = n++;
			n = n-2;
		}
	}

}

void printPPM() {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			cout << (int)testmap[i++] << "-";
			cout << (int)testmap[i++] << "-";
			cout << (int)testmap[i] << "  ";
			
		}
		cout << endl;
	}
}

void blurFilter() {

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			kernel[m][n] = rand() % 255 + 1;
			//kernel[m][n] = 0.11;
		}
	}

}

void dervativeFilter() {

	double rN = 0.5;
	for (int m = -1; m < hK-1; m++) {
		for (int n = -1; n < wK-1; n++) {
			//int k = m * wK + n;
			int u, v;
			u = m + 1;
			v = n + 1;
			kernel[u][v] = -((n*m)/rN);
			//kernel[m][n] = 0.11;
		}
	}

}

void dervativeFilterFromFile() {
	string token;
	ifstream filePPM("kernelFilter.txt");

	if (filePPM.is_open())
	{
		for (int y = 0; y <= 3; y++) {
			for (int x = 0; x < 3; x++) {
				filePPM >> token;
				kernel[x][y] = atoi(token.c_str());
			}
		}

	}
}

void createKernel() {
	kernel = new double*[hK];
	for (int i = 0; i < hK; ++i)
		kernel[i] = new double[wK];

	blurFilter();
	//dervativeFilter();

}

void printKernel() {
	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			cout << kernel[m][n] << "  ";
		}
		cout << endl;
	}
}

void intializePixelMatrix() {
	pixelMatrix = new double*[hK];
	for (int i = 0; i < hK; ++i)
		pixelMatrix[i] = new double[wK];

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			pixelMatrix[m][n] = 0.0;
		}
	}
}

void flushPixelMatrix() {
	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			pixelMatrix[m][n] = 0.0;
		}
	}
}

void printPixelMAtrix() {
	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			cout << pixelMatrix[m][n] << "  ";
		}
		cout << endl;
	}
}

void populatePixelMatrix(int x, int y, int channel) {
	flushPixelMatrix();
	int I = channel;

	if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
		//border edges
		if ((x == 0 && y == 0) || (x == width - 1 && y == height - 1) || (x == width - 1 && y == 0) || (x == 0 && y == height - 1)) {
			//corner
			if ((x == 0 && y == 0)) {
				I += (y * width + x) * 3;
				for (int m = 1; m < hK; m++) {
					for (int n = 1; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == height - 1)) {
				I += ((y - 1) * width + (x - 1)) * 3;
				for (int m = 0; m < hK - 1; m++) {
					for (int n = 0; n < wK - 1; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == 0)) {
				I += ((y)* width + (x - 1)) * 3;
				for (int m = 1; m < hK; m++) {
					for (int n = 0; n < wK - 1; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 1)* width + (x)) * 3;
				for (int m = 0; m < hK - 1; m++) {
					for (int n = 1; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}


		}
		else {
			//along the edge
			if (y == 0) {
				I += ((y)* width + (x - 1)) * 3;
				for (int m = 1; m < hK; m++) {
					for (int n = 0; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - wK) * 3);
				}
			}
			else if (x == 0) {
				I += ((y - 1)* width + (x)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 1; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if (x == width - 1) {
				I += ((y - 1)* width + (x - 1)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 0; n < wK - 1; n++) {
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 1)* width + (x - 1)) * 3;
				for (int m = 0; m < hK - 1; m++) {
					for (int n = 0; n < wK; n++) {
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - wK) * 3);
				}
			}

		}
	}
	else {
		// Any other pixel inside the borders
		 I += ((y - 1) * width + (x - 1)) * 3;
		for (int m = 0; m < hK; m++) {
			for (int n = 0; n < wK; n++) {
				//int k = m * wK + n;
				pixelMatrix[m][n] = testmap[I];
				I = I + 3;
			}
			I = I + ((width - wK) * 3);
		}

	}

}

double convolution(int x, int y) {

	double sum = 0;
	double ksum = 0;
	for (int i = 0; i < hK; i++) {
		for (int j = 0; j < wK; j++) {
			ksum += kernel[j][i];
			sum = sum + kernel[j][i] * pixelMatrix[j][i];
		}
	}

	if (ksum != 0) {
		sum = sum / ksum;
	}

//	cout << sum << endl;

	return sum;

}

double convolution(int x, int y, double Color) {

	double sum = 0;
	double ksum = 0;
	for (int i = 0; i < hK; i++) {
		for (int j = 0; j < wK; j++) {
			ksum += kernel[j][i];
			sum = sum + kernel[j][i] * Color;
		}
	}

	if (ksum != 0) {
		sum = sum / ksum;
	}

	//	cout << sum << endl;

	return sum;

}

void applyFilter() {

	convolutedMap = new double[width * height * 3];
	convMap = new unsigned char[width * height * 3];
	
	for (int y =0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int I = ((y-1) * width + (x-1)) * 3;

			double c = testmap[i] / 255;
			double sum = 0;
			double Rf, Gf, Bf, Rftemp;
			populatePixelMatrix(x, y, 0);
			Rftemp = convolution(x, y);
//			cout << Rftemp << endl;
			if (Rftemp < 0) {
				Rf = -Rftemp;
			}
			else {
				Rf = Rftemp;
			}
//			cout << Rf << endl;
			convolutedMap[i] = Rf;
			convMap[i++] = Rf;

			populatePixelMatrix(x, y, 1);
			Gf = convolution(x, y);

			if (Gf < 0) {
				Gf = -Gf;
			}
			
			convolutedMap[i] = Gf;
			convMap[i++] = Gf;


			populatePixelMatrix(x, y, 2);
			Bf = convolution(x, y);

			if (Bf < 0) {
				Bf = -Bf;
			}

			convolutedMap[i] = Bf;
			convMap[i] = Bf;
			//i = i + 2;

		}
	}
	

}


static void windowResize(int w, int h)
{
	glViewport(-w, h, -w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (w / 2), 0, (h / 2), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
static void windowDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, convMap);
	glFlush();
}
static void processMouse(int button, int state, int x, int y)
{
	if (state == GLUT_UP)
		exit(0);
}
static void init(void)
{
	glClearColor(1, 1, 1, 1);
}

int main(int argc, char *argv[])
{
	wK = 5;
	hK = 5;
//	kernel = new double[wK*hK];
	readImage();
	createKernel();
	printKernel();
	intializePixelMatrix();
	applyFilter();
	cout << "testing out derivative";
//	dervativeFilter();
	dervativeFilterFromFile();
	printKernel();


/*
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			cout << (double)convolutedMap[i++] << "-";
			cout << (double)convolutedMap[i++] << "-";
			cout << (double)convolutedMap[i] << "   ";
		}
		cout << endl;
	}

	*/

	
	/*fstream myfile;
	myfile.open("lion3.ppm", ios::out | ios::trunc);
	myfile << "P6 \n" << "# This file generated from Program2 \n" << width << " " << height << "\n" << "255 \n";

	int size = width * height * 3;
	for (int i = 0; i < size; i++) {
		//cout << convMap[i];
		myfile << convMap[i] << " ";

		if ((i + 1) % (width * 3) == 0) {
			myfile << "\n";
		}
	}
	myfile.close();
	*/
	


	cout << endl << "Width of the PPM image is " << width << endl << "Height of the PPM image  is " << height << endl;

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Blurred Image");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();
	return 0;
}
