
// 出力結果
typedef struct _tag_FoldingState FoldingState;
struct _tag_FoldingState {
    int  no;
    char state;
};

// 折りをチェックする
int check_fold(int pat_no, int *pat, int len, FoldingState *ret_fs, string &str, 
                                int *fs_stack0, int *fs_stack1, int rev_mode );

