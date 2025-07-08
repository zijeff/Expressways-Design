#include "state.h"

void init_State(struct State *s)
{
    for (int i = 0; i < s->n; i++) // s->n代表区域总数
    {
        s->adjlist[i].number = i + 1;  // 将下标转换为从1开始计数
        s->adjlist[i].firstarc = NULL; // 将初始的指针置为空
        s->adjlist[i].val = 0;         // 初始的产业总值置为0
    }
}
void delete_State(struct State *s)
{
    // 遍历所有的区域（顶点）
    for (int i = 0; i < s->n; i++)
    {
        ArcNode *p = s->adjlist[i].firstarc; // 获取当前顶点的边链表头
        while (p != NULL)
        {
            ArcNode *temp = p; // 存储当前边节点
            p = p->nextarc;      // 移动到下一个边节点
            free(temp);          // 释放当前边节点的内存
        }
    }
    // 释放邻接表数组内存
    free(s->adjlist);
}
void parse(struct State *s, struct PNG *p)
{
    // TODO
}
int solve1(struct State *s)
{

    return 0;
}
int solve2(struct State *s)
{
    // TODO
    return 0;
}
