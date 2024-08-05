//
// Instructor: Sai-Keung WONG
// Email:	cswingo@cs.nctu.edu.tw
//			wingo.wong@gmail.com
//
// National Yang Ming Chiao Tung University, Taiwan
// Computer Science
// Date: 2021/03/01
//
#include "mySystem_CubicFunction.h"
#include <iostream>

using namespace std;

CUBIC_FUNCTION::CUBIC_FUNCTION( )
{
	mNumSamples = 400;
    a=1;
	b = -0.5;
	c = d = 0.5;
}

void CUBIC_FUNCTION::increase(double dv)
{
    b += 0.1;
}

    void CUBIC_FUNCTION::decrease(double dv)
    {
    b -= 0.1;

    }

int x4=-100,x5=100;
void CUBIC_FUNCTION::askForInput( )
{
	//
	//ax^3 + bx^2 + cx + d
	//Show the system name
	//Ask to input a, b, c, d
	//Ask to input the range of x in [-100, 100]
	//Ask to input the number of sample points
	//
	cout << "CUBIC_FUNCTION" << endl;
	
	cout << "Input  the two coefficients: c and d." << endl;
	cin >> c >> d;
	while(true)
	{
	cout << "Input the range of x in [-100, 100]:" << endl;
	cin >> x4 >> x5; 
	if(x4>-100 & x5<100)  break;
	}
	while(true)
	{
	cout << "Input the number of sample points[2-100]:" << endl;
	cin >> mNumSamples;
	if( mNumSamples>2 & mNumSamples<100) break;
	}
}

void CUBIC_FUNCTION::getRangeOfX( double &x0, double &x1 ) const
{
	//Return the range of x in x0 and x1.
	x0 = x4;
	x1 = x5;
}

int CUBIC_FUNCTION::getNumOfSamples( ) const
{
	//Return the number of samples
	//return mNumSamples;
	return mNumSamples;

}

double CUBIC_FUNCTION::getValue( double x ) const
{
	//Compute ax^3 + bx^2 + cx + d
	//i.e., compute the value a*x*x*x + b*x*x + c*x + d
	return a*x*x*x + b*x*x + c*x + d;
	//return ((a*x + b)*x + c)*x + d;
}
// CODE: 2021/03/01. Do not delete this line.