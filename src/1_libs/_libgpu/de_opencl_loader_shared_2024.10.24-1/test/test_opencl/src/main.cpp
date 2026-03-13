#include <CL/cl.h>
#include <stdio.h>

int main() {
    cl_uint n = 0;
    cl_int err = clGetPlatformIDs(0, NULL, &n);
    if (err != CL_SUCCESS) {
        printf("OpenCL error: %d\n", err);
        return 1;
    }
    printf("OpenCL platforms found: %u\n", n);
    return 0;
}
