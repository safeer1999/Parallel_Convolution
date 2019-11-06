__global__ void hello(){
    printf("hello");
}

void main(){
    hello<<<1,1>>>();
    return 0;
}