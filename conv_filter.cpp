#include <vector>
#include <iostream>
#include <random>

class Convolution
{
public:
	Convolution();
	~Convolution();
	
};

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
	int new_col_len = (img_shape[1]-filter_shape[1])/stride;

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





int main(int argc, char const *argv[])
{
	int num_filters = 4;
	int filter_shape[] = {3,3};

	std::vector<std::vector<std::vector<float> >> filter_bank(num_filters,std::vector<std::vector<float>>(filter_shape[0],std::vector<float>(filter_shape[1])));	

	init_filters(num_filters,filter_shape,filter_bank);

	// displays filters
	for (int i=0; i<num_filters; ++i)
	{
		for (int j = 0; j < filter_shape[0]; ++j)
		{
			for (int k = 0; k < filter_shape[1]; ++k)
			{
					std::cout<<filter_bank[i][j][k]<<" ";
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}

	// applying filters, intermediate output
	std::vector<std::vector<float> > v1 = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}} ;
	std::vector<std::vector<std::vector<float> >>  v_bank = {{{1,2,3},{4,5,6},{7,8,9}},{{2,4,6},{8,10,12},{14,16,18}}} ;
	int img_shape[] = {4,4};
	std::vector<std::vector<std::vector<float> > > convolved_layer = apply_filter(v1,img_shape,v_bank,filter_shape);

	for (int i = 0; i < convolved_layer.size(); ++i)
	{
		for (int j = 0; j < convolved_layer[0].size(); ++j)
		{
			for (int k = 0; k < convolved_layer[0][0].size(); ++k)
			{
				std::cout<<convolved_layer[i][j][k]<<" ";
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}

	/* std::cout<<matrix_sum(v2)<<std::endl;

	std::vector<std::vector<float> > product = matrix_multiply(v1,1,1,v2,1,1,2,2);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			std::cout<<product[i][j]<<" ";
		}
		std::cout<<std::endl;
	} */

	return 0;
}