//
// Instructor: Sai-Keung WONG
// Email:	cswingo@cs.nctu.edu.tw
//			wingo.wong@gmail.com
//
// National Yang Ming Chiao Tung University, Taiwan
// Computer Science
// Date: 2021/03/29
//
#include "mySystem_SortingVisualization.h"
#include <iostream>

using namespace std;

SORTING_VISUALIZATION::SORTING_VISUALIZATION( )
{
    mNumElements = 20;
    mMinValue = 1;
    mMaxValue = 30;
    reset( );
}

/*
Generate randomly the elements between mMinValue and mMaxValue.
The total number of elements is mNumElements.
*/
void SORTING_VISUALIZATION::reset( )
{
	mNumbers.clear();
	for(int i=0;i<mNumElements;i++)
	{
		int x=rand()%(mMaxValue-mMinValue)+mMinValue;
		mNumbers.push_back(x);
	}
	mMaxNonHandledElementIndex=mNumElements-1;
	mCurrentIndex=0;
}

/*
Show the system title.
Ask to input the range [mMinValue, mMaxValue] of the elements.
Ask to input the number of elements.
Generate the elements.
*/
void SORTING_VISUALIZATION::askForInput( )
{
    cout << "//////////////////////////////" << endl;
    cout << "SORTING_VISUALIZATION" << endl;
	cout<<"input the range of the number range [1,30]"<<endl;
	cout<<"input the mMaxValue"<<endl;
	cin>>mMaxValue;
	cout<<"input the mMinValue"<<endl;
	cin>>mMinValue;
	cout<<"input the number of element range"<<endl;
	cin>>mNumElements;
	reset();
    cout << "//////////////////////////////" << endl;
    cout << "Press SPACEBAR to visualize the process..." << endl;
    cout << "//////////////////////////////" << endl;

}

/*
Two elements whose
indices are (index) and (index+1).

Swap these two elements if their order is incorrect.
*/
void SORTING_VISUALIZATION::checkAndSwapTwoElements( int index )
{
	if ( mNumbers[ index ] > mNumbers[ index+1 ]) swap( mNumbers[ index ], mNumbers[ index+1 ] );

}

/*
Perform one swap step for two elements.
The first element is indicated by mCurrentIndex.
*/
void SORTING_VISUALIZATION::processOneSwapStep( )
{
		if(mCurrentIndex!=mMaxNonHandledElementIndex)
		{
			checkAndSwapTwoElements( mCurrentIndex );
			mCurrentIndex++;
		}
		else
		{
			mMaxNonHandledElementIndex--;
			mCurrentIndex=0;
		}
}

/*
Return the current element that will be processed.
*/
int SORTING_VISUALIZATION::getCurrentElementIndex( ) const
{
    return mCurrentIndex;
}

/*
Return the index of the last non-handled element.

mMaxNonHandledElementIndex
*/
int SORTING_VISUALIZATION::getMaxNonHandledElementIndex( ) const
{
    return mMaxNonHandledElementIndex;
}

/*
Return the number of elements.
*/
int SORTING_VISUALIZATION::getNumOfElements( ) const
{
    //Return the number of elements
    return mNumElements;
}

/*
Return the element whose index is elementIndex.
*/
int SORTING_VISUALIZATION::getElement( int elementIndex ) const
{
    return mNumbers[elementIndex];
}