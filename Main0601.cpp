#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#define length 1000000
#define samplesize 1000
#define BLOCK_SIZE 500
#define BUF_SIZE (BLOCK_SIZE / 8 + 1)
typedef unsigned char byte;

char g_bitSrc[2 * BLOCK_SIZE] = {0};
byte g_byteSrc[2 * BLOCK_SIZE / 8] = {0};

char g_bitDst[length * samplesize] = {0};
byte g_byteDst[length * samplesize / 8] = {0};

//This Module is to adjust a block of sequence with the prescribed Linear complexity Profile

class State {
public:
    State() {}
    State(char *src) {
        for (int i = 0; i < BLOCK_SIZE; i++)
            q[i] = src[i];
    }
    ~State() {}
    void Init()
    {
        N = 0;
        m = -1;
        L = 0;
        memset(B, 0, sizeof(B));
        memset(C, 0, sizeof(C));
        B[0] = C[0] = 1;
    }
    void OneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];
        if (d) {
            if (N + 1 - L > L) {
                char Temp[BLOCK_SIZE + 1];
                for (int i = 1; i <= L; i++)Temp[i] = C[i];
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
                for (int i = 1; i <= L; i++)B[i] = Temp[i];
                L = N + 1 - L;
                m = N;
            } else {
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
            }
        }
        N++;
    }
    /*pull down the point that is above the line y=0.5x*/
    void PullDownOneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];

        q[N] ^= d;

        OneRound();
    }
    /*push up the point that is below the line y=0.5x*/
    void PushUpOneRound()
    {
        char d = q[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & q[N - i];

        q[N] ^= ~d;

        OneRound();
    }
    /*whether the point is on the line y=0.5x*/
    bool IsInSlash()
    {
        return 2 * L == N;
    }

    void ShowCurrentLCP(int len)
    {
        Init();
        for (int i = 0; i < len; i++) {
            OneRound();
            printf("(%3d, %3d)%c", N, L, (i + 1) % 10 == 0 ? '\n' : ' ');
        }
    }
    void ShowCurrentPoint()
    {
        printf("(%3d, %3d)\n", N, L);
    }
public:
    int N;
    int m;
    int L;
    char B[BLOCK_SIZE + 1];
    char C[BLOCK_SIZE + 1];
    char q[BLOCK_SIZE];
};

bool AutomicCheck(int *y, int point, int L) {
    if (y[point] != L) {
        printf("***ERROR*** check error, x = %d, y = %d\n", point, L);
        return false;
    }
    return true;
}

void CheckLCP(State &St, int nfirst, int nsecond, int mm, int gap) {
    int y[BLOCK_SIZE + 1] = {0};
    int frontJumps = 0;
    
    int frontLen = BLOCK_SIZE - nfirst * (2 + 4) - nsecond * 2 - 2 * mm - 1 * 2 - gap;

    St.Init();
    for (int i = 0 ; i < BLOCK_SIZE; i++) {
        St.OneRound();
        y[i + 1] = St.L;
        if (y[i + 1] > y[i] && i < frontLen) {
            frontJumps++;
        }
    }


    int curCheckpoint = frontLen;
    int curExpectL = frontLen / 2;
    //printf("frontLen = %d\n", frontLen);
    if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
        return;
    }

    if (frontJumps % 2 == 0) {
        // printf("even case\n");
        for (int i = 0; i < nfirst; i++) {
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 2;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }


            curCheckpoint++;
            curExpectL += 1;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }

        for (int i = 0 ; i < nsecond; i++) {
            curCheckpoint++;
            curExpectL += 1;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }

        for (int i = 0; i < mm - 1; i++) {
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }
        curCheckpoint++;
        curExpectL += mm;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }
        for (int i = 0; i < mm; i++) {
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }

        curCheckpoint++;
        curExpectL += 1;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }
        curCheckpoint++;
        curExpectL += 0;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }
    } else {
        // printf("odd case\n");
        for (int i = 0; i < nfirst; i++) {
            curCheckpoint++;
            curExpectL += 1;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }

            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 2;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }

        for (int i = 0 ; i < nsecond; i++) {
            curCheckpoint++;
            curExpectL += 1;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }

        curCheckpoint++;
        curExpectL += 1;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }
        curCheckpoint++;
        curExpectL += 0;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }

        for (int i = 0; i < mm - 1; i++) {
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }
        curCheckpoint++;
        curExpectL += mm;
        if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
            return;
        }
        for (int i = 0; i < mm; i++) {
            curCheckpoint++;
            curExpectL += 0;
            if (!AutomicCheck(y, curCheckpoint, curExpectL)) {
                return;
            }
        }
    }
}

