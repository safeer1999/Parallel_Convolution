#include <vector>
#include <iostream>
#include <random>
#include <ctime>
#include <math.h>
class Convolution
{
public:
	Convolution();
	~Convolution();
	
};

void print_matrix(std::vector<std::vector<float> > a,int beg_row=0,int beg_col=0,int row=0,int col=0)
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
			std::cout<<a[beg_row+i][beg_col+j]<<" ";
		}
		std::cout<<std::endl;
	}
}

void init_filters(int num_filters,int filter_shape[], std::vector<std::vector<std::vector<float> > > &filter_bank)
{
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0,0.1);

    
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

std::vector<std::vector<float> > matrix_multiply(std::vector<std::vector<float> > a,int a_beg_row,int a_beg_col,std::vector<std::vector<float> > b, int b_beg_row,int b_beg_col  ,int row,int col)
{
	std::vector<std::vector<float> > product(row,std::vector<float>(col,0) );

	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			product[i][j] = a[a_beg_row+i][a_beg_col+j]*b[b_beg_row+i][b_beg_col+j] ;

			//std::cout<<product[i][j]<<" ";
		}
		//std::cout<<std::endl;
	}

	return product;
}

float matrix_sum(std::vector<std::vector<float> > a)
{
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

std::vector<std::vector<float> > convolve(std::vector<std::vector<float> > img, int img_shape[],std::vector<std::vector<float> > filter, int filter_shape[], int stride)
{

	std::vector<std::vector<float> > filtered_img;

	for (int i = 0; i <= img_shape[0]-filter_shape[0]; i+=stride)
	{
		std::vector<float> v;
		for (int j = 0; j <= img_shape[1]-filter_shape[1]; j+=stride)
		{
			float masked_values = matrix_sum(matrix_multiply(img,i,j,filter,0,0,filter_shape[0],filter_shape[1]));
			v.push_back(masked_values);
		}
		filtered_img.push_back(v);

	}

	return  filtered_img;
}



std::vector<std::vector<std::vector<float> > > apply_filter(std::vector<std::vector<float> >  img,int img_shape[],std::vector<std::vector<std::vector<float> > >  filter_bank,int filter_shape[])
{
	std::vector<std::vector<std::vector<float> > > convolved_layer;
	for (int i = 0; i < filter_bank.size(); ++i)
	{
		convolved_layer.push_back(convolve(img,img_shape,filter_bank[i],filter_shape,1));
	}

	return convolved_layer;
}


float matrix_max(std::vector<std::vector<float> > a,int beg_row,int beg_col, int row,int col)
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

std::vector<std::vector<float> > apply_maxPool(std::vector<std::vector<float> >  img,int img_shape[],int stride,int pool_dim)
{
	std::vector<std::vector<float> > pooled_img;

	for (int i = 0; i <= img_shape[0]-pool_dim; i+=stride)
	{
		std::vector<float> v;
		for (int j = 0; j <= img_shape[1]-pool_dim; j+=stride)
		{
			//std::cout<<i<<' '<<j<<std::endl;
			float max_val = matrix_max(img,i,j,pool_dim,pool_dim);
			v.push_back(max_val);

		}
		pooled_img.push_back(v);
	}

	return pooled_img;



}

std::vector<std::vector<std::vector<float> > > apply_maxPool_to_filters(std::vector<std::vector<std::vector<float> > >  prev_layer,int img_shape[])
{
	std::vector<std::vector<std::vector<float> > > pooled_layer;

	for (int i = 0; i < prev_layer.size(); ++i)
	{
		std::vector<std::vector<float> > v =  apply_maxPool(prev_layer[i],img_shape,2,2);
		pooled_layer.push_back(v);
	}

	return pooled_layer;
}

float reLU(float x)
{
	return x>0 ? x : 0;
}

void apply_activation(std::vector<std::vector<float> > &inp)
{
	for (int i = 0; i < inp.size(); ++i)
	{
		for (int j = 0; j < inp[0].size(); ++j)
		{
			inp[i][j] = reLU(inp[i][j]);
		}
	}
}

std::vector<std::vector<std::vector<float> > > feed_through_layer(std::vector<std::vector<float> > img, int img_shape[], std::vector<std::vector<std::vector<float> > > filter_bank,int filter_shape[])
//void feed_through_layer(std::vector<std::vector<float> > img, int img_shape[], std::vector<std::vector<std::vector<float> > > filter_bank, int filter_shape[])
{
	std::vector<std::vector<std::vector<float> > > temp = apply_filter(img,img_shape,filter_bank,filter_shape);
	
	
	for(int i=0;i<temp.size();i++)
		apply_activation(temp[i]);

	int filtered_img_shape[] = {temp[0].size(),temp[0][0].size()};
	return apply_maxPool_to_filters(temp,filtered_img_shape);
}

int main(int argc, char const *argv[])
{
	int num_filters = 4;
	int filter_shape[] = {3,3};
	int img_shape[] = {6,6};


	std::vector<std::vector<std::vector<float> >> filter_bank(num_filters,std::vector<std::vector<float>>(filter_shape[0],std::vector<float>(filter_shape[1])));	

	init_filters(num_filters,filter_shape,filter_bank);

	std::vector<std::vector<float> > img;
	float count=0;
	for (int i = 0; i < 6; ++i)
	{
		std::vector<float> v;
		for(int j=0;j<6;j++)
			v.push_back(++count);
		img.push_back(v);
	}	


	std::vector<std::vector<std::vector<float> > > final_layer = feed_through_layer(img,img_shape,filter_bank,filter_shape);

	for (int i = 0; i < final_layer.size(); ++i)
	{
		print_matrix(final_layer[i]);
		std::cout<<std::endl;
	}

	return 0;
}
