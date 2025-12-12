#include <cstdint>
#include <cstdio>

struct Node {
    Node* next;
    int value;
};

__attribute__((noinline))
int memory_bound(Node* head, int iters) {
    int sum = 0;
    #pragma clang loop unroll(disable)
    for (int i = 0; i < iters; i++) {
        head = head->next;
        sum += head->value;
    }
    return sum;
}

__attribute__((noinline))
float compute_bound(float x, int iters) {
    float acc = x;
    #pragma clang loop unroll(disable)
    for (int i = 0; i < iters; i++) {
        acc = acc * 1.0001f + 0.0003f;
        acc = acc * 1.0002f - 0.0001f;
        acc = acc * acc;
    }
    return acc;
}

__attribute__((noinline))
float streaming_mem(float* arr, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) sum += arr[i];
    return sum;
}

__attribute__((noinline))
float mixed_bound(Node* head, float x, int iters) {
    double acc = x;
    for (int i = 0; i < iters; i++) {
        head = head->next;              
        acc += head->value;
        acc = acc * 1.0001f + 0.0003f; 
    }
    return float(acc);
}


int main() {
    // simple driver
    const int N = 5;
    Node nodes[N];
    for (int i = 0; i < N; i++) {
        nodes[i].value = i + 1;
        nodes[i].next = &nodes[(i + 1) % N];
    }

    printf("Running memory_bound...\n");
    int s = memory_bound(&nodes[0], 5);
    printf("memory_bound result = %d\n", s);

    printf("Running compute_bound...\n");
    float r = compute_bound(1.0f, 5);
    printf("compute_bound result = %f\n", r);


    int n = 1 << 22;
    float* arr = new float[n];
    for (int i = 0; i < n; i++) arr[i] = 1.0f;

    printf("Running streaming_mem...\n");
    float sm = streaming_mem(arr, n);
    printf("streaming_mem result = %f\n", sm);

    delete[] arr;

    printf("Running mixed_bound...\n");
    float mb = mixed_bound(&nodes[0], 1.0f, 2000000);
    printf("mixed_bound result = %f\n", mb);


    return 0;
}
