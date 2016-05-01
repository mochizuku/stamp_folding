// main.c
// 
// 
// Satoshi Mochizuki
//=========================================================
using namespace std;

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string> 
#include <unordered_map>
#include <map>
#include <vector>

#include "check_fold.h"

#define _TIME_DISP
//#define _DEBUG_DISP1
#define _DEBUG_DISP2


// -------------------------------------------------------
/**
 prototype
*/
// display result
static void dump_fs_rslt( FoldingState *fs, int fs_len );

// 階乗の計算
static long int factorial(int n);

// -------------------------------------------------------
/**
* argc
* argv 
* 
*/
int main(int argc, char *argv[])
{
int     n, i ,j;
int     ret;

long int num = 0;
long int rslt_ok = 0;

int *c, *pc, *q;
int k, t;

int          *perm;
FoldingState *fs;
string       fs_str;
int	max_fs_num = 0;

int  *fs_stack[2];

unordered_map<string, int>  mp_fs;
map <int, int>              rslt;

#ifdef _TIME_DISP
clock_t start, Time;
#endif

    // ================================
    // parameter check
    if(argc < 2)
    {
        fprintf(stderr, "Usage : %s   N \n", argv[0]);
        fprintf(stderr, "  N     :  number of digits\n");
        exit(0);
    }

    n = atoi(argv[1]);
    if(n < 2)
    {
        fprintf(stderr, "Error : illegal parameter input.\n");
        exit(-1);
    }

    // ================================
    // 生成した順列の格納用バッファを作成
    perm = (int *)malloc( n * sizeof(int) );
    if( perm == NULL)
    {
        fprintf(stderr, "Error : out of memory(perm).\n");
        return(-1);
    }

    fs = (FoldingState *)malloc((n-1) * sizeof(FoldingState));
    if( fs == NULL)
    {
        fprintf(stderr, "Error : out of memory(fs).\n");
        free(perm);
        return(-1);
    }

    c = (int *)malloc(sizeof(int) * n);
    if(c == NULL)
    {
        fprintf(stderr, "Error : out of memory  in genperm(c)\n");
        free(perm);
        free(fs);
        return(-1);
    }

    // ================================
    // 折り確認用stackの準備
    fs_stack[0] = (int *)malloc(n * sizeof(int));
    if(fs_stack[0] == NULL)
    {
        fprintf(stderr, "Error : out of memory(stack0).\n");
        free(perm);
        free(fs);
        free(c);
        return(-1);
    }

    fs_stack[1] = (int *)malloc(n * sizeof(int));
    if(fs_stack[1] == NULL)
    {
        fprintf(stderr, "Error : out of memory(stack1).\n");
        free(perm);
        free(fs);
        free(c);
        free(fs_stack[0]);
        return(-1);
    }

    // ================================
    #ifdef _TIME_DISP
    start = clock();
    #endif

    // ================================
    // パターン生成し折れるかCheckする
#ifdef _DEBUG_DISP1
    printf("[checking]\n");
#endif

    for(k = 1, q = perm, pc = c; k <= n; ) *q++ = *pc++ = k++;
    k = 1;
    pc = c;
    do
    {
        // -----------------------------
        t = *(perm + k);
        *(perm + k) = *(q = perm + ((k & 1)? *pc: 0));
        *q = t;

        // -----------------------------
        // 正しく折れるかCheckする
        // - 2016.1.25 add
        //   折れなかった時はmodeを変えて再度Check
        fs_str = "";
        ret = check_fold( num, perm , n, fs, fs_str, fs_stack[0], fs_stack[1], 0 );
		//if ( ret == 0 )
		//{
	    //    ret = check_fold( num, perm , n, fs, fs_str, fs_stack[0], fs_stack[1], 1 );
		//}

        if ( ret == 1 )
        {
            // ハッシュテーブル作成
            // - ハッシュに登録されている時：
            //    折り数カウントを＋１する
            // - ハッシュに登録されていない時：
            //    折り数カウントを1に初期化する
            auto itr = mp_fs.find( fs_str );
            if( itr != mp_fs.end() )
            {
                itr->second += 1;
            }
            else
            {
                mp_fs[fs_str] = 1;
            }

            rslt_ok++;
        }

        // -----------------------------
        // for debug
#ifdef _DEBUG_DISP1
        putchar(' ');
        printf("%ld:%c ", num, ret==1 ? 'P':'F' );
        printf( "S=");
        for ( j = 0 ; j < n ; j++ )
        {
            printf("%d", *(perm + j) );
        }
        //if ( ret == 1 ) dump_fs_rslt( fs, n-1 );

        if ( ret == 1 )
        {
            putchar(' ');
            printf( "F=%s", fs_str.c_str() );
        }
        putchar('\n');
#endif
        num++;

        // -----------------------------
        k = 1;
        pc = c;
        while(*pc == 0) *pc++ = k++;
        (*pc)--;

    } while(k < n);

    // ================================
    // display hash table
#ifdef _DEBUG_DISP2
    printf("[result: n=%d pass=%ld/%ld]\n", n, rslt_ok, num);

    for( auto itr = mp_fs.begin(); itr != mp_fs.end(); ++itr )
    {
        printf( " S=%s num=%d \n", itr->first.c_str(), itr->second );
		if ( max_fs_num < itr->second ) max_fs_num = itr->second;
    }
    printf( " max folding num=%d \n", max_fs_num );
#endif

    // ================================
    // display result
    // 折り方のパターン数をKeyとしたテーブルを作成
    for( auto itr = mp_fs.begin(); itr != mp_fs.end(); ++itr )
    {
        auto itr_rslt = rslt.find( itr->second );
        if( itr_rslt != rslt.end() )
        {
            itr_rslt->second += 1;
        }
        else
        {
            rslt[itr->second] = 1;
        }
    }

    printf( "\n");
    printf("[result: n=%d pass=%ld/%ld]\n", n, rslt_ok, num);
    for( auto itr = rslt.begin(); itr != rslt.end(); ++itr )
    {
        printf( " x=%d y=%d\n", itr->first, itr->second );
    }

    // ================================
    free((void *)c);
    free( perm );
    free( fs );
    free(fs_stack[0]);
    free(fs_stack[1]);

    // ================================
    #ifdef _TIME_DISP
#ifdef __CYGWIN__
    printf("time=%f[sec] \n", (double)Time / CLK_TCK);
#else
    printf("time=%f[sec] \n", (double)Time / CLOCKS_PER_SEC);
#endif
#endif

    return 1;
}

// -------------------------------------------------------
/**
 * 階乗を計算する
 * @param  n    [in] 整数 
 * @return 階乗
 */
static long int factorial(int n)
{
long int result = 1;
long int k;

    for(k = 1; k <= n; k++)
    {
        result *= k;
    }

    return( result );
}
// -------------------------------------------------------
/**
 * 結果をdumpする
 * @param  none
 */
static void dump_fs_rslt( FoldingState *fs, int fs_len )
{
int cnt;

    printf( " ");
    printf( "F=");
    for ( cnt = 0 ; cnt < fs_len ; cnt++ )
    {
        #if 0
        printf( "%d-(%c)-%d ", fs[cnt].no
                             , fs[cnt].state
                             , fs[cnt].no+1 );
        #else
        printf( "%c", fs[cnt].state );
        #endif
    }
    //printf( "]");

    return;
}
