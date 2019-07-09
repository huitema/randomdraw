// rnadomdrawvs.c : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "md5.h"

#define MD5_NB_TESTS 7

char const * md5_test_input[MD5_NB_TESTS] = {
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};

unsigned char const md5_test_out0[16] = {
    0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e };

unsigned char const md5_test_out1[16] = {
    0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8, 0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 };

unsigned char const md5_test_out2[16] = {
    0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 };

unsigned char const md5_test_out3[16] = {
    0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 };

unsigned char const md5_test_out4[16] = {
    0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00, 0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b };

unsigned char const md5_test_out5[16] = {
    0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5, 0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f };

unsigned char const md5_test_out6[16] = {
    0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55, 0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a };

unsigned char const * md5_test_output[MD5_NB_TESTS] = {
    md5_test_out0, 
    md5_test_out1,
    md5_test_out2,
    md5_test_out3,
    md5_test_out4,
    md5_test_out5,
    md5_test_out6
};

unsigned char const mod_test_0[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char const mod_test_1[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102
};

unsigned char const mod_test_2[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 0
};

#define NB_MOD_TEST 4

unsigned char const * mod_test_input[NB_MOD_TEST] = {
    mod_test_0,
    mod_test_1,
    mod_test_2,
    md5_test_out0
};

const int mod_test_output[NB_MOD_TEST] = {
    0,
    1,
    54,
    5
};

int mod_long(int denom, unsigned char const * oct, int nb_oct)
{
    int m = 0;

    for (int i = 0; i < nb_oct; i++) {
        m *= 256;
        m += oct[i];
        m %= denom;
    }

    return m;
}

int mod_test()
{
    int ret = 0;
    for (int i = 0; i < NB_MOD_TEST; i++) {
        int m = mod_long(101, mod_test_input[i], 16);

        if (m != mod_test_output[i]) {
            fprintf(stderr, "Mod test %d returns %d instead of %d\n", i, m, mod_test_output[i]);
            ret = -1;
        }
    }

    return ret;
}

int md5_test()
{
    int ret = 0;

    for (int i = 0; i < MD5_NB_TESTS; i++) {
        MD5_CTX md5_ctx;
        unsigned char md5_result[16];
        MD5Init(&md5_ctx);
        MD5Update(&md5_ctx, md5_test_input[i], strlen(md5_test_input[i]));
        MD5Final(md5_result, &md5_ctx);

        if (memcmp(md5_result, md5_test_output[i], 16) != 0) {
            fprintf(stderr, "Test number %d fails.\n", i);
            ret = -1;
        }
    }

    return ret;
}

void randomdraw(int nb_num, int range, unsigned char * hashes, int * results, int * sorted, char const * init_string)
{
    MD5_CTX md5_ctx;
    int i_round = 0;
    unsigned char const * x = (unsigned char const *)init_string;
    int lx = (int)strlen(init_string);

    do {
        int m;
        int i_res;

        MD5Init(&md5_ctx);
        MD5Update(&md5_ctx, x, lx);
        MD5Final(&hashes[16 * i_round], &md5_ctx);
        x = &hashes[16 * i_round];
        lx = 16;
        m = mod_long(range - i_round, &hashes[16 * i_round], 16);
        for (i_res = 0; i_res < i_round; i_res++) {
            if (sorted[i_res] <= m) {
                m++;
            }
            else {
                break;
            }
        }
        results[i_round] = m;

        for (int i = i_round; i > i_res; i--) {
            sorted[i] = sorted[i - 1];
        }
        sorted[i_res] = m;
        i_round++;
    } while (i_round < nb_num);
}

int draw_test_expected[8] = { 0, 5, 7, 6, 3, 4, 2, 1 };
char const * draw_test_init = "draw test";

int draw_test()
{
    int ret = 0;
    int sorted[8];
    int result[8];
    unsigned char hashes[18 * 8];

    for (int n = 1; ret == 0 && n < 9; n++) {
        randomdraw(n, 9, hashes, result, sorted, draw_test_init);
        /* Check sorted */
        for (int i = 1; ret == 0 && i < n; i++) {
            if (sorted[i] <= sorted[i - 1]) {
                fprintf(stderr, "Draw test %d, sort failed.\n", n);
                ret = -1;
            }
        }
        /* Check expected */
        for (int i = 0; ret == 0 && i < n; i++) {
            if (result[i] != draw_test_expected[i]) {
                fprintf(stderr, "Draw test %d, results[%d] don't match.\n", n, i);
                ret = -1;
            }
        }
    }

    return ret;
}

int main(int argc, char ** argv)
{
    int ret = md5_test();
    int nb_num = 0;
    int n_low = 0;
    int n_high = 0;
    char const * init_string = NULL;
    int * results = NULL;
    int * sorted = NULL;
    unsigned char * hashes = NULL;

    if (ret < 0) {
        fprintf(stderr, "MD5 test fails\n");
    }
    else {
        ret = mod_test();
        if (ret < 0) {
            fprintf(stderr, "Mod test fails\n");
        }
        else {
            ret = draw_test();
            if (ret < 0) {
                fprintf(stderr, "Draw test fails\n");
            }
        }
    }

    if (ret == 0) {
        if (argc != 5) {
            ret = -1;
        }
        else {
            nb_num = atoi(argv[1]);
            n_low = atoi(argv[2]);
            n_high = atoi(argv[3]);
            init_string = argv[4];

            if (nb_num < 0 || n_low < 0 || n_high < 0 ||
                (nb_num > (n_high - n_low + 1))) {
                ret = -1;
            }
            else {
                results = (int *)malloc(nb_num * sizeof(int));
                sorted = (int *)malloc(nb_num * sizeof(int));
                hashes = (unsigned char *)malloc(nb_num * 16);

                if (hashes == NULL || results == NULL) {
                    fprintf(stderr, "Cannot allocate memory for %d results\n", nb_num);
                    ret = -1;
                }
            }
        }
        if (ret != 0) {
            fprintf(stderr, "Usage: %s <nb draws> <first in range> <last in range> <string>\n", argv[0]);
        }
        else {
            printf("Picking %d numbers between %d and %d, using MD5(%s)\n",
                nb_num, n_low, n_high, init_string);
        }
    }

    if (ret == 0) {
        int range = n_high - n_low + 1;

        randomdraw(nb_num, range, hashes, results, sorted, init_string);

        for (int i_round = 0; i_round <nb_num; i_round ++){
            printf("Rank %d: %d -- md5=", i_round + 1, results[i_round] + n_low);
            for (int i = 0; i < 16; i++) {
                printf("%02x", hashes[16 * i_round + i]);
            }
            printf("\n");
        };
    }

    if (results != NULL) {
        free(results);
    }

    if (sorted != NULL){
        free(sorted);
    }

    if (hashes != NULL) {
        free(hashes);
    }

    return ret;
}
