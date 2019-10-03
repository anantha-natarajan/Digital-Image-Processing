
//Program written by Anantha Natarajan
//Opengl part referenced from sample program 
//This program contains all the linear and affine transformations
//Program reads a PPM image and stores it in testmap
//Then the transformation is applied and the result is stored in transformMap
//At then end of every function the transformMap is copied to testmap so that other transformations can be applied
//Anti Aliasing is done with 3*3 matrix and random sample per sub pixel. That is 9 random samples for each pixel
//Transform matrix is stored as a 1D array and matrix multiplication is done

#include<windows.h>
#include <cstdlib>
#include <iostream>
#include <glut.h>


#include <fstream>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// =============================================================================
// These variables will store the generated ppm image's width, height, and color
// =============================================================================

int width, height;
unsigned char *testmap;
unsigned char *transformMap;


void readImage() {
	string token;
	ifstream filePPM("girl.ppm", ios::binary);

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
		transformMap = new unsigned char[width * height * 3];


		for (int y = height - 1; y >= 0; y--) {
			for (int x = 0; x < width; x++) {
				int i = (y * width + x) * 3;
				int j = i;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				testmap[i++] = colors[0];
				testmap[i++] = colors[1];
				testmap[i] = colors[2];

				transformMap[j++] = 255;
				transformMap[j++] = 255;
				transformMap[j] = 255;

			}
		}

		filePPM.close();

	}

}


void copy() {
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int j = i;
			int k = j;
			testmap[i++] = transformMap[j++];
			testmap[i++] = transformMap[j++];
			testmap[i] = transformMap[j];

			transformMap[k++] = 255;
			transformMap[k++] = 255;
			transformMap[k] = 255;

		}
	}

}


void scale() {

	//first form the transform matrix
	float t[9] = { 0,0,1,0,2,0,2,0,0 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;

			float xy[3] = { x,y,1 };
			float uv[3];

			for (int i = 0; i < 9; ) {
				for (int j = 2; j >= 0; j--) {
					uv[j] = 0;
					for (int k = 0; k < 3; k++) {
						uv[j] += t[i] * xy[k];
						i++;
					}
				}
			}

	/*		for (int i = 0; i < 3; i++) {
				cout << uv[i] << "";
			}
			*/
			int u, v;
			u = uv[0];
			v = uv[1];

			int flag = 1;

			while (u < 0)
			{
				flag = 0;
				u += width;
			}
			while (u > width - 1)
			{
				flag = 0;
				u -= width;
			}
			while (v < 0)
			{
				flag = 0;
				v += height;
			}
			while (v > height - 1)
			{
				flag = 0;
				v -= height;
			}

			if (flag != 0) {
				int index2 = (v * width + u) * 3;
				transformMap[index++] = testmap[index2++];
				transformMap[index++] = testmap[index2++];
				transformMap[index] = testmap[index2];
			}
		}
	}

	

//	cout << u << " " << v;

	copy();

}

void rotate() {

		float theta = -(15*3.14/180);

		//first form the transform matrix
		float t[9] = { 0,0,1,-(sin(theta)),cos(theta),0,cos(theta),sin(theta),0 };

		for (int b = 2; b >= 0; b--) {
			for (int a = 0; a < 3; a++) {
				int index = b * 3 + a;
				cout << t[index] << " ";
			}
			cout << endl;
		}

		for (int y = 0; y < height; y++) { // iterate through pixmap...
			for (int x = 0; x < width; x = x++) {
				int index = (y * width + x) * 3;
				int R, G, B;
				
				R = 0;
				G = 0;
				B = 0;

				for (double m = 0; m < 3; m = m++) {
					for (double n = 0; n < 3; n = n++) {
						
						double rand1 = rand() % 3;
						double rand2 = rand() % 3;

						rand1 /= 10;
						rand2 /= 10;


						double X = double(x) + (m / 3) + rand1;
						double Y = double(y) + (n / 3) + rand2;
						int Ra, Ga, Ba;

						float xy[3] = { X,Y,1 };
						float uv[3];

						for (int i = 0; i < 9; ) {
							for (int j = 2; j >= 0; j--) {
								uv[j] = 0;
								for (int k = 0; k < 3; k++) {
									uv[j] += t[i] * xy[k];
									i++;
								}
							}
						}

						/*		for (int i = 0; i < 3; i++) {
									cout << uv[i] << "";
								}
								*/

						int u, v;
						u = uv[0];
						v = uv[1];

						int flag = 1;

						while (u < 0)
						{
							flag = 0;
							u += width;
						}
						while (u > width - 1)
						{
							flag = 0;
							u -= width;
						}
						while (v < 0)
						{
							flag = 0;
							v += height;
						}
						while (v > height - 1)
						{
							flag = 0;
							v -= height;
						}

						if (flag != 0) {
							int index2 = (v * width + u) * 3;
							Ra = testmap[index2++];
							Ga = testmap[index2++];
							Ba = testmap[index2];
						}
						else {
							Ra = 255;
							Ga = 255;
							Ba = 255;
						}

						R = R + Ra / 9;
						G = G + Ga / 9;
						B = B + Ba / 9;

					}
				}

				


				transformMap[index++] = R;
				transformMap[index++] = G;
				transformMap[index] = B;
			}
		}

		copy();
}

