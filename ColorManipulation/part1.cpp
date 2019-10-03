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
int width2, height2, n2;
unsigned char *testmap;



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


// Piece wise linear interpolation function with r1,r2,r3,r4 as 0, 0.3, 0.6, 1 respectively

double interpolate(double C, int x, int y) {


	double x0, y0, x1, y1, x2, y2, x3, y3, C1;

	double r0, r1, r2, r3, r4;

	r0 = 0;
	r1 = 0.3;
	r2 = 0.6;
	r3 = 1;

	x0 = 0;
	y0 = 0;
	x1 = 1/3;
	y1 = r1;
	x2 = 2/3;
	y2 = r2;
	x3 = 1;
	y3 = 1;


	if (C >= x0 && C < x1) {
		C1 = ((x1 - x) * y0 + (x - x0) * y1) / (x1 - x0);
	}
	else if (C >= x1 && C < x2) {
		C1 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
	}
	else {
		C1 = ((x3 - x) * y2 + (x - x2) * y3) / (x3 - x2);
	}

	return C1;

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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, testmap);
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

	readImage();

	double C,C1;

	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;

			//Interpolating Red Colors
			// Change order of incrementing i and interpolating function to interpolate other color channels
		
			C = testmap[i] / 255;
			C1 = interpolate(C, x, y);
			testmap[i] = C1;

			i = i + 2;

		}
	}
	


	/*
	fstream myfile;
	myfile.open("FlowerMixed.ppm", ios::out | ios::trunc);
	myfile << "P6 \n" << "# This file generated from Program2 \n" << width << " " << height << "\n" << "255 \n";

	int size = width * height * 3;
	for (int i = 0; i < size; i++) {
		myfile << (int)mixedmap[i] << " ";

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
	glutCreateWindow("Image Generator");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}
