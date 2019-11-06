#include <vector>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
#include <fstream>
#include <omp.h>
#include <string.h>

using namespace std;

typedef vector<vector<float> > matrix;

class Convolution
{
public:
	Convolution();
	~Convolution();
	
};
//--------------------------------------------------------SAVE/LOAD MATRICES----------------------------------

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

void to_dat(char filename[],char new_file[])
{
	fstream text_file,dat_file;

	text_file.open(filename,ios::in);
	dat_file.open(new_file,ios::out|ios::binary);

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
		//cout<<count<<endl;
		}
		
	}

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

void to_txt(char filename[],char new_file[])
{
	fstream text_file,dat_file;

	dat_file.open(filename,ios::in|ios::binary);
	text_file.open(new_file,ios::out);

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


vector<matrix> load_matrix(char filename[],int num)
{
	fstream file;
	vector<matrix> dataset;
	int rows,col;

	file.open(filename,ios::in|ios::binary);
	file.read((char*)&rows,sizeof(int));
	file.read((char*)&col,sizeof(int));
	const int ROWS=rows;
	const int COL=col;
	float array[ROWS][COL];

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	for (int i = 0; i < num; ++i)
	{
		matrix m;
		file.read((char*)array,ROWS*COL*sizeof(float));
		for(int j=0;j<ROWS;j++)
		{
			m.push_back(vector<float>(array[j],array[j]+COL));
		}
		dataset.push_back(m);
	}

	/*for (int i = 0; i < 200; ++i)
	{
		for(int j=0;j<200;j++)
			cout<<array[i][j]<<" ";
		cout<<endl;
	}*/
	
	cout<<endl<<endl<<"loaded file"<<endl;

	file.close();

	return dataset;

}


//----------------------------------------------------MATRIX OPERATIONS------------------------------------------

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

//multiplies corresponding elements of 2 matrices' submatrix such that sub_mat(a)[<row>][<col>] X sub_mat(b)[<row>][<col>] 
//<a> -> first matrix
//<b> -> second matrix
matrix matrix_multiply(matrix a,int a_beg_row,int a_beg_col,matrix b, int b_beg_row,int b_beg_col  ,int row,int col)
{
	//omp_set_num_threads(8);
	matrix product(row,vector<float>(col,0) );

	#pragma omp parallel
	{
		#pragma  omp for collapse(2)
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < col; ++j)
			{
				product[i][j] = a[a_beg_row+i][a_beg_col+j]*b[b_beg_row+i][b_beg_col+j] ;

				//cout<<product[i][j]<<" ";
			}
			//cout<<endl;
		}
	}

	return product;
}

//sum of the matrix
float matrix_sum(matrix a)
{
	float sum=0;

	//omp_set_num_threads(8);
	#pragma omp parallel
	{
		#pragma omp for reduction(+:sum) collapse(2)
		for (int i = 0; i < a.size(); ++i)
		{
			for (int j = 0; j < a[0].size(); ++j)
			{
				sum+= a[i][j];
			}
		}
	}

	return sum;
}

// max value in matrix <a>
float matrix_max(matrix a,int beg_row,int beg_col, int row,int col)
{
	const int ROWS = row;
	float l[ROWS];


	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < row; ++i)
		{
			l[i] = -INFINITY;
			for (int j = 0; j < col; ++j)
			{
				if(a[beg_row+i][beg_col+j] > l[i])
				{
					l[i] = a[beg_row+i][beg_col+j];
				}
			}
		}
	}

	float l_max = -INFINITY;
	for(int i=0;i<ROWS;i++)
	{
		if(l[i]>l_max)
		{
			l_max=l[i];
		}
	}

	return l_max;

}
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------CONVOLUTION OPERATIONS--------------------

// creates <num_filters> number of filters and stores in filter_bank
// <filter_shape> -> dimension of each filter
//<filter_bank>-> array of filters
void init_filters(int num_filters,int filter_shape[], vector<matrix > &filter_bank)
{
	default_random_engine generator;
	normal_distribution<float> distribution(0.0,0.1);

    
	for (int i=0; i<num_filters; ++i)
		{
		for (int j = 0; j < filter_shape[0]; ++j)
		{
			for (int k = 0; k < filter_shape[1]; ++k)
			{
				float number = distribution(generator);
				filter_bank[i][j][k] = number;
			}
		}

	}
}