void rotateNoAntialiased() {
	float theta = -(15 * 3.14 / 180);

	//first form the transform matrix
	float t[9] = { 0,0,1,-(sin(theta)),cos(theta),0,cos(theta),sin(theta),0 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;
			int R, G, B;

			R = 0;
			G = 0;
			B = 0;

					float xy[3] = { x,y,1 };
					float uv[3];

					for (int i = 0; i < 9; ) {
						for (int j = 2; j >= 0; j--) {
							uv[j] = 0;
							for (int k = 0; k < 3; k++) {
								uv[j] += t[i] * xy[k];
								i++;
							}
						}
					}

					/*		for (int i = 0; i < 3; i++) {
								cout << uv[i] << "";
							}
							*/

					int u, v;
					u = uv[0];
					v = uv[1];

					int flag = 1;

					while (u < 0)
					{
						flag = 0;
						u += width;
					}
					while (u > width - 1)
					{
						flag = 0;
						u -= width;
					}
					while (v < 0)
					{
						flag = 0;
						v += height;
					}
					while (v > height - 1)
					{
						flag = 0;
						v -= height;
					}

					if (flag != 0) {
						int index2 = (v * width + u) * 3;
						R = testmap[index2++];
						G = testmap[index2++];
						B = testmap[index2];
					}
					else {
						R = 255;
						G = 255;
						B = 255;
					}


			transformMap[index++] = R;
			transformMap[index++] = G;
			transformMap[index] = B;
		}
	}
	copy();
}

void shear() {

	//first form the transform matrix
	float t[9] = { 0,0,1,0,1,0,1,-0.25,0 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;
			int R, G, B;

			R = 0;
			G = 0;
			B = 0;

			for (double m = 0; m < 3; m = m++) {
				for (double n = 0; n < 3; n = n++) {

					double rand1 = rand() % 3;
					double rand2 = rand() % 3;

					rand1 /= 10;
					rand2 /= 10;


					double X = double(x) + (m / 3) + rand1;
					double Y = double(y) + (n / 3) + rand2;
					int Ra, Ga, Ba;

					float xy[3] = { X,Y,1 };
					float uv[3];

					for (int i = 0; i < 9; ) {
						for (int j = 2; j >= 0; j--) {
							uv[j] = 0;
							for (int k = 0; k < 3; k++) {
								uv[j] += t[i] * xy[k];
								i++;
							}
						}
					}

					/*		for (int i = 0; i < 3; i++) {
								cout << uv[i] << "";
							}
							*/

					int u, v;
					u = uv[0];
					v = uv[1];

					int flag = 1;

					while (u < 0)
					{
						flag = 0;
						u += width;
					}
					while (u > width - 1)
					{
						flag = 0;
						u -= width;
					}
					while (v < 0)
					{
						flag = 0;
						v += height;
					}
					while (v > height - 1)
					{
						flag = 0;
						v -= height;
					}

					if (flag != 0) {
						int index2 = (v * width + u) * 3;
						Ra = testmap[index2++];
						Ga = testmap[index2++];
						Ba = testmap[index2];
					}
					else {
						Ra = 255;
						Ga = 255;
						Ba = 255;
					}

					R = R + Ra / 9;
					G = G + Ga / 9;
					B = B + Ba / 9;

				}
			}




			transformMap[index++] = R;
			transformMap[index++] = G;
			transformMap[index] = B;
		}
	}

	copy();

}

