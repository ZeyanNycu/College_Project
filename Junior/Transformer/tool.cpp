#include<cmath>
#include<iostream>
#include"glut.h"
using namespace std;
#define M 4 
void getCofactor(double A[M][M], double temp[M][M], int p, int q,int n)
{
    int i = 0, j = 0;
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            //  Copying into temporary matrix only those
            //  element which are not in given row and
            //  column
            if (row != p && col != q) {
                temp[i][j++] = A[row][col];
 
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}
double determinant(double A[M][M], int n)
{
    double D = 0; // Initialize result
 
    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];
 
    double temp[M][M]; // To store cofactors
 
    int sign = 1; // To store sign multiplier
 
    // Iterate for each element of first row
    for (int f = 0; f < n; f++) {
        // Getting Cofactor of A[0][f]
        getCofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1);
 
        // terms are to be added with alternate sign
        sign = -sign;
    }
 
    return D;
}
void adjoint(double A[M][M], double adj[M][M])
{
    if (M == 1) {
        adj[0][0] = 1;
        return;
    }
 
    // temp is used to store cofactors of A[][]
    int sign = 1;
	double temp[M][M];
 
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            // Get cofactor of A[i][j]
            getCofactor(A, temp, i, j, M);
 
            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i + j) % 2 == 0) ? 1 : -1;
 
            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign) * (determinant(temp, M - 1));
        }
    }
}
bool inverse(double A[M][M], double inverse[M][M])
{
    // Find determinant of A[][]
    double det = determinant(A, M);
    if (det == 0) {
        cout << "Singular matrix, can't find its inverse";
        return false;
    }
    // Find adjoint
    double adj[M][M];
    adjoint(A, adj);
    // Find Inverse using formula "inverse(A) =
    // adj(A)/det(A)"
    for (int i = 0; i < M; i++)
        for (int j = 0; j < M; j++)
            inverse[i][j] = adj[i][j] / float(det);
    return true;
}
struct Vertex
{
		double x;
		double y;
		double z; 
		double w;	
};
void mmul(double matA[][4],double matB[][4],double matC[][4])
{
	for(int i = 0; i<4 ; i++)
	{
		for(int j = 0; j<4 ; j++)
		{
			matC[i][j] = 0;
			for(int k = 0 ; k<4 ; k++)	matC[i][j] += matA[i][k]*matB[k][j];
		}
	}
}
void mul(double matA[][4],Vertex& v)
{
	double ans[] = {0,0,0,0};
	for(int i = 0; i<4 ; i++)
	{
		ans[i] += matA[i][0]*v.x;
		ans[i] += matA[i][1]*v.y;
		ans[i] += matA[i][2]*v.z;
		ans[i] += matA[i][3]*v.w;
	}
	v.x = ans[0]; v.y = ans[1]; v.z = ans[2]; v.w = ans[3];
}
void go_back(double s[4][4],double x,double y,double z,Vertex& v)
{
	double R[4][4] = {
				{1,0,0, x},
				{0,1,0, y},
				{0,0,1,0},
				{0,0,0,1}
			};
	mul(R,v);
	mul(s,v);
	double R1[4][4] = {
				{1,0,0, -x},
				{0,1,0, -y},
				{0,0,1,0},
				{0,0,0,1}
			};
	mul(R1,v);
	
}
