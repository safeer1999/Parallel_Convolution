#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef vector<vector<float> > matrix;

void print_matrix(matrix a,int beg_row=0,int beg_col=0,int row=0,int col=0)// displays a float matrix
{
	if (row==0 && col==0)
	{
		row = a.size();
		col = a[0].size();	
	}

	for (int i = 0; i < row; ++i)
	{
		for(int j=0;j<col;j++)
		{
			cout<<a[beg_row+i][beg_col+j]<<" ";
		}
		cout<<endl;
	}
}

void save_matrix(int rows)
{
	float array[784];
	fstream file;
	file.open("imgs.dat",ios::out|ios::binary);

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	for (int i = 0; i < rows; ++i)
	{
		for(int j=0;j<784;j++)
		{
			array[j]=random()%1000;
		}
		file.write((char*)array,784*sizeof(float));
	}

	cout<<"saved matrix to file"<<endl;

	file.close();


}

void load_matrix(char filename[],int rows)
{
	fstream file;
	float array[784];

	file.open(filename,ios::in|ios::binary);

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	for (int i = 0; i < rows; ++i)
	{
		file.read((char*)array,784*sizeof(float));
		for(int j=0;j<784;j++)
			cout<<array[j]<<" ";
		cout<<endl;
	}
	
	cout<<"loaded file"<<endl;

	file.close();

}

int main()
{


	/*float dataset[10][784]; 
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<784;j++)
		{
			dataset[i][j] = random()%1000;
		}
	}*/

	save_matrix(65000);

	char filename[] = "imgs.dat";
	//load_matrix(filename,65000);


	/*int a[][3] = {{1,2,3},{4,5,6},{7,8,9}};

	fstream file;
	file.open("sample.dat",ios::out|ios::binary);

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	file.write((char*)a,sizeof(int)*9);

	file.close();

	file.open("sample.dat",ios::in|ios::binary);

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	int b[3][3];
	file.read((char*)b,sizeof(int)*9);

	cout<<b[0][1]<<" "<<b[2][2]<<endl;

	file.close();*/

	return 0;
}