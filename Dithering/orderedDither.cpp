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
unsigned char *image2;
unsigned char *outImage;


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

int main(int argc, char *argv[])
{

	readImage();
	outImage = new unsigned char[width * height * 3];

	int  dith[3][3] = { { 28,255, 57},
						{142,113,227},
					  {170,198, 85} };

	//Converting the image to grayscale

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int k = (y * width + x) * 3;
			int l = k;

			double oldR, oldG, oldB, grayV;

			oldR = image1[k++];
			oldG = image1[k++];
			oldB = image1[k];

			grayV = (0.299*oldR) + (0.587*oldG) + (0.114*oldB);

			image1[l++] = grayV;
			image1[l++] = grayV;
			image1[l] = grayV;

		}
	}


	//Converting the grayscale values to a scale of 0-16
	//Finding corresponding pixel in threshold matrix
	//If the color val of pixel is less than threshold assign black else white

	for (int y = 0; y < height -3; y = y+3) {
		for (int x = 0; x < width -3; x = x+3) {

			for (int n = 0; n < 3; n++) {

				for (int m = 0; m < 3; m++) {

					int index = ((y + n)*width + (x + m)) * 3;

					int gray = image1[index];

					int newgray;

					if (gray > dith[n][m]) {
						newgray = 255;
					}
					else {
						newgray = 0;
					}

					image1[index++] = newgray;
					image1[index++] = newgray;
					image1[index] = newgray;


				}

			}

			//			break;

		}
		//		break;
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
