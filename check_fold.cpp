// check_fold.c
// 
// 
// Satoshi Mochizuki
//=========================================================
using namespace std;

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <map>
#include <algorithm>

#include "check_fold.h"

int  _DEBUG_DISP3 = 0;

// -------------------------------------------------------
/**
 prototype
*/
/// �w��No�̈ʒu������
static int _srch_pos(int *pat, int len, int target );

/// 
static int _mark_pos(int sel, int from_pos, int to_pos, int &fold_no );

///
static void dump_mark(int *pat, int len);

/**
 static variable
*/
static int  *_stack[2];     // [0]:L stack [1];R stack
static int  _stack_len[2];

static FoldingState *_fs;

// -------------------------------------------------------
/**
 * �܂���`�F�b�N����
 * @param  pat_no     [in] �܂�p�^�[��No
 * @param  *pat       [in] �܂�p�^�[��
 * @param  len        [in] pat�̒���
 * @param  ret_fs     [in] ���ʊi�[�o�b�t�@
 * @param  str        [out] �܂茋��(����������)
 * @param  fs_stack0  [in] �܂�m�F�pStack (L)
 * @param  fs_stack1  [in] �܂�m�F�pStack (R)
 * @param  rev_mode	  [in] 1:�t���Œ��ׂ�
 * @retval 0      NG(�܂�Ȃ�)
 * @retval 1      OK(�܂��)
 */
int check_fold(int pat_no, int *pat, int len, FoldingState *ret_fs, string &str, 
                                int *fs_stack0, int *fs_stack1, int rev_mode )
{
int ret = 0;
int cnt;

int sel;
int cur_no;
int step;
int from,to;
int from_pos, to_pos;

int	fold_no;

	_DEBUG_DISP3 = 0;
	//if ( pat_no == 245 )	_DEBUG_DISP3 = 1;

    // ================================
    // stack����
    _stack[0] = fs_stack0;
    _stack_len[0] = 0;

    _stack[1] = fs_stack1;
    _stack_len[1] = 0;

    // ================================
	//�@MV�p�^�[�����N���A���Ă���
	str = "";

	fold_no = 0;

    // ================================
	// Stack���N���A���Ă���
    // L/R stack�ɐ擪�f�[�^��Push���Ă���
	for ( cnt = 0 ; cnt < len ; cnt ++ )
	{
	    *(_stack[0] + cnt) = 0;
	    *(_stack[1] + cnt) = 0;
	}
    _stack_len[0] = 1;
    _stack_len[1] = 1;

// debug
if ( _DEBUG_DISP3 )
{
printf( "\n");
printf(" pat = ");
for ( cnt = 0 ; cnt < len ; cnt++ )
{
	printf("%d", *(pat + cnt) );
}
}

    // ================================
    // �܂��Check����
    sel = 0;
    cnt = 0;

	cur_no = 1;
	step   = 1;
	if ( rev_mode )
	{
		 cur_no = len;
		 step   = -1;
	}
    while( cnt < (len-1) )
    {
        // ------------------------------
		from = cur_no;
		to   = from + step;

		from_pos = _srch_pos(pat, len, from);
		to_pos   = _srch_pos(pat, len, to  );

// debug
if ( _DEBUG_DISP3 )
{
printf( "\n");
printf( " from=%d(p=%d) to=%d(p=%d) L/R=%d\n", from, from_pos, to, to_pos, sel );
}
// debug

        // -------------------------------
        // from�ʒu�`to�ʒu�܂ł�mark
        // - �܂�Ȃ��ꍇ�͏I��
		ret = _mark_pos(sel, from_pos, to_pos, fold_no );
		if ( ret == 0 )
		{
			ret = 0;
			goto _term;
		}

        // -------------------------------
		// MV�p�^�[������
		if ( rev_mode == 0 )
		{
			if ( sel == 0 )
			{
				if ( from_pos < to_pos )
				{
					str += 'V';
				}
				else
				{
					str += 'M';
				}
			}
			else
			{
				if ( from_pos < to_pos )
				{
					str += 'M';
				}
				else
				{
					str += 'V';
				}
			}
		}
		else
		{
			if ( sel == 0 )
			{
				if ( from_pos < to_pos )
				{
					str += 'M';
				}
				else
				{
					str += 'V';
				}
			}
			else
			{
				if ( from_pos < to_pos )
				{
					str += 'V';
				}
				else
				{
					str += 'M';
				}
			}
		}

// debug
if ( _DEBUG_DISP3 )
{
dump_mark(pat, len);
}
// debug

        // -------------------------------
		// next
		cur_no += step;
		cnt++;
		sel = (sel + 1) & 0x1;

    }
	ret = 1;

_term:

    return(ret);
}
// -------------------------------------------------------
/**
 * �w��No�̈ʒu������
 * @param  *pat       [in] �܂�p�^�[��
 * @param  len        [in] pat�̒���
 * @param  target     [in] �����Ώ�
 * @return target�̈ʒu(0-n)
 * @retval = 0 �܂�Ȃ�
 * @retbal = 1 �܂��
 */
