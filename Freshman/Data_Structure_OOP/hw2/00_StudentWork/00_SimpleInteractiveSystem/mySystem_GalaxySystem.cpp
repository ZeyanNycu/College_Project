//
// Instructor: Sai-Keung WONG
// Email:	cswingo@cs.nctu.edu.tw
//			wingo.wong@gmail.com
//
// National Yang Ming Chiao Tung University, Taiwan
// Computer Science
// Date: 2021/03/29
//
#include "mySystem_GalaxySystem.h"
#include <iostream>
#include<time.h>
using namespace std;

namespace {
    double getRandDouble( double L, double U ) {
        return L + (U-L)*rand( )/(double) RAND_MAX;
    }
};

GALAXY_SYSTEM::GALAXY_SYSTEM( )
{
    mTimeStep = 0.0025; 
    mMaxV = 100.0;
    mNumOfObjs = 100;
    mNumLargeObjs = 1;
    mMinR = 0.5;            //minimum radius of objects
    mMaxR = 3.0;            //maximum radius of objects
    mSpaceSize = 300.0;
    mG = 5.0;               //gravitational constant
    generateObjects( );
}

/*
Generate randomly the positions,  
velocities and radii of the objects.//

Also, if the objects are large, 
set their masses to be a large number.

The mass of an  object depends on the object's radius.

The position of an object is inside 
[-halfSpaceSize, halfSpaceSize]x[-halfSpaceSize, halfSpaceSize].

The velocity of an object is inside
[-100.0, 100]x[-100.0, 100].

Initialize the alive flag of each object.
the alive flag indicates whether the object is alive or not.
If the object is not alive, ignore it in all the calculations.

If there's only one large object,
set its position to the origin
and set its velocity to zero.

Use 
TAIL_PLANET tail(x,y);
mTails.push_back( tail ); // push a tail to mTails.
to initialize the tail of an object. 
It sets all the points of the tail to the same point, i.e., (x,y).

Each object has one tail.

*/
void GALAXY_SYSTEM::generateObjects( )
{
	srand(time(NULL));
    mNumFrames = 0;
    mTails.clear();
	mX.clear();
	mY.clear();
	mVx.clear();
	mVy.clear();
	mR.clear();
	mMass.clear();
	mAlive.clear();
    //Fill in here
    /* You can remove the following lines.
    mX.resize( mNumOfObjs );
    mY.resize( mNumOfObjs );
    mR.resize( mNumOfObjs );
    mMass.resize( mNumOfObjs );
    mVx.resize( mNumOfObjs );
    mVy.resize( mNumOfObjs );*/
    //mFx.resize( mNumOfObjs+mNumLargeObjs );
    //mFy.resize( mNumOfObjs+mNumLargeObjs );
	//mTails.resize(mNumOfObjs);
	for(int i=0;i<mNumOfObjs;i++)
	{
		double x,y;
		x=getRandDouble( -mSpaceSize/2, mSpaceSize/2 );
		y=getRandDouble( -mSpaceSize/2, mSpaceSize/2 );
		mX.push_back(x);
		mY.push_back(y);
		TAIL_PLANET tail(x,y);
		mTails.push_back(tail);
	}
	for(int j=0;j<mNumOfObjs;j++)
	{
		double x,y;
		x=getRandDouble(-mMaxV,mMaxV);
		y=getRandDouble(-mMaxV,mMaxV);
		mVx.push_back(x);
		mVy.push_back(y);
	}
	for(int j=0;j<mNumOfObjs;j++)
	{
		double x;
		x=getRandDouble(mMinR,mMaxR);
		mR.push_back(x);
	}
	for(int k=0;k<mNumOfObjs;k++)
	{
		mMass.push_back(mR[k]*mR[k]);
	}
	for(int k=0;k<mNumOfObjs;k++)
	{
		mAlive.push_back(true);
	}
	/*for(int l=0;l< mNumOfObjs+mNumLargeObjs;l++)
	{
		mFx[l]=0;
		mFy[l]=0;
	}*/
	if(mNumLargeObjs == 1)
	{
		mX.pop_back();
		mY.pop_back();
		mVx.pop_back();
		mVy.pop_back();
		mR.pop_back();
		mMass.pop_back();
		mX.push_back(0);
		mY.push_back(0);
		mVx.push_back(0);
		mVy.push_back(0);
		mR.push_back(mMaxR*2.0);
		mMass.push_back(mMaxR*mMaxR*1000);
	}
	if((mNumLargeObjs == 2))
	{
		for(int j=0;j<mNumLargeObjs;j++)
		{
			mX.pop_back();
			mY.pop_back();
			mVx.pop_back();
			mVy.pop_back();
			mR.pop_back();
			mMass.pop_back();
		}
		for(int i=0;i<mNumLargeObjs;i++)
		{
			mX.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mY.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mVx.push_back(0);//getRandDouble(-mMaxV,mMaxV));
			mVy.push_back(0);//getRandDouble(-mMaxV,mMaxV));
			mR.push_back(mMaxR*2.0);
			mMass.push_back(mMaxR*mMaxR*1000);
		}
	}
	if((mNumLargeObjs == 3))
	{
		for(int j=0;j<mNumLargeObjs;j++)
		{
			mX.pop_back();
			mY.pop_back();
			mVx.pop_back();
			mVy.pop_back();
			mR.pop_back();
			mMass.pop_back();
		}
		for(int i=0;i<mNumLargeObjs;i++)
		{
			mX.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mY.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mVx.push_back(0);//getRandDouble(-mMaxV,mMaxV));
			mVy.push_back(0);//getRandDouble(-mMaxV,mMaxV));
			mR.push_back(mMaxR*2.0);
			mMass.push_back(mMaxR*mMaxR*1000);
		}
	}
	mFx.resize( mNumOfObjs );
    mFy.resize( mNumOfObjs );
	mTails.resize(mNumOfObjs);
}