//gets an input as an input and filters it using <filter>
//<img> is the input image
//<filter> is the applied filter
matrix convolve(matrix img, int img_shape[],matrix filter, int filter_shape[], int stride)
{

	matrix filtered_img(img_shape[0]-filter_shape[0]+1,vector<float>(img_shape[1]-filter_shape[1],0));

	#pragma omp parallel for collapse(2)
	for (int i = 0; i <= img_shape[0]-filter_shape[0]; i+=stride)
	{
		for (int j = 0; j <= img_shape[1]-filter_shape[1]; j+=stride)
		{
			filtered_img[i][j] = matrix_sum(matrix_multiply(img,i,j,filter,0,0,filter_shape[0],filter_shape[1]));
		}

	}

	return  filtered_img;
}


//applies multiple filters from the <filter_bank> on the input <img>
vector<matrix > apply_filter(matrix  img,int img_shape[],vector<matrix >  filter_bank,int filter_shape[])
{
	vector<matrix > convolved_layer(filter_bank.size());
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < filter_bank.size(); ++i)
		{
			convolved_layer[i] = convolve(img,img_shape,filter_bank[i],filter_shape,1);
		}
	}

	return convolved_layer;
}



//applies pooling function on the <img>
// <stride> -> step size of pooling frame
// <pool_dime> -> size of the frame
matrix apply_maxPool(matrix  img,int img_shape[],int stride,int pool_dim)
{
	matrix pooled_img(img_shape[0]-pool_dim+1, vector<float>(img_shape[1]-pool_dim+1));

	#pragma omp parallel for collapse(2)
	for (int i = 0; i <= img_shape[0]-pool_dim; i+=stride)
	{
		for (int j = 0; j <= img_shape[1]-pool_dim; j+=stride)
		{
			//cout<<i<<' '<<j<<endl;
			pooled_img[i][j] = matrix_max(img,i,j,pool_dim,pool_dim);

		}
	}

	return pooled_img;



}


vector<matrix > apply_maxPool_to_filters(vector<matrix >  prev_layer,int img_shape[])
{

	vector<matrix > pooled_layer(prev_layer.size());

	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < prev_layer.size(); ++i)
		{
			matrix v =  apply_maxPool(prev_layer[i],img_shape,2,2);
			pooled_layer[i] = v;
		}
	}

	return pooled_layer;
}

float reLU(float x)
{
	return x>0 ? x : 0;
}

void apply_activation(matrix &inp)
{

	#pragma omp parallel
	{
		#pragma omp for collapse(2)
		for (int i = 0; i < inp.size(); ++i)
		{
			for (int j = 0; j < inp[0].size(); ++j)
			{
				inp[i][j] = reLU(inp[i][j]);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------

//---------------------------------------------------------DRIVER FUNCTIONS-----------------------------------

//takes an input <img> applies all the filters then takes the output of that and applies the activation function followed by max pooling
vector<matrix > feed_through_layer(matrix img, int img_shape[], vector<matrix > filter_bank,int filter_shape[])
{
	vector<matrix > temp = apply_filter(img,img_shape,filter_bank,filter_shape);
	
	
	for(int i=0;i<temp.size();i++)
		apply_activation(temp[i]);

	int filtered_img_shape[] = {temp[0].size(),temp[0][0].size()};
	return apply_maxPool_to_filters(temp,filtered_img_shape);
}


int main(int argc, char const *argv[])
{
	int num_filters = 4;
	int filter_shape[] = {3,3};
	int num_images = 20;
	int num_threads = 8;

	omp_set_dynamic(0);     // disable dynamic teams
	omp_set_num_threads(num_threads);



	vector<matrix> filter_bank(num_filters,vector<vector<float>>(filter_shape[0],vector<float>(filter_shape[1])));	
	double beg = omp_get_wtime();
	init_filters(num_filters,filter_shape,filter_bank);
	double end = omp_get_wtime();
	cout<<"Time to initialize filters: "<<end-beg<<endl;
	
	beg = omp_get_wtime();
	char filename[] = "imgs.dat";
	vector <matrix> imgs = load_matrix(filename,num_images);
	int img_shape[] = {imgs[0].size(),imgs[0][0].size()};
	cout<<img_shape[0]<<", "<<img_shape[1]<<endl;
	end = omp_get_wtime();
	cout<<"Time to load dataset: "<<end-beg<<endl<<"Loaded file: "<<filename<<endl;

	beg = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < num_images; ++i)
	{
		float init = omp_get_wtime();
		vector<matrix > final_layer = feed_through_layer(imgs[i],img_shape,filter_bank,filter_shape);
		float final = omp_get_wtime();

		cout<<"Time to perform convolution on image "<<i<<": "<<final-init<<endl;
	}
	end = omp_get_wtime();

	cout<<endl<<"time for all images: "<<end-beg<<endl;



	return 0;
}
