//
// Instructor: Sai-Keung WONG
// Email:	cswingo@cs.nctu.edu.tw
//			wingo.wong@gmail.com
//
// National Yang Ming Chiao Tung University, Taiwan
// Computer Science
// Date: 2021/03/01
//
#include "mySystem_SineCosineFunction.h"
#include <iostream>

using namespace std;

namespace {
//minimum number of sample points
#define MIN_NUM_SAMPLES 10

//default number of sample points
#define DFT_NUM_SAMPLES 100  
};

SINECOSINE_FUNCTION::SINECOSINE_FUNCTION( )
{
	x0 = -10.0;
	x1 = 10.0;
	mNumSamples = DFT_NUM_SAMPLES;
	a = b = 1;
}

void SINECOSINE_FUNCTION::increaseA(double increasedAmount)
{
	increasedAmount=0.05;
    a += increasedAmount;

}

    void SINECOSINE_FUNCTION::decreaseA(double decreasedAmount)
{
	decreasedAmount=0.05;
    a -= decreasedAmount;
}

    void SINECOSINE_FUNCTION::increaseB(double increasedAmount)
{
	increasedAmount=0.05;
    b += increasedAmount;

}

void SINECOSINE_FUNCTION::decreaseB(double decreasedAmount)
{
	decreasedAmount=0.05;
    b -= decreasedAmount;
}

void SINECOSINE_FUNCTION::increaseSampleNum(int increasedAmount )
{
	increasedAmount=5;
    mNumSamples += increasedAmount;
}

void SINECOSINE_FUNCTION::decreaseSampleNum(int decreasedAmount)
{
	decreasedAmount=5;
	mNumSamples -= decreasedAmount;
	if(mNumSamples<MIN_NUM_SAMPLES)
	{
		mNumSamples=MIN_NUM_SAMPLES;
	}
}

//
// reset
void SINECOSINE_FUNCTION::reset()
{
    a = b = 1;
}

//
//Show the system name
//Ask to input a, b
//Ask to input the range of x in [-10, 10]
//Ask to input the number of sample points
//
int x2=-10,x3=10;
void SINECOSINE_FUNCTION::askForInput( )
{
	cout << "SINECOSINE_FUNCTION" << endl;
	cout << "Input the coeffients, a and b: a cos(b*x)" << endl;
	cin >> a >> b;
	while(true)
	{
	cout<<"input the range of x"<<endl;
	cin>>x2>>x3;
	if(x2>-10 & x3<10)	break;
	}
	cout<<"input the number of the sample"<<endl;
	cin>>mNumSamples;
}
//
// Get the range of x
// by-reference
//(x0, x1) formal parameters
// x0: minimum x 
// x1: maximum x
//
void SINECOSINE_FUNCTION::getRangeOfX( double &x0, double &x1 ) const
{
	//Return the range of x in x0 and x1.
	x0=x2;
	x1=x3;
}

//
//Return the number of samples
//
int SINECOSINE_FUNCTION::getNumOfSamples( ) const
{
	return mNumSamples;
}

//
// Get the function value for x
//
double SINECOSINE_FUNCTION::getValue( double x ) const
{
	// a cos(b*x)
	return a*cos(b*x);
}
// CODE: 2021/03/01. Do not delete this line.