// Calculate the jump comlexity of a sequence
int JumpComplexity(char *s, int len) {
    if (len <= 0) {
        return 0;
    }

    int jumps = 0;

    int N;
    int m;
    int L;
    char B[BLOCK_SIZE + 1];
    char C[BLOCK_SIZE + 1];
    char T[BLOCK_SIZE + 1];

    m = -1;
    L = 0;
    memset(B, 0, sizeof(B));
    memset(C, 0, sizeof(C));
    B[0] = C[0] = 1;

    for (N = 0; N < len; N++) {
        char d = s[N];
        for (int i = 1; i <= L; i++)d ^= C[i] & s[N - i];
        if (d) {
            if (N + 1 - L > L) {
                char Temp[BLOCK_SIZE + 1];
                for (int i = 1; i <= L; i++)Temp[i] = C[i];
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
                for (int i = 1; i <= L; i++)B[i] = Temp[i];
                L = N + 1 - L;
                m = N;
                jumps++;
            } else {
                for (int i = N - m; i <= N + 1 - L; i++)C[i] ^= B[i - (N - m)];
            }
        }
    }

    return jumps;
}

/*
 * fp: file descriptor of the source sequence
 */
 // Generate a sequence with each block having the prescribed linear complexity profile 
void MakeSequence(FILE *fp, int nfirst, int nsecond, int mm, int gap) {
    State curState;
    State backState;
    int frontlen = BLOCK_SIZE - nfirst * (2 + 4) - nsecond * 2 - 2 * mm - 1 * 2 - gap;
    int tmpLen = 0;
    printf("=========================================================================\n");
    printf("nfirst = %d\n", nfirst);
    printf("nsecond = %d\n", nsecond);
    printf("mm = %d\n", mm);
    printf("frontlen = %d\n", frontlen);
    printf("=========================================================================\n");

    for (int loop = 0; loop < 1; loop++) {

        if (loop % 2 == 0) {
            if (2 * BLOCK_SIZE / 8 != fread(g_byteSrc, 1, 2 * BLOCK_SIZE / 8, fp)) {
                printf("***ERROR*** fread failed\n");
                return;
            }
            for (int i = 0; i < 2 * BLOCK_SIZE / 8; i++) {
                for (int j = 0; j < 8; j++) {
                    g_bitSrc[8 * i + j] = (g_byteSrc[i] & 0x80) ? 1 : 0;
                    g_byteSrc[i] <<= 1;
                }
            }
        }
        for (int i = 0; i < BLOCK_SIZE; i++) {
            curState.q[i] = loop % 2 == 0 ? g_bitSrc[i] : g_bitSrc[i + 500];
        }

        
        /*initialize the Berlekamp-Massey algorithm*/
        curState.Init();
        backState = curState;
        for (int i = 0; i < frontlen; i++) {
            curState.OneRound();
            if (curState.IsInSlash()) {
                backState = curState;
            }
        }

        curState = backState;

        tmpLen = (frontlen - curState.N) / 2;
        if (tmpLen > 0) {
            for (int i = 0; i < tmpLen - 1; i++) {
                curState.OneRound();
            }
            curState.PushUpOneRound();
            for (int i = 0; i < tmpLen; i++) {
                curState.OneRound();
            }
        }
        curState.ShowCurrentLCP(frontlen);
        printf("after\n\n\n");

        // 此处需根据前frontLen的跳跃数的奇偶性来调整
        if (JumpComplexity(curState.q, frontlen) % 2 == 0) { // 前frontLen的跳跃数为偶数
            printf("Come here\n");
            curState.PullDownOneRound();

            curState.ShowCurrentPoint();
            
            curState.PushUpOneRound();
            curState.OneRound();
            curState.OneRound();

            curState.PushUpOneRound();
            curState.OneRound();


            for (int i = 0; i < nfirst - 1; i++) {
                curState.PullDownOneRound();
                
                curState.PushUpOneRound();
                curState.OneRound();
                curState.OneRound();

                curState.PushUpOneRound();
                curState.OneRound();
            }

            for (int i = 0; i < nsecond; i++) {
                curState.PushUpOneRound();
                curState.OneRound();
            }
            
            for (int i = 0; i < mm - 1; i++) {
                curState.PullDownOneRound();
            }
            curState.PushUpOneRound();
            for(int i = 0; i < mm; i++) {
                curState.OneRound();
            }

            
            curState.PushUpOneRound();
            curState.OneRound();
            


        } else {
            for (int i = 0; i < nfirst; i++) {
                curState.PushUpOneRound();
                curState.OneRound();

                curState.PullDownOneRound();
                curState.PushUpOneRound();
                curState.OneRound();
                curState.OneRound();
            }

            for (int i = 0; i < nsecond; i++) {
                curState.PushUpOneRound();
                curState.OneRound();
            }
            
            
            curState.PushUpOneRound();
            curState.OneRound();
            

            for (int i = 0; i < mm - 1; i++) {
                curState.PullDownOneRound();
            }
            curState.PushUpOneRound();    
        }

Assign:
        //CheckLCP(curState, nfirst, nsecond, mm, gap);
        curState.ShowCurrentLCP(BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            g_bitDst[BLOCK_SIZE * loop + i] = curState.q[i];
        }
    }
}