void mirror() {

	//first form the transform matrix
	float t[9] = { 0,0,1,0,1,0,-1,0,0 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;

			float xy[3] = { x,y,1 };
			float uv[3];

			for (int i = 0; i < 9; ) {
				for (int j = 2; j >= 0; j--) {
					uv[j] = 0;
					for (int k = 0; k < 3; k++) {
						uv[j] += t[i] * xy[k];
						i++;
					}
				}
			}

			/*		for (int i = 0; i < 3; i++) {
						cout << uv[i] << "";
					}
					*/

			int u, v;
			u = uv[0];
			v = uv[1];

			while (u < 0)
			{
				u += width;
			}
			while (u > width - 1)
			{
				u -= width;
			}
			while (v < 0)
			{
				v += height;
			}
			while (v > height - 1)
			{
				v -= height;
			}

			int index2 = (v * width + u) * 3;
			transformMap[index++] = testmap[index2++];
			transformMap[index++] = testmap[index2++];
			transformMap[index] = testmap[index2];

		}
	}



	//	cout << u << " " << v;

	copy();

}

void translate() {

	//first form the transform matrix
	float t[9] = { 0,0,1,0,1,-50,1,0,-50 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;

			float xy[3] = { x,y,1 };
			float uv[3];

			for (int i = 0; i < 9; ) {
				for (int j = 2; j >= 0; j--) {
					uv[j] = 0;
					for (int k = 0; k < 3; k++) {
						uv[j] += t[i] * xy[k];
						i++;
					}
				}
			}

			/*		for (int i = 0; i < 3; i++) {
						cout << uv[i] << "";
					}
					*/

			int u, v;
			u = uv[0]/uv[2];
			v = uv[1]/uv[2];

			int flag = 1;

			while (u < 0)
			{
				flag = 0;
				u += width;
			}
			while (u > width - 1)
			{
				flag = 0;
				u -= width;
			}
			while (v < 0)
			{
				flag = 0;
				v += height;
			}
			while (v > height - 1)
			{
				flag = 0;
				v -= height;
			}

			if (flag != 0) {
				int index2 = (v * width + u) * 3;
				transformMap[index++] = testmap[index2++];
				transformMap[index++] = testmap[index2++];
				transformMap[index] = testmap[index2];

			}

		}
	}

	copy();

	//	cout << u << " " << v;



}

void perspective() {
	//first form the transform matrix
	float xp = -0.000833;
	float yp = -0.000833;

	cout << xp << endl << yp <<endl;

	float t[9] = { xp,yp,1,0,1,0,1,0,0 };

	for (int b = 2; b >= 0; b--) {
		for (int a = 0; a < 3; a++) {
			int index = b * 3 + a;
			cout << t[index] << " ";
		}
		cout << endl;
	}

	for (int y = 0; y < height; y++) { // iterate through pixmap...
		for (int x = 0; x < width; x = x++) {
			int index = (y * width + x) * 3;

			float xy[3] = { x,y,1 };
			float uv[3];

			for (int i = 0; i < 9; ) {
				for (int j = 2; j >= 0; j--) {
					uv[j] = 0;
					for (int k = 0; k < 3; k++) {
						uv[j] += t[i] * xy[k];
						i++;
					}
				}
			}

			/*		for (int i = 0; i < 3; i++) {
						cout << uv[i] << "";
					}
					*/

			int u, v;
			u = uv[0] / uv[2];
			v = uv[1] / uv[2];

			int flag = 1;

			while (u < 0)
			{
				flag = 0;
				u += width;
			}
			while (u > width - 1)
			{
				flag = 0;
				u -= width;
			}
			while (v < 0)
			{
				flag = 0;
				v += height;
			}
			while (v > height - 1)
			{
				flag = 0;
				v -= height;
			}

			flag = 1;

			if (flag != 0) {
				int index2 = (v * width + u) * 3;
				transformMap[index++] = testmap[index2++];
				transformMap[index++] = testmap[index2++];
				transformMap[index] = testmap[index2];
			}

		}
	}


	copy();
	//	cout << u << " " << v;



}


static void windowResize(int w, int h)
{
	glViewport(0, 0, w, h);
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

	scale();

	shear();

	translate();

	rotate();
	//rotateNoAntialiased();
	
	perspective();

	mirror();
	

	cout << endl << "Width of the PPM image is " << width << endl << "Height of the PPM image  is " << height << endl;
//	system("pause");
	
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutCreateWindow("Transformations ");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();
	
	return 0;
}