/*
Handle the key events.
*/
bool GALAXY_SYSTEM::handleKeyPressedEvent( int key )
{
    bool flgHandled = false;
	switch (key)
	{
	case '1':
		mNumLargeObjs =1;
		generateObjects( );
		/*mX.pop_back();
		mY.pop_back();
		mVx.pop_back();
		mVy.pop_back();
		mR.pop_back();
		mMass.pop_back();
		mX.push_back(0);
		mY.push_back(0);
		mVx.push_back(0);
		mVy.push_back(0);
		mR.push_back(mMaxR*2.0);
		mMass.push_back(mMaxR*mMaxR*1000);*/
		flgHandled=true;
		break;
	case '2':
		mNumLargeObjs =2;
		generateObjects( );
		//update();
		/*for(int j=0;j<mNumLargeObjs;j++)
		{
			mX.pop_back();
			mY.pop_back();
			mVx.pop_back();
			mVy.pop_back();
			mR.pop_back();
			mMass.pop_back();
		}
		for(int i=0;i<mNumLargeObjs;i++)
		{
			mX.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mY.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mVx.push_back(getRandDouble(-mMaxV,mMaxV));
			mVy.push_back(getRandDouble(-mMaxV,mMaxV));
			mR.push_back(mMaxR*2.0);
			mMass.push_back(mMaxR*mMaxR*1000);
		}*/
		flgHandled=true;
		break;
	case '3':
		mNumLargeObjs =3;
		generateObjects( );
		//update();
		//update();
		/*for(int j=0;j<mNumLargeObjs;j++)
		{
			mX.pop_back();
			mY.pop_back();
			mVx.pop_back();
			mVy.pop_back();
			mR.pop_back();
			mMass.pop_back();
		}
		for(int i=0;i<mNumLargeObjs;i++)
		{
			mX.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mY.push_back(getRandDouble( -mSpaceSize/2, mSpaceSize/2 ));
			mVx.push_back(getRandDouble(-mMaxV,mMaxV));
			mVy.push_back(getRandDouble(-mMaxV,mMaxV));
			mR.push_back(mMaxR*2.0);
			mMass.push_back(mMaxR*mMaxR*1000);
		}*/
		flgHandled=true;
		break;
	}
    return flgHandled;
}

/*
Generate the objects.
Call generateObjects( ).
*/
void GALAXY_SYSTEM::reset( )
{
    generateObjects( );
}

/*
Show the system title.
Show the key usage.
*/
void GALAXY_SYSTEM::askForInput( )
{
    cout << "GALAXY_SYSTEM::askForInput" << endl;
	 cout << "Key usage:" << endl;
    cout << "1: one large object" << endl; // generate the objects. Only one large object
    cout << "2: two large object" << endl; // generate the objects. Only two large objects
    cout << "3: three large object" << endl; // generate the objects. Only three large objects
    cout << "r: reset" << endl;

}

/*
Return the number of objects
return mNumOfObjs
*/
int GALAXY_SYSTEM::getNumOfObjs( ) const
{
    return mNumOfObjs;
}

/*
Get the object information based on the object index.
(x,y) = position
r = radius
Return the alive flag.
*/
bool GALAXY_SYSTEM::getObjInfo( int objIndex, double &x, double &y, double &r ) const
{
    x = mX[objIndex];
    y = mY[objIndex];
    r = mR[objIndex];
    return mAlive[objIndex];
}