void ConvertBitsToBytes(char *bitSrc, byte *byteDst, int bitLen)
{
    for (int i = 0; i < bitLen / 8; i++) {
        byteDst[i] = 0;
        for(int j = 0; j < 8; j++) {
            byteDst[i] <<= 1;
            if (8 * i + j < bitLen) {
                byteDst[i] ^= bitSrc[8 * i + j];
            }
        }
    }
}

void ConvertBytesToBits(byte *byteSrc, char *bitDst, int bitLen)
{
    for (int i = 0; i < bitLen / 8; i++) {
        byte tmp = byteSrc[i];
        for(int j = 0; j < 8; j++) {
            if (8 * i + j < bitLen) {
                bitDst[8 * i + j] = (tmp >> 7);
                tmp <<= 1;
            }
        }
    }
}



int main(int argc, char **argv) {
    FILE *fp_in = fopen("rdseed1.data", "rb");
    if (fp_in == NULL) {
        printf("fopen file rdseed1.data failed\n");
        return 1;
    }
    
    MakeSequence(fp_in, 46, 64, 22, 10);  // Generate a block of sequence with the prescribed linear complexity profile, where the parameters are the number of PCTs like T1, T2 ...
    fclose(fp_in);

    /*
    for (int i = 0; i < samplesize * length / 8; i++) {
        for (int j = 0; j < 8; j++) {
            g_byteDst[i] <<= 1;
            g_byteDst[i] ^= g_bitDst[8 * i + j];
        }
    }
    
    
    FILE *fp_out = fopen("OhsTestSample.data", "wb");
    if (fp_out == NULL) {
        printf("fopen file %s failed\n", "OhsTestSample.data");
        return 1;
    }

    if (samplesize * length / 8 != fwrite(g_byteDst, 1, samplesize * length / 8, fp_out)) {
        printf("fwrite file %s failed\n", "OhsTestSample.data");
        fclose(fp_out);
        return 1;
    }

    fclose(fp_out);*/

    /*FILE *fp_out = fopen("OhsTestSample.data", "rb");
    if (fp_out == NULL) {
        printf("fopen file %s failed\n", "OhsTestSample.data");
        return 1;
    }
    fread(g_byteSrc, 1, BLOCK_SIZE * 2, fp_out);
    fclose(fp_out);

    ConvertBytesToBits(g_byteSrc, g_bitSrc, BLOCK_SIZE * 2);
    State St(g_bitSrc);
    St.Init();
    St.ShowCurrentLCP();
    printf("%d\n", JumpComplexity(g_bitSrc, 40));*/

    printf("success\n");
    return 0;
}
