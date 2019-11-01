#include <vector>
#include <iostream>
#include <random>
#include <time.h>
#include <math.h>
#include <fstream>
#include <omp.h>

using namespace std;

typedef vector<vector<float> > matrix;

class Convolution
{
public:
	Convolution();
	~Convolution();
	
};

matrix load_matrix(char filename[],int rows)
{
	fstream file;
	float array[784];
	matrix dataset;

	file.open("imgs.dat",ios::in|ios::binary);

	if(!file)
	{
		cout<<"file not open"<<endl;
	}

	for (int i = 0; i < rows; ++i)
	{
		file.read((char*)array,784*sizeof(float));
		vector<float> v(array,array+784);
		dataset.push_back(v);
	}
	
	cout<<"loaded file"<<endl;

	file.close();

	return dataset;

}

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

//multiplies corresponding elements of 2 matrices' submatrix such that sub_mat(a)[<row>][<col>] X sub_mat(b)[<row>][<col>] 
//<a> -> first matrix
//<b> -> second matrix
matrix matrix_multiply(matrix a,int a_beg_row,int a_beg_col,matrix b, int b_beg_row,int b_beg_col  ,int row,int col)
{
	matrix product(row,vector<float>(col,0) );
	//parallel
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			product[i][j] = a[a_beg_row+i][a_beg_col+j]*b[b_beg_row+i][b_beg_col+j] ;

			//cout<<product[i][j]<<" ";
		}
		//cout<<endl;
	}

	return product;
}

//sum of the matrix
float matrix_sum(matrix a)
{
	//parallel
	float sum=0;
	for (int i = 0; i < a.size(); ++i)
	{
		for (int j = 0; j < a[0].size(); ++j)
		{
			sum+= a[i][j];
		}
	}

	return sum;
}

//gets an input as an input and filters it using <filter>
//<img> is the input image
//<filter> is the applied filter
matrix convolve(matrix img, int img_shape[],matrix filter, int filter_shape[], int stride)
{

	matrix filtered_img;

	for (int i = 0; i <= img_shape[0]-filter_shape[0]; i+=stride)
	{
		vector<float> v;
		for (int j = 0; j <= img_shape[1]-filter_shape[1]; j+=stride)
		{
			float masked_values = matrix_sum(matrix_multiply(img,i,j,filter,0,0,filter_shape[0],filter_shape[1]));
			v.push_back(masked_values);
		}
		filtered_img.push_back(v);

	}

	return  filtered_img;
}


//applies multiple filters from the <filter_bank> on the input <img>
vector<matrix > apply_filter(matrix  img,int img_shape[],vector<matrix >  filter_bank,int filter_shape[])
{
	vector<matrix > convolved_layer;
	//parallel
	for (int i = 0; i < filter_bank.size(); ++i)
	{
		convolved_layer.push_back(convolve(img,img_shape,filter_bank[i],filter_shape,1));
	}

	return convolved_layer;
}

// max value in matrix <a>
float matrix_max(matrix a,int beg_row,int beg_col, int row,int col)
{
	float l = -INFINITY;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			if(a[beg_row+i][beg_col+j] > l)
			{
				l = a[beg_row+i][beg_col+j];
			}
		}
	}
}

//applies pooling function on the <img>
// <stride> -> step size of pooling frame
// <pool_dime> -> size of the frame
matrix apply_maxPool(matrix  img,int img_shape[],int stride,int pool_dim)
{
	matrix pooled_img;

	for (int i = 0; i <= img_shape[0]-pool_dim; i+=stride)
	{
		vector<float> v;
		for (int j = 0; j <= img_shape[1]-pool_dim; j+=stride)
		{
			//cout<<i<<' '<<j<<endl;
			float max_val = matrix_max(img,i,j,pool_dim,pool_dim);
			v.push_back(max_val);

		}
		pooled_img.push_back(v);
	}

	return pooled_img;



}


vector<matrix > apply_maxPool_to_filters(vector<matrix >  prev_layer,int img_shape[])
{
	vector<matrix > pooled_layer;
	//parallel
	for (int i = 0; i < prev_layer.size(); ++i)
	{
		matrix v =  apply_maxPool(prev_layer[i],img_shape,2,2);
		pooled_layer.push_back(v);
	}

	return pooled_layer;
}

float reLU(float x)
{
	return x>0 ? x : 0;
}

void apply_activation(matrix &inp)
{
	//parallel
	for (int i = 0; i < inp.size(); ++i)
	{
		for (int j = 0; j < inp[0].size(); ++j)
		{
			inp[i][j] = reLU(inp[i][j]);
		}
	}
}
//takes an input <img> applies all the filters then takes the output of that and applies the activation function followed by max pooling
vector<matrix > feed_through_layer(matrix img, int img_shape[], vector<matrix > filter_bank,int filter_shape[])
//void feed_through_layer(matrix img, int img_shape[], vector<matrix > filter_bank, int filter_shape[])
{
	double beg=0,end=0;

	beg =omp_get_wtime();
	vector<matrix > temp = apply_filter(img,img_shape,filter_bank,filter_shape);
	end = omp_get_wtime();
	cout<<"Time to apply_filter: "<<end-beg<<endl;
	
	beg = omp_get_wtime();
	for(int i=0;i<temp.size();i++)
		apply_activation(temp[i]);
	end = omp_get_wtime();
	cout<<"Time to apply activation: "<<end-beg<<endl;

	beg = omp_get_wtime();
	int filtered_img_shape[] = {temp[0].size(),temp[0][0].size()};
	temp =  apply_maxPool_to_filters(temp,filtered_img_shape);
	end = omp_get_wtime();
	cout<<"Time to perform max pooling: "<<end-beg<<endl;

	return temp;
}

int main(int argc, char const *argv[])
{
	int num_filters = 4;
	int filter_shape[] = {3,3};



	vector<matrix> filter_bank(num_filters,vector<vector<float>>(filter_shape[0],vector<float>(filter_shape[1])));	

	double beg = omp_get_wtime();
	init_filters(num_filters,filter_shape,filter_bank);
	double end = omp_get_wtime();
	cout<<"Time to initialize filters: "<<end-beg<<endl;
	
	beg = omp_get_wtime();
	char filename[] = "imgs.dat";
	matrix img = load_matrix(filename,100);
	int img_shape[] = {100,784};
	end = omp_get_wtime();
	cout<<"Time to load dataset: "<<end-beg<<endl;
	/*float count=0;
	for (int i = 0; i < 6; ++i)
	{
		vector<float> v;
		for(int j=0;j<6;j++)
			v.push_back(++count);
		img.push_back(v);
	}	*/
	beg = omp_get_wtime();
	vector<matrix > final_layer = feed_through_layer(img,img_shape,filter_bank,filter_shape);
	end = omp_get_wtime();

	cout<<"Time to perform convolution: "<<end-beg<<endl;

	/*for (int i = 0; i < final_layer.size(); ++i)
	{
		print_matrix(final_layer[i]);
		cout<<endl;
	}*/


	return 0;
}
