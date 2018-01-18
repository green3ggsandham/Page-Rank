#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"

#define BUFFER 256

int getNumOfLinks(FILE * txt);
int charsInString(char * line_buffer);


int main(void)
{
	Engine *ep = NULL;
	mxArray *result = NULL;
	mxArray *testArray = NULL; // mxArray is the fundamental type underlying MATLAB data
	double time[3][3] = { { 1.0, 2.0, 3.0 },{ 4.0, 5.0, 6.0 },{ 7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
	
	testArray = mxCreateDoubleMatrix(3, 3, mxREAL);
	memcpy((void*)mxGetPr(testArray), (void *)time, 9 * sizeof(double));

	printf("\nStarting MATLAB engine\n");
	if (!(ep = engOpen(NULL))) 
	{
		fprintf(stderr, "\nCannot start MATLAB engine\n");
		system("pause");
		return 1;
	}





//EXAMPLE

	printf("\nWriting testArray\n");
	if (engPutVariable(ep, "testArray", testArray))
	{
		fprintf(stderr, "\nCannot write test array to MATLAB \n");
		system("pause");
		return 1;
	}
	if (engEvalString(ep, "testArrayEigen = eig(testArray)"))
	{
		fprintf(stderr, "\nError calculating eigenvalues  \n");
		system("pause");
		return 1;
	}
	printf("\nRetrieving eigenvector\n");
	if ((result = engGetVariable(ep, "testArrayEigen")) == NULL)
	{
		fprintf(stderr, "\nFailed to retrieve eigenvalue vector\n");
		system("pause");
		return 1;
	}
	else
	{
		size_t sizeOfResult = mxGetNumberOfElements(result);
		size_t i = 0;
		printf("The eigenvalues are:\n");
		for (i = 0; i < sizeOfResult; ++i)
		{
			printf("%f\n", *(mxGetPr(result) + i));
		}
	}
	mxDestroyArray(testArray);
	testArray = NULL;

	



//MATRIX MULTIPLICATION

	mxArray *matrixOne = NULL;
	mxArray *matrixTwo = NULL;
	double time1[3][3] = { { 1.0, 2.0, 3.0 },{ 4.0, 5.0, 6.0 },{ 7.0, 8.0, 9.0 } };		//two new matrices
	double time2[3][3] = { { 2.0, 2.0, 2.0 },{ 3.0, 3.0, 3.0 },{ 4.0, 4.0, 4.0 } };

	matrixOne = mxCreateDoubleMatrix(3, 3, mxREAL);
	matrixTwo = mxCreateDoubleMatrix(3, 3, mxREAL);
	memcpy((void*)mxGetPr(matrixOne), (void *)time1, 9 * sizeof(double));
	memcpy((void*)mxGetPr(matrixTwo), (void *)time2, 9 * sizeof(double));

	printf("\nWriting MatrixOne\n");
	if (engPutVariable(ep, "matrixOne", matrixOne))
	{
		fprintf(stderr, "\nCannot write test array to MATLAB \n");
		system("pause");
		return 1;
	}
	printf("\nWriting MatrixTwo\n");
	if (engPutVariable(ep, "matrixTwo", matrixTwo))
	{
		fprintf(stderr, "\nCannot write test array to MATLAB \n");
		system("pause");
		return 1;
	}
	printf("\nMultiplying Matrices\n");
	if (engEvalString(ep, "testArrayMultiplication = matrixOne*matrixTwo"))
	{
		fprintf(stderr, "\nError multiplying  \n");
		system("pause");
		return 1;
	}
	if ((result = engGetVariable(ep, "testArrayMultiplication")) == NULL)		//multiplies matrices stores into result
	{
		fprintf(stderr, "\nFailed to retrieve result\n");
		system("pause");
		return 1;
	}
	else
	{
		size_t sizeOfResult = mxGetNumberOfElements(result);
		size_t i = 0;
		printf("\nThe first matrix was:\n");
		for (i = 0; i < sizeOfResult / 3; ++i)		//printing first matrix
		{
			printf("%f  ", time1[0][i]);
			printf("%f  ", time1[1][i]);
			printf("%f\n", time1[2][i]);
		}
		printf("\nThe second matrix was:\n");
		for (i = 0; i < sizeOfResult / 3; ++i)		//printing second matrix
		{
			printf("%f  ", time2[0][i]);
			printf("%f  ", time2[1][i]);
			printf("%f\n", time2[2][i]);
		}
		printf("\nThe matrix product is:\n");		//printing product
		for (i = 0; i < sizeOfResult / 3; ++i)
		{
			printf("%f  ", *(mxGetPr(result) + i));     //prints first character of row
			printf("%f  ", *(mxGetPr(result) + 3 + i));		//second character of row
			printf("%f\n", *(mxGetPr(result) + 6 + i));		//third character of row
		}
	}
	mxDestroyArray(matrixOne);
	matrixOne = NULL;
	mxDestroyArray(matrixTwo);
	matrixTwo= NULL;





//PAGERANK

	FILE *txt = NULL;
	mxArray *MatrixThree = NULL;
	double *time3 = NULL;
	int length, row, column;
	char line_buffer[BUFFER];

	printf("\nOpening web.txt\n");
	fopen_s(&txt, "web.txt", "r");
	length = getNumOfLinks(txt);						//call to function that finds matrix dimension
	time3 = malloc((length*length) * sizeof(double));	//allocate space for new matrix 
	row = 0;

	while (fgets(line_buffer, BUFFER, txt))				//loop taking the information from the file and storing into new array
	{
		for (column = 0; column < length; column++)
		{
			time3[row * length + column] = line_buffer[column * 2] - '0';
		}
		row++;
	}

	MatrixThree = mxCreateDoubleMatrix(length, length, mxREAL);
	memcpy((void*)mxGetPr(MatrixThree), (void*)time3, (length*length) * sizeof(double));

	printf("\nWriting Connectivity Matrix\n");
	if (engPutVariable(ep, "ConnectivityMatrix", MatrixThree))
	{
		fprintf(stderr, "\nCannot write connect matrix to MATLAB\n");
		system("pause");
		return 1;
	}
	if (engEvalString(ep, "ConnectivityMatrix = ConnectivityMatrix'"))			//all the commands from the prelab
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "dimension = size(ConnectivityMatrix, 1);")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "columnsums = sum(ConnectivityMatrix, 1);"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "p = 0.85;")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "zerocolumns = find(columnsums ~= 0);"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "D = sparse(zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension);"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "StochasticMatrix = ConnectivityMatrix * D"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "[row, column] = find(columnsums == 0);")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "StochasticMatrix(:, column) = 1 ./ dimension;")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "Q = ones(dimension, dimension)")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q / dimension);"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "PageRank = ones(dimension, 1);"))
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "for i = 1:100 PageRank = TransitionMatrix *PageRank; end")) 
		fprintf(stderr, "%s\n", "matlab error");
	if (engEvalString(ep, "PageRank =PageRank / sum(PageRank)"))
		fprintf(stderr, "%s\n", "matlab error");

	printf("\nRetrieving PageRank\n");
	if ((result = engGetVariable(ep, "PageRank")) == NULL)
	{
		fprintf(stderr, "\nFailed to retrieve PageRank\n");
		system("pause");
		return 1;
	}
	else
	{
		size_t sizeOfResult = mxGetNumberOfElements(result);
		size_t i = 0;
		printf("\n\nNODE  RANK\n---   ----\n");
		while (i < sizeOfResult)
		{
			printf("%d    ", i + 1);
			printf("%.4f   \n", mxGetPr(result)[i++]);
		}
	}

	mxDestroyArray(MatrixThree);
	MatrixThree = NULL;




	if (engClose(ep))
	{
		fprintf(stderr, "\nFailed to close MATLAB engine\n");
	}
	fclose(txt);
	system("pause");
	return 0;
	
}


int getNumOfLinks(FILE * txt) 
{
	char line_buffer[BUFFER];
	fgets(line_buffer, BUFFER, txt);
	fseek(txt, 0, SEEK_SET);
	return (int)strlen(line_buffer) - charsInString(line_buffer, ' ') - 1;
}

int charsInString(char * line_buffer)
{
	int count=0;

	while (line_buffer[count] != NULL)
	{
		if (line_buffer[count] == ' ')
			count++;
		else
			*line_buffer++;
	}
	return count;
}
