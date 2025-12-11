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

    return 0;
}
