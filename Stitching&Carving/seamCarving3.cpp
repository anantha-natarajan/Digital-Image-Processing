//Program Written by Anantha Natarajan Santhana Srinivasan

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
unsigned char *image1;
unsigned char *energyMatrix;
unsigned char *outImage;
unsigned char *carvedImage;
unsigned char *seamCarvedImage;
int *seam;

void readImage() {

	string token;
	ifstream filePPM("beach.ppm", ios::binary);

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



		image1 = new unsigned char[width * height * 3];
		carvedImage = new unsigned char[width * height * 3];

		for (int y = height - 1; y >= 0; y--) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				image1[i++] = colors[0];
				image1[i++] = colors[1];
				image1[i] = colors[2];
				i = (y * width + x) * 3;
				carvedImage[i++] = colors[0];
				carvedImage[i++] = colors[1];
				carvedImage[i] = colors[2];

			}
		}

		filePPM.close();

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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, image1);
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

int findMinimum(int x, int y) {
	double E1, E2, E3, Emin;
	int X, Y, I, xmin;

	//cout << x - 1 << "  " << x << "  " << x + 1 << endl;
	//cout << y << endl;

	X = x - 1;
	Y = y;

	if (X < 0) {
		X = width - 1;
	}

	I = (Y*width + X) * 3;
	E1 = energyMatrix[I];

	//cout << E1 << "  ";

	X = x;
	Y = y;

	I = (Y*width + X) * 3;
	E2 = energyMatrix[I];

	//	cout << E2 << "  ";

	X = x + 1;
	Y = y;

	if (X > width - 1) {
		X = 0;
	}

	I = (Y*width + X) * 3;
	E3 = energyMatrix[I];

	//	cout << E3 << "  " << endl;

	Emin = min(min(E1, E2), E3);

	if (Emin == E1) {
		xmin = x - 1;
	}
	else if (Emin == E2) {
		xmin = x;
	}
	else {
		xmin = x + 1;
	}




	return xmin;

}

void findSeam() {

	int Y = 0;
	int min = 0;
	int xmin = 0;
	for (int x = 0; x < width; x++) {
		int I = (Y * width + x) * 3;
		double temp = energyMatrix[I];
		if (temp == 255) {
			continue;
		}
		//	cout << temp << endl;
		if (temp < min || temp == min) {
			min = temp;
			xmin = x;
		}
	}

	seam[0] = xmin;
	int X = xmin;

	for (int Y = 1; Y < height; Y++) {
		X = findMinimum(X, Y);
		seam[Y] = X;
	}
}

void ReduceEnergyMatrix() {

	for (int y = 0; y < height; y++) {
		int X = seam[y];
		for (int x = 0; x < width; x++) {
			if (x == X) {
				int i = (y * width + x) * 3;
				energyMatrix[i++] = 255;
				energyMatrix[i++] = 0;
				energyMatrix[i] = 0;
			}
		}
	}


	for (int y = 0; y < height; y++) {

		int seamX = seam[y];
		for (int x = 0, X = 0; x < width && X < width; x++, X++) {
			int i = (y * width + x) * 3;
			int l = (y * width + X) * 3;
			int k = i;
			double t1, t2, t3;

			if (x == seamX) {
				x++;
				k = (y * width + x) * 3;
			}

			t1 = energyMatrix[k++];
			t2 = energyMatrix[k++];
			t3 = energyMatrix[k];
			energyMatrix[l++] = t1;
			energyMatrix[l++] = t2;
			energyMatrix[l] = t3;
		}
	}

}

void ReduceImage() {

	for (int y = 0; y < height; y++) {

		int seamX = seam[y];
		for (int x = 0, X = 0; x < width && X < width; x++, X++) {
			int i = (y * width + x) * 3;
			int l = (y * width + X) * 3;
			int k = i;
			double t1, t2, t3;

			if (x == seamX) {
				x++;
				k = (y * width + x) * 3;
			}

			t1 = carvedImage[k++];
			t2 = carvedImage[k++];
			t3 = carvedImage[k];
			carvedImage[l++] = t1;
			carvedImage[l++] = t2;
			carvedImage[l] = t3;
		}
	}
}

