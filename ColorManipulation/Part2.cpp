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
unsigned char *controlmap;
unsigned char *mixedmap;


void readImage() {

	string token;
	ifstream filePPM("poo.ppm", ios::binary);

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


		for (int y = height-1; y >= 0; y--) {
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

void readControlImage() {

	string token;
	ifstream filePPM("b.ppm", ios::binary);

	if (filePPM.is_open())
	{

		filePPM >> token;
		if (token != "P6") {
			cout << "Incorrect file!\n";
			filePPM.close();
			system("pause");
		}

		filePPM >> token;
		width2 = atoi(token.c_str());

		filePPM >> token;
		height2 = atoi(token.c_str());
		filePPM >> token;



		controlmap = new unsigned char[width2 * height2 * 3];


		for (int y = height2 - 1; y >= 0; y--) {
			for (int x = 0; x < width2; x++) {
				int i = (y * width2 + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				controlmap[i++] = colors[0];
				controlmap[i++] = colors[1];
				controlmap[i] = colors[2];
			}
		}

		filePPM.close();

	}

}

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
	else{
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

void HSVtoRGB(double h, double s, double v, double &rt, double &gt, double &bt) {

	double r, g, b;
	double i, fct, p, q, t, tmp;

	if (s == 0) {
		r = v;
		g = v;
		b = v;
	}

	tmp = h/60;
	i = floor(tmp);
//	cout << i <<endl;
	fct = tmp - i;
//	cout << fct;
	p = v * (1 - s);
	q = v * (1 - s * fct);
	t = v * (1 - s * (1 - fct));

	if (h >= 0 && h < 60) {
		r = v;
		g = t;
		b = p;
	} else if(h >= 60 && h < 120) {
		r = q;
		g = v;
		b = p;
	} else if (h >= 120 && h < 180) {
		r = p;
		g = v;
		b = t;
	}
	else if (h >= 180 && h < 240) {
		r = p;
		g = q;
		b = v;
	}
	else if (h >= 240 && h < 300) {
		r = t;
		g = p;
		b = v;
	}
	else {
		r = v;
		g = p;
		b = q;
	}
	rt = r * 255;
	gt = g * 255;
	bt = b * 255;

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
	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, mixedmap);
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
	readControlImage();
	mixedmap = new unsigned char[width * height * 3];
	double *hsvControl, *hsvTest;
	hsvControl = new double[width2 * height2 * 3];
	hsvTest = new double[width2 * height2 * 3];
	
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int j = (y * width + x) * 3;

			int red = controlmap[i++];
			int green = controlmap[i++];
			int blue = controlmap[i++];
			double h, s, v;

			RGBtoHSV( red, green, blue, h,s,v);

			hsvControl[j++] = h;
			hsvControl[j++] = s;
			hsvControl[j] = v;

			i = (y * width + x) * 3;
			j = (y * width + x) * 3;

			red = testmap[i++];
			green = testmap[i++];
			blue = testmap[i++];

			double h1, s1, v1;

			RGBtoHSV(red, green, blue, h1, s1, v1);

			hsvTest[j++] = h1;
			hsvTest[j++] = s1;
			hsvTest[j] = v1;

			i = (y * width + x) * 3;
			j = (y * width + x) * 3;

			double rm, gm, bm , hue, sat, value ;
			hue = hsvControl[i++];
			sat = hsvControl[i++];
			value = hsvTest[i];
			HSVtoRGB(hue, sat, value, rm , gm , bm);

			mixedmap[j++] = rm;
			mixedmap[j++] = gm;
			mixedmap[j] = bm;

		}
	}
	

	cout << endl << (int)controlmap[0] << "---" << (int)controlmap[1] << "---" << (int)controlmap[2] << endl;
	cout << hsvControl[0] << "---" << hsvControl[1] << "---" << hsvControl[2] << endl;
	cout << "folowing" << endl;
	cout << endl << (int)testmap[0] << "---" << (int)testmap[1] << "---" << (int)testmap[2] << endl;
	cout << hsvTest[0] << "---" << hsvTest[1] << "---" << hsvTest[2] << endl;

	double h2 = 121.353;
	double s2 = 0.230769;
	double v2 = 0.0509804;
	double rt, gt, bt;
	HSVtoRGB(h2, s2, v2, rt, gt, bt);

	cout << endl << rt << "----" << gt << "----" << bt << endl;

	cout << "folowing" << endl;
//	cout << endl << (int)mixedmap[0] << "---" << (int)mixedmap[1] << "---" << (int)mixedmap[2] << endl;
	
	//double h2 = 0;
	//double s2 = 0.230769;
	//double v2 = 0.0509804;
	//double h2 = 121.353;
	//double s2 = 0.93007;
	//double v2 = 0.560784;
	//HSVtoRGB(h2,s2,v2);



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
