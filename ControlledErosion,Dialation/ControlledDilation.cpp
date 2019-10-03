
//Program written by Anantha Natarajan
//Opengl part referenced from sample program 
//kernel is a black and white image
//The program dilates the pixels of our image that aligns with the black region of the control image
// kernel of size 5*5 is used

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

#define PI 3.14159265
#define maximum(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z))) 
#define minimum(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// =============================================================================
int width, height, n, wc, hc;
int wK, hK;
unsigned char *testmap;
unsigned char *controlmap2;
double *controlmap;
double *convolutedMap;
unsigned char *convMap;
double** kernel;
double** pixelMatrix;
double** controlMatrix;

/*class position {

	public:
	int xx;
	int yy;

	void setValue(int x, int y) {
		xx = x;
		yy = y;
	}

	void printValues() {
		cout << xx << " - " << yy;
	}

};
*/

//position *positionControlMatrix;


void RGBtoHSV(int r, int g, int b, double &h, double &s, double &v) {

	double red, green, blue;
	double max, min, delta;

	red = r / 255.0; green = g / 255.0; blue = b / 255.0;  /* r, g, b to 0 - 1 scale */

	max = maximum(red, green, blue);
	min = minimum(red, green, blue);

	v = max;        /* value is maximum of r, g, b */

	if (max == 0) {    /* saturation and hue 0 if value is 0 */
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;           /* saturation is color purity on scale 0 - 1 */

		delta = max - min;
		if (delta == 0)                    /* hue doesn't matter if saturation is 0 */
			h = 0;
		else {
			if (red == max)                  /* otherwise, determine hue on scale 0 - 360 */
				h = (green - blue) / delta;
			else if (green == max)
				h = 2.0 + (blue - red) / delta;
			else /* (blue == max) */
				h = 4.0 + (red - green) / delta;
			h = h * 60.0;
			if (h < 0)
				h = h + 360.0;
		}

	}

	//	if(r == 10 && g == 143 && b == 13)
	//	cout << endl << h << "   " << s << "   " << v << "  " << endl;
}


void readControlImage() {

	string token;
	ifstream filePPM("pooControl2.ppm", ios::binary);

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



		controlmap = new double[width * height * 3];
		controlmap2 = new unsigned char[width * height * 3];


		for (int y = height - 1; y >= 0; y--) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);

				int red = colors[0];
				int green = colors[1];
				int blue = colors[2];
			//	double h, s, v;

			//	RGBtoHSV(red, green, blue, h, s, v);

				//			cout << h << " " << s << " " << v << endl;
			//	if (v < 0) {
			//		v = 0;
			//	}
				controlmap2[i++] = red;
				controlmap2[i++] = green;
				controlmap2[i] = blue;

			}
		}

		filePPM.close();

	}

}

void readImage() {

	string token;
	ifstream filePPM("image.ppm", ios::binary);

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
			n = n - 2;
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
	for (int m = -1; m < hK - 1; m++) {
		for (int n = -1; n < wK - 1; n++) {
			//int k = m * wK + n;
			int u, v;
			u = m + 1;
			v = n + 1;
			kernel[u][v] = -((n*m) / rN);
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

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			kernel[m][n] = 0.23;
		}
	}

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

void intializeControlMatrix() {



	controlMatrix = new double*[hK];
	for (int i = 0; i < hK; ++i)
		controlMatrix[i] = new double[wK];

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			controlMatrix[m][n] = 0.0;
		}
	}

	/*positionControlMatrix = new position[hK*wK];

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			int k = m * wK + n;
			positionControlMatrix[k].setValue(0, 0);
		}
	}

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			int k = m * wK + n;
			positionControlMatrix[k].printValues();
			//cout << endl;
		}
	}*/

}

void flushControlMatrix() {
	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			controlMatrix[m][n] = 0.0;
			//int k = m * wK + n;
			//positionControlMatrix[k].setValue(0, 0);
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

void printControlMAtrix() {
	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {
			//int k = m * wK + n;
			cout << controlMatrix[m][n] << "  ";
		}
		cout << endl;
	}
}

