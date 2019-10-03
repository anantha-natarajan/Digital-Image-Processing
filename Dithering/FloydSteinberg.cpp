//Program Written by Anantha Natarajan Santhana Srinivasan
//

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
unsigned char *image2;
unsigned char *outImage;


void readImage() {

	string token;
	ifstream filePPM("cat.ppm", ios::binary);

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


		for (int y = height - 1; y >= 0; y--) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				image1[i++] = colors[0];
				image1[i++] = colors[1];
				image1[i] = colors[2];
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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, outImage);
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
	outImage = new unsigned char[width * height * 3];

	for (int y = height-1; y > -1; y--) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int j = i;

			double oldR, oldG, oldB , newR, newG, newB, eR, eG, eB;

			oldR = image1[i++];
			oldG = image1[i++];
			oldB = image1[i];

			//Finding Old R,G,B values

			newR = (oldR > 96) ? 255 : 0;
			newG = (oldG > 96) ? 255 : 0;
			newB = (oldB > 96) ? 255 : 0;

			//Assigning new RGB values to either black or white

			outImage[j++] = newR;
			outImage[j++] = newG;
			outImage[j] = newB;

			eR = oldR - newR;
			eG = oldG - newG;
			eB = oldB - newB;

			//Finding error values of R, G, B
			//Propogating the error to nearby pixels

			int X, Y,I,J;
			X = x + 1;
			Y = y;

			int tr, tg, tb;

			if (((X > 0 || X==0) && (X < width - 1)) && ((Y>0||Y==0) && (Y < height - 1))) {

				I = (Y * width + X) * 3;
				J = I;
				image1[I++] = image1[J++] + (double)(7*eR/16);
				image1[I++] = image1[J++] + (double)(7 * eR / 16);
				image1[I] = image1[J] + (double)(7 * eR / 16);

			}

			X = x - 1;
			Y = y - 1;

			if (((X > 0 || X == 0) && (X < width - 1)) && ((Y > 0 || Y == 0) && (Y < height - 1))) {

				I = (Y * width + X) * 3;
				J = I;
				image1[I++] = image1[J++] + (double)(3 * eR / 16);
				image1[I++] = image1[J++] + (double)(3 * eR / 16);
				image1[I] = image1[J] + (double)(3 * eR / 16);

			}

			X = x ;
			Y = y - 1;

			if (((X > 0 || X == 0) && (X < width - 1)) && ((Y > 0 || Y == 0) && (Y < height - 1))) {

				I = (Y * width + X) * 3;
				J = I;
				image1[I++] = image1[J++] + (double)(5 * eR / 16);
				image1[I++] = image1[J++] + (double)(5 * eR / 16);
				image1[I] = image1[J] + (double)(5 * eR / 16);

			}

			X = x + 1;
			Y = y - 1;

			if (((X > 0 || X == 0) && (X < width - 1)) && ((Y > 0 || Y == 0) && (Y < height - 1))) {

				I = (Y * width + X) * 3;
				J = I;
				image1[I++] = image1[J++] + (double)(eR / 16);
				image1[I++] = image1[J++] + (double)(eR / 16);
				image1[I] = image1[J] + (double)(eR / 16);

			}
			
		}

	}


	/*fstream myfile;
	myfile.open("FlowerMixed.ppm", ios::out | ios::trunc);
	myfile << "P6 \n" << "# This file generated from Program2 \n" << width << " " << height << "\n" << "255 \n";

	int size = width * height * 3;
	for (int i = 0; i < size; i++) {
		myfile << (int)outImage[i] << " ";

		if ((i + 1) % (width * 3) == 0) {
			myfile << "\n";
		}
	}

	myfile.close();

	*/

	cout << endl << "Width of the PPM image is " << width << endl << "Height of the PPM image  is " << height << endl;

	//	system("pause");

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Floyd-Steinberg Dither");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();

	return 0;
}