void formSeamCarvedImage() {
	for (int y = 0; y < height; y++) {
		int X = seam[y];
		for (int x = 0; x < width; x++) {

			if (x == X) {
				int i = (y * width + x) * 3;
				seamCarvedImage[i++] = 255;
				seamCarvedImage[i++] = 0;
				seamCarvedImage[i] = 0;
			}
			else {

				int I = (y * width + x) * 3;
				double temp = seamCarvedImage[I];
				if (temp == 255) {
					continue;
				}

				int i = (y * width + x) * 3;
				int j = i;
				seamCarvedImage[i++] = image1[j++];
				seamCarvedImage[i++] = image1[j++];
				seamCarvedImage[i] = image1[j];
			}
		}
	}
}

int main(int argc, char *argv[])
{

	readImage();
	outImage = new unsigned char[width * height * 3];
	energyMatrix = new unsigned char[width * height * 3];
	seamCarvedImage = new unsigned char[width * height * 3];
	seam = new int[height - 1];

	int  k[3][3] = { { -2,0,-2},
						{0,0,0},
					  {2,0,2} };


	for (int n = 0; n < 3; n++) {
		for (int m = 0; m < 3; m++) {
			cout << k[m][n] << " ";
		}
		cout << endl;
	}

	//Applying sobel filter

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			//	x = 250;
			//	y = x;

			int i = (y * width + x) * 3;
			int j = i;
			int sumR;
			sumR = 0;
			//	cout << "the value at 250 250 is" << (double)image1[j++]<< endl;
			for (int n = 0; n < 3; n++) {
				for (int m = 0; m < 3; m++) {
					int X, Y;
					X = x + (m - 1);
					Y = y + (n - 1);
					if (((X > 0 || X == 0) && (X < width)) && ((Y > 0 || Y == 0) && (Y < height))) {
						int I = (Y * width + X) * 3;
						sumR = sumR + (k[m][n] * image1[I++]);

					}

				}
			}

			if (sumR < 0) {
				sumR = 0;
			}

			if (sumR > 255) {
				sumR = 255;
			}

			//	cout << "the value after is" << sum;
			outImage[i++] = sumR;
			outImage[i++] = sumR;
			outImage[i] = sumR;

		}

	}

	//Cumulative energies matrix
	int Y = height - 1;
	for (int x = 0; x < width; x++) {
		int I = (Y * width + x) * 3;
		int J = I;
		energyMatrix[I++] = outImage[J++];
		energyMatrix[I++] = outImage[J++];
		energyMatrix[I] = outImage[J];
	}

	for (int y = height - 2; y > -1; y--) {
		for (int x = 0; x < width; x++) {

			int I = (y * width + x) * 3;
			double E1, E2, E3, E, Emin;
			E = outImage[I];

			int X, Y;
			X = x - 1;
			Y = y + 1;

			if (X < 0) {
				X = width - 1;
			}

			I = (Y*width + X) * 3;
			E1 = outImage[I];

			X = x;
			Y = y + 1;

			I = (Y*width + X) * 3;
			E2 = outImage[I];

			X = x + 1;
			Y = y + 1;

			if (X > width - 1) {
				X = 0;
			}

			I = (Y*width + X) * 3;
			E3 = outImage[I];

			Emin = min(min(E1, E2), E3);

			E = E + Emin;

			I = (y * width + x) * 3;
			//	cout << E << "-" << x << "-" << y << endl;
			energyMatrix[I++] = E;
			energyMatrix[I++] = E;
			energyMatrix[I] = E;

		}
	}

	//100 vertical seams are to be generated
	for (int i = 0; i < 100; i++) {
		//Backtracking now
		findSeam();
		//Marking the seams in Energy image and reducing width of energy image
		ReduceEnergyMatrix();
		//Reducing the original image by removing the seams 
		ReduceImage();
		//Forming an output image in original size with seams higlighted in RED
		formSeamCarvedImage();
	}

	cout << "----------------------------------------------";

	//for (int i = 0; i < height; i++) {
	//	cout << seam[i] << endl;
	//}

	cout << endl << "Width of the PPM image is " << width << endl << "Height of the PPM image  is " << height << endl;

	system("pause");

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	//change width according to the number of seams
	//To see the image in original size remove '-100'
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Seam Carving");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}