void populateControlMatrix(int x, int y, int channel) {
	flushControlMatrix();
	int I = channel;

	if (x == 0 || y == 0 || x == width - 1 || y == height - 1) {
		//border edges
		if ((x == 0 && y == 0) || (x == width - 1 && y == height - 1) || (x == width - 1 && y == 0) || (x == 0 && y == height - 1)) {
			//corner
			if ((x == 0 && y == 0)) {
				I += (y * width + x) * 3;
				for (int m = 2; m < hK; m++) {
					for (int n = 2; n < wK; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == height - 1)) {
				I += ((y - 2) * width + (x - 2)) * 3;
				for (int m = 0; m < hK - 2; m++) {
					for (int n = 0; n < wK - 2; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == 0)) {
				I += ((y)* width + (x - 2)) * 3;
				for (int m = 2; m < hK; m++) {
					for (int n = 0; n < wK - 2; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 2)* width + (x)) * 3;
				for (int m = 0; m < hK - 2; m++) {
					for (int n = 2; n < wK; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}


		}
		else {
			//along the edge
			if (y == 0) {
				I += ((y)* width + (x - 2)) * 3;
				for (int m = 2; m < hK; m++) {
					for (int n = 0; n < wK; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - wK) * 3);
				}
			}
			else if (x == 0) {
				I += ((y - 2)* width + (x)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 2; n < wK; n++) {
						//int k = m * wK + n;
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if (x == width - 1) {
				I += ((y - 2)* width + (x - 2)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 0; n < wK - 1; n++) {
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 2)* width + (x - 2)) * 3;
				for (int m = 0; m < hK - 2; m++) {
					for (int n = 0; n < wK; n++) {
						controlMatrix[m][n] = controlmap2[I];
						I = I + 3;
					}
					I = I + ((width - wK) * 3);
				}
			}

		}
	}
	else {
		// Any other pixel inside the borders
		I += ((y - 2) * width + (x - 2)) * 3;
		for (int m = 0; m < hK; m++) {
			for (int n = 0; n < wK; n++) {
				//int k = m * wK + n;
				controlMatrix[m][n] = controlmap2[I];
				I = I + 3;
			}
			I = I + ((width - wK) * 3);
		}

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
				for (int m = 2; m < hK; m++) {
					for (int n = 2; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == height - 1)) {
				I += ((y - 2) * width + (x - 2)) * 3;
				for (int m = 0; m < hK - 2; m++) {
					for (int n = 0; n < wK - 2; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if ((x == width - 1 && y == 0)) {
				I += ((y)* width + (x - 2)) * 3;
				for (int m = 2; m < hK; m++) {
					for (int n = 0; n < wK - 1; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 2)* width + (x)) * 3;
				for (int m = 0; m < hK - 2; m++) {
					for (int n = 2; n < wK; n++) {
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
				I += ((y)* width + (x - 2)) * 3;
				for (int m = 2; m < hK; m++) {
					for (int n = 0; n < wK; n++) {
						//int k = m * wK + n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - wK) * 3);
				}

				if (x == width - 1) {
					cout << (double)testmap[1440] << endl;
				}
			}
			else if (x == 0) {
				I += ((y - 2)* width + (x)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 2; n < wK; n++) {
						//int k = m * wK + n;
						printKernel();
						cout << I << "-" << x << "-" << y << "-" << m << "-" << n;
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else if (x == width - 1) {
				I += ((y - 2)* width + (x - 2)) * 3;
				for (int m = 0; m < hK; m++) {
					for (int n = 0; n < wK - 2; n++) {
						pixelMatrix[m][n] = testmap[I];
						I = I + 3;
					}
					I = I + ((width - (wK - 1)) * 3);
				}
			}
			else {
				I += ((y - 2)* width + (x - 2)) * 3;
				for (int m = 0; m < hK - 2; m++) {
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
		I += ((y - 2) * width + (x - 2)) * 3;
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

double convolution(int x, int y,int def) {

	double sum = 0;
	int min = 999;
	int max = 0;
	for (int i = 0; i < hK; i++) {
		for (int j = 0; j < wK; j++) {
			if (controlMatrix[j][i] == 32) {
				if (pixelMatrix[j][i] > max) {
					max = pixelMatrix[j][i];
				}
			}

		}
	}

	if (min == 999) {
		min = def;
	}

	if (max == 0) {
		max = def;
	}
	//cout << sum << endl;

	return max;

}

void populateKernel(int x0, int y0) {
	//blurFilter();
	//dervativeFilter();
	int index = (y0 * width + x0) * 3;
	double theta = controlmap[index];
	theta = theta * 3.14 / 180;
	double a = cos(theta);
	double b = sin(theta);
	//cout << controlmap[index] << endl << sin(theta) << endl;

	for (int m = 0; m < hK; m++) {
		for (int n = 0; n < wK; n++) {



		}
	}


}

void applyFilter() {

	convolutedMap = new double[width * height * 3];
	convMap = new unsigned char[width * height * 3];

	for (int y = 2; y < height - 2; y++) {
		for (int x = 2; x < width - 2; x++) {
			int i = (y * width + x) * 3;
			int I = ((y - 2) * width + (x - 2)) * 3;

			int red = testmap[i++];
			double sum = 0;
			double Rf, Gf, Bf, Rftemp;


			populatePixelMatrix(x, y, 0);

			populateControlMatrix(x, y, 0);
			//populateKernel(x, y);
			Rf = convolution(x, y, red);
			if (Rf < 0) {
				Rf = -Rf;
			}
			convMap[I++] = Rf;

			int green = testmap[i++];
			populatePixelMatrix(x, y, 1);
			populateControlMatrix(x, y, 1);
			Gf = convolution(x, y, green);

			if (Gf < 0) {
				Gf = -Gf;
			}
			convMap[I++] = Gf;

			int blue = testmap[i];
			populatePixelMatrix(x, y, 2);
			populateControlMatrix(x, y, 2);
			Bf = convolution(x, y, blue);

			if (Bf < 0) {
				Bf = -Bf;
			}
			convMap[I] = Bf;
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
	readImage();
	readControlImage();

	intializePixelMatrix();
	intializeControlMatrix();
	createKernel();
	applyFilter();


	populatePixelMatrix(245, 245, 0);

	printPixelMAtrix();

	cout << "Control Matrix is" << endl;

	populateControlMatrix(200, 250, 0);

	int index = (245 * width + 245) * 3;

	printControlMAtrix();

	//populateKernel(245, 245);

	cout << endl;
	printKernel();
	int t = convolution(245, 245, testmap[index] );

	cout << endl << "The min value at 245 245 is " << t;

	//	populateControlMatrix(245, 245, 2);
	//	printControlMAtrix();


		//printKernel();


		//printKernel();

		/*

			for (int y = 0; y < 3; y++) {
				for (int x = 0; x < 3; x++) {
					int i = (y * 3 + x) * 3;
					cout << (double)controlmap[i++] << "-";
					cout << (double)controlmap[i++] << "-";
					cout << (double)controlmap[i] << "   ";
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
