#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/sysinfo.h>

using namespace std;

struct thread_work_t{
    long unsigned d;
    long unsigned start_n;
    long unsigned end_n;
    long unsigned int *digits;
};

void *sum(void *thread_work_uncasted) {
    struct thread_work_t *thread_work = (struct thread_work_t*)thread_work_uncasted;
    const long unsigned int d = thread_work->d;
    const long unsigned int start_n = thread_work->start_n;
    const long unsigned int end_n = thread_work->end_n;
    long unsigned int *digits = thread_work->digits;

    // intermediate value
    for (long unsigned int digit = 0; digit < d + 11; ++digit) {
        digits[digit] = 0;
    }
    // main loop, calculate for each i the value of 1/i to a precesion of d
    for (long unsigned int i = start_n; i <= end_n; ++i) {
        long unsigned int remainder = 1;
        for (long unsigned int digit = 0; digit < d + 11 && remainder; ++digit) {
            long unsigned int div = remainder / i;
            long unsigned int mod = remainder % i;
            digits[digit] += div;
            remainder = mod * 10;
        }
    }
}

void generate_output(long unsigned int *digits, int d, char* output) {
    // move values bigger than 10 up
    for (long unsigned int i = d + 11 - 1; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }
    // round last digit
    if (digits[d + 1] >= 5) {
        ++digits[d];
    }
    // move values bigger than 10 up
    for (long unsigned int i = d; i > 0; --i) {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }
    // print result into output
    stringstream stringstreamA;
    stringstreamA << digits[0] << ".";
    for (long unsigned int i = 1; i <= d; ++i) {
        stringstreamA << digits[i];
    }
    stringstreamA << '\0';
    string stringA = stringstreamA.str();
    stringA.copy(output, stringA.size());
}

int main() {
    // Determine the amount of available CPUs
    int cpus = get_nprocs();
    // nprocs() might return wrong amount inside of a container.
    // Use MAX_CPUS instead, if available.
    if (getenv("MAX_CPUS")) {
        cpus = atoi(getenv("MAX_CPUS"));
    }
    // Sanity-check
    assert(cpus > 0 && cpus <= 64);
    fprintf(stderr, "Running on %d CPUs\n", cpus);

    long unsigned int d = 1, n = 1; //cpus = 1;

    // read input
    cin >> d >> n;

    long unsigned int digits[cpus][d+11];
    struct thread_work_t tw[cpus];
    pthread_t thread[cpus];

    for (int i=0; i < cpus; i++) {
        tw[i].digits  = digits[i];
        tw[i].start_n = (n/cpus)* i    + 1;
        tw[i].end_n   = (n/cpus)*(i+1);
        tw[i].d       = d;

        fprintf(stderr, "Starting thread %d from %ld to %ld\n", i, tw[i].start_n, tw[i].end_n);
    
        pthread_create(&thread[i], NULL, sum, (void*)&tw[i]);
    }

    // main thread takes care of the leftover
    struct thread_work_t maintw;
    long unsigned int mdigits[d+11] = {0};
    maintw.digits  = mdigits;
    maintw.start_n = (n/cpus) * cpus + 1;
    maintw.end_n   = n;
    maintw.d       = d;

    sum((void*)&maintw);

    for (int i=0; i<cpus; i++) {
        // wait for all threads
        pthread_join(thread[i], NULL);
        for (int j=0; j<=d+11; j++) {
            mdigits[j] += tw[i].digits[j];
        }
    }

    // allocate output buffer
    char output[d + 10]; // extra precision due to possible error
    generate_output(mdigits, d, output);

    cout << output << endl;

    return 0;
}
