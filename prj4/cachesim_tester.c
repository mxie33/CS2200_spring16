
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "cachesim.h"
#include <getopt.h>

void print_help_and_exit(void) {
    printf("cachesim [OPTIONS] < traces/file.trace\n");
    printf("  -c C\t\tTotal size in bytes is 2^C\n");
    printf("  -b B\t\tSize of each block in bytes is 2^B\n");
    printf("  -s S\t\tNumber of blocks per set is 2^S\n");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}

void print_statistics(struct cache_stats_t* p_stats);

int main(int argc, char* argv[]) {
    double best_AAT = DBL_MAX;
    int c;
    int b;
    int s;
    int bestC;
    int bestB;
    int bestS;
    /* Read arguments */ 
    char* filename = argv[2];

    printf("%c\n", *filename);
 
    for (b = 0; b <= 6; b++) {
        for (c = 0;  c < 15; c++) {
            for (s = 0; s <= c - b; s++) {
                if (((1<<(c-b))*(((64-c-b)) + (1 << (b+3)))) <= (1<<18)){
                    FILE* fin  = fopen(filename, "r");
                    if (fin == NULL) {
                        printf("file doesn't exist error occurred\n");
                        return 0;
                    }
                    uint64_t bb = (uint64_t) b;
                    uint64_t cc = (uint64_t) c;
                    uint64_t ss = (uint64_t) s;
                    printf("-----------------------------\n");
                    printf("Cache Settings\n");
                    printf("C: %" PRIu64 "\n", cc);
                    printf("B: %" PRIu64 "\n", bb);
                    printf("S: %" PRIu64 "\n", ss);
                    printf("\n");

                    /* Setup the cache */
                    cache_init(c, s, b);

                    /* Setup statistics */
                    struct cache_stats_t stats;
                    memset(&stats, 0, sizeof(struct cache_stats_t));
                    stats.miss_penalty = 100;
                    stats.access_time = 2;

                    /* Begin reading the file */ 

                    char rw;
                    uint64_t address;
                    while (!feof(fin)) { 
                        int ret = fscanf(fin, "%c %" PRIx64 "\n", &rw, &address); 
                        if(ret == 2) {
                            cache_access(rw, address, &stats);
                        }
                    }
                    cache_cleanup(&stats);

                    print_statistics(&stats);
                    fclose(fin);
                    if (stats.avg_access_time < best_AAT) {
                        best_AAT = stats.avg_access_time;
                        bestC = c;
                        bestS = s;
                        bestB = b;
                    }
                }
            }
        }
    }
    int len = strlen(filename);
    char out[len + 4];
    sprintf(out,"%c.txt",*filename);
    FILE *outfile = fopen(out,"w");
    printf("The best average access time for %c is %f\n",*out,best_AAT);
    printf("The optimal size for C is %d, B is %d, S is %d", bestC, bestB, bestS);
    fclose(outfile);
    return 0;
}

void print_statistics(struct cache_stats_t* p_stats) {
    printf("Cache Statistics\n");
    printf("Accesses: %" PRIu64 "\n", p_stats->accesses);
    printf("Reads: %" PRIu64 "\n", p_stats->reads);
    printf("Read misses: %" PRIu64 "\n", p_stats->read_misses);
    printf("Writes: %" PRIu64 "\n", p_stats->writes);
    printf("Write misses: %" PRIu64 "\n", p_stats->write_misses);
    printf("Misses: %" PRIu64 "\n", p_stats->misses);
    printf("Writebacks: %" PRIu64 "\n", p_stats->write_backs);
    printf("Access Time: %" PRIu64 "\n", p_stats->access_time);
    printf("Miss Penalty: %" PRIu64 "\n", p_stats->miss_penalty);
    printf("Miss rate: %f\n", p_stats->miss_rate);
    printf("Average access time (AAT): %f\n", p_stats->avg_access_time);
}
