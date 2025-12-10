#include <stdio.h>

void merge(char* l, char* r, int size) {
    char temp[size];
    char* end = l + size;
    char* j = l;
    char* k = r;
    for (int i = 0; i < size; ++i)
    {
        if (j < r && (*j < *k || k > end))
        {
            temp[i] = *j;
            ++j;
        }
        else 
        {
            temp[i] = *k;
            ++k;
        }
    }
    for (int i = 0; i < size; ++i)
    {
        l[i] = temp[i];
    }
}

void mergeSort(char* data, int size) {
    if (size == 1) return;
    char* mid = data + size/2;
    mergeSort(data, size / 2);
    mergeSort(mid, size/2);
    merge(data, mid, size);
}


int main() {
    char random[50000];
    FILE* rfile = fopen("/dev/urandom", "r");
    fgets(random, sizeof(random), rfile);
    fclose(rfile);
    mergeSort(random, sizeof(random));
    for (int i = 0; i < sizeof(random);++i)
    {
        printf("%d ", random[i]);
    }
}