#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

void convert_string_to_float(char row[], float row_float[], int len)
{
	char* split = strtok(row,", ");

	for (int i = 0; i < len; i++)
	{
		row_float[i]=stof(split);
		//cout<<split<<endl;
		split = strtok(NULL,", ");
	}
}

void to_dat(char filename[])
{
	fstream text_file,dat_file;

	text_file.open(filename,ios::in);
	dat_file.open("index.dat",ios::out|ios::binary);

	char row[3200];
	text_file.getline(row,3199);
	char* split;
	split = strtok(row,"(,)");
	const int ROWS = stoi(split);
	split = strtok(NULL,"(,)");
	const int COL = stoi(split);
	dat_file.write((char*)&ROWS,sizeof(int));
	dat_file.write((char*)&COL,sizeof(int));
	float row_float[COL];
	int count=0;



	while(text_file)
	{
		count++;
		if(text_file.getline(row,3199))
		{
		convert_string_to_float(row,row_float,COL);
		dat_file.write((char*)row_float,COL*sizeof(float));
		cout<<count<<endl;
		}
		
	}

	//cout<<count<<endl;


}

string convert_float_to_string(float row_float[],int len)
{
	string row_string;
	for (int i = 0; i < len; ++i)
	{
		//cout<<row_float[i]<<endl;
		row_string.append(to_string(row_float[i]));
		row_string.append(", ");
	}
	return row_string;
}

void to_txt(char filename[])
{
	fstream text_file,dat_file;

	dat_file.open(filename,ios::in|ios::binary);
	text_file.open("index_back.txt",ios::out);

	int rows,cols;
	dat_file.read((char*)&rows,sizeof(int));
	dat_file.read((char*)&cols,sizeof(int));

	const int ROWS = rows;
	const int COL  = cols;

	float row_float[COL];

	text_file<<"("<<ROWS<<","<<COL<<")"<<endl;
	while(dat_file)
	{
		dat_file.read((char*)row_float,COL*sizeof(float));
		string row_string=convert_float_to_string(row_float,COL);
		//cout<<row_string<<endl;
		text_file<<row_string<<endl;

	}

}

int main()
{
	//to_dat("index.txt");
	to_txt("index.dat");
	return 0;
}