/*
Merge the objects if they overlap with each other.
Modify the velocities of the objects after merging
based on the conservation of momentum.
Set the alive flags of the objects accordingly.

Pseudo-code

For each pair of the objects
  if they do not overlap continue
  If they overlap
    do
      turn off the alive flag of the object with smaller radius
      compute the new velocity of the larger object
*/
void GALAXY_SYSTEM::mergeObjects( )
{
	//cout<<"hello"<<endl;
	double mivi_mjvj_X;
	double mivi_mjvj_Y;
	for(int i=0;i<mNumOfObjs+mNumLargeObjs;i++)
	{
		if ( !mAlive[i] ) continue;
		for(int j=i+1;j<mNumOfObjs+mNumLargeObjs;j++)
		{
			if ( !mAlive[j] ) continue;
			if ( !mAlive[i] ) continue;
			double d=sqrt((mX[i]-mX[j])*(mX[i]-mX[j])+(mY[i]-mY[j])*(mY[i]-mY[j]));
			if(d<mR[i] || d<mR[j])
			{
			if(mMass[i]>=mMass[j])
			{
				mivi_mjvj_X = mVx[i]*mMass[i]+mVx[j]*mMass[j];
				mivi_mjvj_Y = mVy[i]*mMass[i]+mVy[j]*mMass[j];
				mMass[i] += mMass[j];
				mVx[i] = mivi_mjvj_X / mMass[i];
				mVy[i] = mivi_mjvj_Y / mMass[i];
				mAlive[j]=false;
			}
			else
			{
				mivi_mjvj_X = mVx[i]*mMass[i]+mVx[j]*mMass[j];
				mivi_mjvj_Y = mVy[i]*mMass[i]+mVy[j]*mMass[j];
				mMass[j] += mMass[i];
				mVx[i] = mivi_mjvj_X / mMass[i];
				mVy[i] = mivi_mjvj_Y / mMass[i];
				mAlive[i]=false;
			}
			}
		}
	}
}

/*
Update the position of the objects
Steps:
1. compute the forces exerting on each object
2. compute the velocity of each object
3. compute the position of each object

Constraints:
The component of a velocity must be inside [-mMaxV, mMaxV].

*/
void GALAXY_SYSTEM::update( ) 
{
    int skipFrames = 10;
    /////////////////////////////////////////////////////
    mergeObjects( ); // merge objects first if they overlap with each other
    //cout<<"hello"<<endl;
    //Fill in here
	for(int l=0;l< mNumOfObjs;l++)
	{
		mFx[l]=0;
		mFy[l]=0;
	} 
	for(int i=0;i<mNumOfObjs;i++)
	{
		//cout<<"hello"<<endl;
		if ( !mAlive[i] ) continue;
		//cout<<"world"<<endl;
		for(int j=i+1;j<mNumOfObjs;j++)
		{
			if ( !mAlive[i] ) continue;
			if ( !mAlive[j] ) continue;
			//cout<<mFx[i]<<endl;
			double d=sqrt((mX[i]-mX[j])*(mX[i]-mX[j])+(mY[i]-mY[j])*(mY[i]-mY[j]));
		 	double dx=mX[i] - mX[j],dy=mY[i] - mY[j];
			double nx=dx/d;
			double ny=dy/d;
			double mij = mMass[i]*mMass[j];
			mFx[i] -= mij*mG*nx/(d*d);
			mFy[i] -= mij*mG*ny/(d*d);
			mFx[j] += mij*mG*nx/(d*d);
			mFy[j] += mij*mG*ny/(d*d);
			//cout<<mFx[i]<<endl;
		}
	}
    //compute forces exerting on objects
	for(int i=0;i<mNumOfObjs;i++)
	{
		if ( !mAlive[i] ) continue;
		 double m = mMass[i];
		 mVx[i] = mVx[i] + (mFx[i]/m)*mTimeStep;
		 //cout<<mVx[i]<<endl;
		 mVy[i] = mVy[i] + (mFy[i]/m)*mTimeStep;
		 //cout<<mVy[i]<<endl;
		 if(mVx[i]>mMaxV ) mVx[i]=mMaxV;
		 if(mVx[i]<-mMaxV ) mVx[i]=-mMaxV;
		 if(mVy[i]>mMaxV) mVy[i]=mMaxV;
		 if(mVy[i]<-mMaxV) mVy[i]=-mMaxV;
	}
    //compute velocities of objects
	for(int i=0;i<mNumOfObjs;i++)
	{
		if ( !mAlive[i] ) continue;
		mX[i] = mX[i] + mVx[i]*mTimeStep;
		mY[i] = mY[i] + mVy[i]*mTimeStep;
		//TAIL_PLANET tail(mX[i],mY[i]);
		//mTails.push_back(tail);
		if (skipFrames == 0 || mNumFrames%skipFrames == 0) 
		{
            mTails[i].add_to_front(mX[i], mY[i]);
		}
		//COORD_2D tail(mX[i],mY[i]);
		//mTails[i].points.push_back(tail);
	}
    //compute positions of objects
    //add a point to the tail of each object
    //each tail has a maximum number of sample points
    //

    /////////////////////////////////////////////////////
    ++mNumFrames;
}

//
// return the number of sample points of the tail
// of an object with index planet_index
int GALAXY_SYSTEM::getTail_NumSamplePoints( int planet_index ) const 
{ 
    //mTails
    return mTails[planet_index].points.size();
}

//
// return the coordinates of the sample point of an object
//
COORD_2D GALAXY_SYSTEM::getTail_SamplePointCoords( 
    int planet_index,               
    int sample_point_index ) const 
{ 
    //mTails
    return mTails[planet_index].points[sample_point_index];
}