static int _mark_pos(int sel, int from_pos, int to_pos, int &fold_no )
{
int	cnt;
int st_pos = from_pos;
int ed_pos = to_pos;

int	mark_id;
int	wk_cnt = 0;

map <int, int> fold_no_cnt;

	// ----------------------
	// 
	if ( from_pos > to_pos )
	{
		st_pos = to_pos;
		ed_pos = from_pos;
	}

	// ----------------------
	// �܂�邩Check����
	// -  st(x)-ed(y) �� ���� fold_no ���t���Ă����OK
	//   - st(0) -- ed(0) : OK
	//   - st(1) -- ed(1) : OK
	//   - st(0) -- ed(1) : NG
	// -   st(1) -- ed(0) : NG
	mark_id = *(_stack[sel] + st_pos);
	if ( *(_stack[sel] + ed_pos) != mark_id )
	{
		return(0);
	}

	// mark_id=-1�̂Ƃ�:
	// - st()-ed()�܂ł̊ԂŁA(-1)�łȂ����𐔂���
	//   �����Ȃ�AOK�i����q��ԁj
	//   ��Ȃ�ANG (�N���X���Ă���)
	// �Ɣ��f����
	if ( mark_id == -1 )
	{
	    for ( cnt = st_pos ; cnt <= ed_pos; cnt++ )
    	{
			if ( *(_stack[sel] + cnt) != -1 )
			{
            auto itr = fold_no_cnt.find( *(_stack[sel] + cnt) );

	            if( itr != fold_no_cnt.end() )
    	        {
        	        itr->second += 1;
            	}
	            else
    	        {
        	        fold_no_cnt[*(_stack[sel] + cnt)] = 1;
            	}

			}
		}

		{
		auto itr = fold_no_cnt.begin();
			while( itr != fold_no_cnt.end() )
			{
				if( itr->second & 0x1 ) return(0);
				itr++;
			}
		}
	}

	#if 0
    for ( cnt = st_pos ; cnt <= ed_pos; cnt++ )
    {
		if ( *(_stack[sel] + cnt) != mark_id ) return(0);
	}
	#endif

	// ----------------------
	// mark��t����
	fold_no++;
	*(_stack[sel] + st_pos) = fold_no;
	*(_stack[sel] + ed_pos) = fold_no;
    //for ( cnt = st_pos ; cnt <= ed_pos; cnt++ )
    for ( cnt = st_pos + 1 ; cnt < ed_pos; cnt++ )
    {
		//if ( *(_stack[sel] + cnt) == 0 ) *(_stack[sel] + cnt) = fold_no;
		// *(_stack[sel] + cnt) = fold_no;

		if ( *(_stack[sel] + cnt) == 0 ) *(_stack[sel] + cnt) = -1;
	}

	return(1);
}

// -------------------------------------------------------
/**
 * �w��No�̈ʒu������
 * @param  *pat       [in] �܂�p�^�[��
 * @param  len        [in] pat�̒���
 * @param  target     [in] �����Ώ�
 * @return target�̈ʒu(0-n)
 * @retval 0 >= target�̈ʒu
 * @retbal 0 <  unknown
 */
static int _srch_pos(int *pat, int len, int target )
{
int cnt;

    for ( cnt = 0 ; cnt < len ; cnt++ )
    {
        if ( *(pat + cnt) == target ) return(cnt);
	}

	return(0);
}

// -------------------------------------------------------
/**
 * mark��dump����
 * @param  sel   [in] 'L' or 'R'
 *               - 0:L , 1:R
 * @param  *pat       [in] �܂�p�^�[��
 * @param  len        [in] pat�̒���
 */
static void dump_mark(int *pat, int len)
{
int cnt;

    printf( " Left  " );
    for ( cnt = 0 ; cnt < len ; cnt++ )
    {
        printf("%d ", *(_stack[0] + cnt) );
    }
    printf( "\n" );

    printf( "       " );
    for ( cnt = 0 ; cnt < len ; cnt++ )
    {
        printf("%d ", *(pat + cnt) );
	}
    printf("\n");

    printf( " Right " );
    for ( cnt = 0 ; cnt < len ; cnt++ )
    {
        printf("%d ", *(_stack[1] + cnt) );
    }
	printf("\n");

    return;
}
//=========================================================
