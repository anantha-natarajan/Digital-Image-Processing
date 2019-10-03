//Program Written by Anantha Natarajan Santhana Srinivasan
//Blend modes implemented are as follows
//NormalBlend Mode using Transparency
//Multiply
//Subtract
//Add
//Maximum
//Minimum
//Results are compared with photoshop blend modes

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
unsigned char *image1;
unsigned char *image2;
unsigned char *outImage;


void readImage() {

	string token;
	ifstream filePPM("orange.ppm", ios::binary);

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

void readControlImage() {

	string token;
	ifstream filePPM("blue.ppm", ios::binary);

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



		image2 = new unsigned char[width2 * height2 * 3];


		for (int y = height2 - 1; y >= 0; y--) {
			for (int x = 0; x < width2; x++) {
				int i = (y * width2 + x) * 3;
				char colors[3];
				filePPM.read(colors, sizeof colors);
				image2[i++] = colors[0];
				image2[i++] = colors[1];
				image2[i] = colors[2];
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

void HSVtoRGB(double h, double s, double v, double &rt, double &gt, double &bt) {

	double r, g, b;
	double i, fct, p, q, t, tmp;

	if (s == 0) {
		r = v;
		g = v;
		b = v;
	}

	tmp = h / 60;
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
	}
	else if (h >= 60 && h < 120) {
		r = q;
		g = v;
		b = p;
	}
	else if (h >= 120 && h < 180) {
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


void RGBtoHSL(int R, int G, int B, double &H, double &S, double &L) {

	double temp, maxVal, minVal,r,g,b;

	r = (double)R / 255.0;
	g = (double)G / 255.0;
	b = (double)B / 255.0;

	temp = min(r, g);
	minVal = min(temp, b);
	temp = max(r, g);
	maxVal = max(temp, b);

	L= ((minVal + maxVal) / 2);

	if (maxVal == minVal) {
		S = 0;
		H = 0;
	}
	else {
	//	cout << L << endl;
		if (L < 0.5) {
			S = (maxVal - minVal) / (maxVal + minVal);
			//cout << "Im here";
		}
		else {
			S = (maxVal - minVal) / (2.0 - maxVal - minVal);
		}

		if (maxVal == r) {
			H = ((g - b) / (maxVal - minVal));
		} 
		if (maxVal == g) {
			H = 2.0 + ((b - r) / (maxVal - minVal));
		}
		if (maxVal == b) {
			H = 4.0 + ((r - g) / (maxVal - minVal));
		}
	}

	L = L * 100;
	S = S * 100;
	H = H * 60;

}

double check(double color, double t1, double t2) {
	double out;
	if ((6 * color) < 1) {
		out = t2 + ((t1 - t2) * 6 * color);
	}
	else {
		if (2 * color < 1) {
			out = t1;
		}
		else {
			if (3 * color < 2) {
				out = t2 + ((t1 - t2) * 6 * (0.666 - color));
			}
			else {
				out = t2;
			}
		}
	}
	out = out * 255;
	return out;
}


void HSLtoRGB(double h, double s, double l, double &rt, double &gt, double &bt) {

	l = l / 100;
	s = s / 100;
	h = h / 360;

	if (s == 0) {
		rt = l * 255;
		gt = l * 255;
		bt = l * 255;
	}
	else {
		double temp1, temp2 ,tempr,tempg,tempb;

		if (l < 0.5) {
			temp1 = l * (1 + s);
		}
		else {
			temp1 = (l + s) - ( l * s);
		}

		temp2 = (2 * l) - temp1;

		tempr = h + 0.333;
		tempg = h;
		tempb = h - 0.333;

	//	cout << tempr << endl << tempg << endl << tempb <<endl;

		if (tempr < 0) {
			tempr = tempr + 1;
		}
		if (tempb < 0) {
			tempb = tempb + 1;
		}
		if (tempg < 0) {
			tempg = tempg + 1;
		}

		if (tempr > 1) {
			tempr = tempr - 1;
		}
		if (tempg > 1) {
			tempg = tempg - 1;
		}
		if (tempb > 1) {
			tempb = tempb - 1;
		}

//		cout << tempr << endl << tempg << endl << tempb << endl;

		rt = round(check(tempr, temp1, temp2));
		gt = round(check(tempg, temp1, temp2));
		bt = round(check(tempb, temp1, temp2));

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

void normalBlend(double alpha,int i, int j) {
	outImage[j++] = image2[i] * alpha + image1[i] * (1 - alpha);
	i++;
	outImage[j++] = image2[i] * alpha + image1[i] * (1 - alpha);
	i++;
	outImage[j] = image2[i] * alpha + image1[i] * (1 - alpha);
}

void multipyBlend(double alpha,int i, int j) {
	
	double r1, g1, b1, r2, g2, b2;
	int r, g, b, k;

	k = j;

	r1 = image1[i++];
	g1 = image1[i++];
	b1 = image1[i];

	r2 = image2[j++];
	g2 = image2[j++];
	b2 = image2[j];

	r = r1 * (r2/255);
	g = g1 * (g2/255);
	b = b1 * (b2/255);

	outImage[k++] = r;
	outImage[k++] = g;
	outImage[k] = b;

}


void subtract(int i, int j) {
	int r1, g1, b1, r2, g2, b2, k, r, g, b;

	k = j;

	r1 = image1[i++];
	g1 = image1[i++];
	b1 = image1[i];

	r2 = image2[j++];
	g2 = image2[j++];
	b2 = image2[j];


	r = r1 - r2;
	g = g1 - g2;
	b = b1 - b2;

	//	cout << r << endl << g << endl << b << endl;

	if (r < 0) {
		r = 0;
	}

	if (b < 0) {
		b = 0;
	}
	if (g < 0) {
		g = 0;
	}

	outImage[k++] = r;
	outImage[k++] = g;
	outImage[k] = b;

}

void add(int i, int j) {
	int r1, g1, b1, r2, g2, b2, k, r, g, b;

	k = j;

	r1 = image1[i++];
	g1 = image1[i++];
	b1 = image1[i];

	r2 = image2[j++];
	g2 = image2[j++];
	b2 = image2[j];


	r = r1 + r2;
	g = g1 + g2;
	b = b1 + b2;

	//	cout << r << endl << g << endl << b << endl;

	if (r > 255) {
		r = 255;
	}

	if (b > 255) {
		b = 255;
	}
	if (g > 255) {
		g = 255;
	}

	outImage[k++] = r;
	outImage[k++] = g;
	outImage[k] = b;

}

void maxBlend(int i, int j) {
	int r1, g1, b1, r2, g2, b2, k, r, g, b;

	k = j;

	r1 = image1[i++];
	g1 = image1[i++];
	b1 = image1[i];

	r2 = image2[j++];
	g2 = image2[j++];
	b2 = image2[j];


	r = max(r1, r2);
	g = max(g1, g2);
	b = max(b1, b2);


	outImage[k++] = r;
	outImage[k++] = g;
	outImage[k] = b;

}

void minBlend(int i, int j) {
	int r1, g1, b1, r2, g2, b2, k, r, g, b;

	k = j;

	r1 = image1[i++];
	g1 = image1[i++];
	b1 = image1[i];

	r2 = image2[j++];
	g2 = image2[j++];
	b2 = image2[j];


	r = min(r1, r2);
	g = min(g1, g2);
	b = min(b1, b2);


	outImage[k++] = r;
	outImage[k++] = g;
	outImage[k] = b;

}

int main(int argc, char *argv[])
{

	readImage();
	readControlImage();
	outImage = new unsigned char[width * height * 3];
	double *hsvControl, *hsvTest;
	hsvControl = new double[width2 * height2 * 3];
	hsvTest = new double[width2 * height2 * 3];

	double alpha;
	cout << "Enter the opacity in terms of percentage : ";
	cin >> alpha;
	alpha = alpha / 100;

	cout << alpha;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = (y * width + x) * 3;
			int j = (y * width + x) * 3;
			normalBlend(alpha,i,j);
			//multipyBlend(alpha, i, j);
			//subtract(i, j);
			//add(i, j);
			//maxBlend(i, j);
			//minBlend(i, j);
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
	glutCreateWindow("Linear Dodge Blend");
	init();
	glutReshapeFunc(windowResize);
	glutDisplayFunc(windowDisplay);
	glutMouseFunc(processMouse);
	glutMainLoop();
	
	return 0;
}
