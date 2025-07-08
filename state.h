#ifndef STATE_H_
#define STATE_H_
#include "suan_png.h"
#include "pxl.h"

typedef struct ANode{
    int number;//编号
    int val;//产业值
    struct ANode *nextarc;//指向下一条边对应的节点
}ArcNode;

typedef struct headnode{
    int number;//编号
    int val;//产业值
    ArcNode *firstarc;//指向相邻的节点
}VNode;

struct State {
    VNode *adjlist;//所有的顶点
    int n;//区域总数
};

// function
void init_State(struct State *s);
void delete_State(struct State *s);
void assign(struct State *a, struct State *b);
void parse(struct State *s, struct PNG *p);
int solve1(struct State *s);
int solve2(struct State *s);

#endif
