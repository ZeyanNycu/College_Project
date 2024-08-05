//
// Instructor: Sai-Keung WONG
// Email:	cswingo@cs.nctu.edu.tw
//			wingo.wong@gmail.com
//
// National Yang Ming Chiao Tung University, Taiwan
// Computer Science
// Date: 2021/03/01
//
#include "mySystem_StudentRecordManager.h"
#include <iostream>
#include<cmath>

using namespace std;

STUDENT_RECORD_MANAGER::STUDENT_RECORD_MANAGER( )
{
	mNumStudents = 0;
}

void STUDENT_RECORD_MANAGER::askForInput( )
{
	//Show the system title
	//Ask to input the number of students
	//Ask to input the score of each student
	//Show the average
	//Show the standard deviation
	//Show the scores in an ascending order
	//
	cout << "STUDENT_RECORD_MANAGER" << endl;
	while ( true ) {
		cout << "Input the number of students [2, 100]:" << endl;

		cin >> mNumStudents;
		if ( mNumStudents >= 2 && mNumStudents <=100 ) break;
	}
		int score;
		cout<<"input the score of each student"<<endl;
		for(int i=0;i<mNumStudents;i++)
		{
			cin>>score;
			mScore.push_back(score);
		}

	computeAverage( );

	computeStandardDeviation( );

    
    sortScores( );
	showSortedScores( );
}

double STUDENT_RECORD_MANAGER::computeAverage( )
{
	//Compute the average
    double totalScore = 0;
	for(int i=0;i<mNumStudents;i++)
	{
		totalScore+=mScore[i];
	}
	mAverageScore = totalScore/mNumStudents;
	cout<<"average is"<<mAverageScore<<endl;
	return mAverageScore;
}

double STUDENT_RECORD_MANAGER::computeStandardDeviation( )
{
	//Compute the standard deviation

	double sum = 0.0,average=0.0;
	for(int i=0;i<mNumStudents;i++)
	{
		sum+=mScore[i];
	}
	average=sum/mNumStudents;
	sum=0;
	for(int j=0;j<mNumStudents;j++)
	{
		sum+=(mScore[j]-average)*(mScore[j]-average)/(mNumStudents);
	}
	sum=sqrt(sum);
	cout<<"StandardDeviation is"<<sum<<endl;
	return mStandardDeviation;
}

void STUDENT_RECORD_MANAGER::sortScores( )
{
	for(int i=0;i<mNumStudents;i++)
	{
		for(int j=i;j<mNumStudents;j++)
		{
			if(mScore[i]>mScore[j])
			{
				swap(mScore[i],mScore[j]);
			}
		}
	}
}

void STUDENT_RECORD_MANAGER::showSortedScores( ) const
{
	for(int k=0;k<mNumStudents;k++)
	{
		cout<<mScore[k]<<endl;
	}
}
int STUDENT_RECORD_MANAGER::getNumOfStudentsInScoreInterval(int s0, int s1) const
{
	//
	//Return the number of students whose score is inside the interval [s0, s1]. 
	//s0 and s1 are included.
	// 
	int num = 0;
	for(int i=0;i<mNumStudents;i++)
	{
		if(s0<=mScore[i] && s1>=mScore[i])
		{
			num++;
		}
	}
    return num;
}
// CODE: 2021/03/01. Do not delete this line.