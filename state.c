#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "state.h"
#include <limits.h>

#define INF INT_MAX

void init_State(struct State *s)
{
    for (int i = 0; i < s->n; i++) // s->n????????????
    {
        s->adjlist[i].number = i + 1;  // ???±???????1???????
        s->adjlist[i].firstarc = NULL; // ???????????????
        s->adjlist[i].val = 0;         // ?????????????0
    }
}
void delete_State(struct State *s)
{
    // ???????е????????
    for (int i = 0; i < s->n; i++)
    {
        ArcNode *p = s->adjlist[i].firstarc; // ?????????????????
        while (p != NULL)
        {
            ArcNode *temp = p; // ?洢???????
            p = p->nextarc;      // ??????????????
            free(temp);          // ?????????????
        }
    }
    // ??????????????
    free(s->adjlist);
}
void parse(struct State *s, struct PNG *p)
{
    // 检查输入参数
    if (!s || !p || !p->image || p->width <= 0 || p->height <= 0) 
    {
        printf("Error: 无效的输入参数\n");
        return;
    }
    int width = p->width;
    int height = p->height;
    int total_nodes = width * height;
    // 设置区域总数
    s->n = total_nodes;
    // 检查区域总数是否有效
    if (total_nodes <= 0) 
    {
        printf("Error: 区域总数无效\n");
        return;
    }
    // 分配并初始化邻接表
    s->adjlist = (VNode *)malloc(total_nodes * sizeof(VNode));
    if (!s->adjlist) 
    {
        printf("Error: 内存分配失败\n");
        return;
    }
    // 初始化所有顶点的邻接表头指针为NULL
    for (int i = 0; i < total_nodes; i++) 
    {
        s->adjlist[i].firstarc = NULL;
        s->adjlist[i].number = i + 1; // 顶点编号从1开始
    }
    // 计算每个顶点的产业值
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            int node_id = y * width + x;
            PXL* px = get_PXL(p, x, y);
            if (!px) continue;
            // 计算产业值
            int R = px->red;
            int G = px->green;
            int B = px->blue;
            s->adjlist[node_id].val = 255*255*3 - R*R - G*G - B*B;
        }
    }
    // 构建邻接表（只添加单向边，避免重复）
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++)
        {
            int node_id = y * width + x;
            // 方向数组：只考虑右方和下方（避免重复添加边）
            int dirs[2][2] = {{0, 1}, {1, 0}};
            // 检查两个方向的邻居
            for (int d = 0; d < 2; d++) 
            {
                int ny = y + dirs[d][0];
                int nx = x + dirs[d][1];
                // 检查邻居是否在地图范围内
                if (ny >= 0 && ny < height && nx >= 0 && nx < width) 
                {
                    int neighbor_id = ny * width + nx;
                    // 创建新的邻接表节点（当前节点 -> 邻居节点）
                    ArcNode* new_arc = (ArcNode*)malloc(sizeof(ArcNode));
                    if (!new_arc) 
                    {
                        printf("Error: 内存分配失败\n");
                        continue;
                    }
                    // 设置边的信息
                    new_arc->number = neighbor_id + 1;  // 顶点编号从1开始
                    new_arc->val = s->adjlist[neighbor_id].val;  // 邻居的产业值
                    // 将新节点插入邻接表头部
                    new_arc->nextarc = s->adjlist[node_id].firstarc;
                    s->adjlist[node_id].firstarc = new_arc;
                    // 创建反向边（邻居节点 -> 当前节点）
                    ArcNode* reverse_arc = (ArcNode*)malloc(sizeof(ArcNode));
                    if (!reverse_arc)
                    {
                        printf("Error: 内存分配失败\n");
                        // 释放已分配的正向边
                        free(new_arc);
                        continue;
                    }
                    reverse_arc->number = node_id + 1;
                    reverse_arc->val = s->adjlist[node_id].val;
                    reverse_arc->nextarc = s->adjlist[neighbor_id].firstarc;
                    s->adjlist[neighbor_id].firstarc = reverse_arc;
                }
            }
        }
    }
}
int dijkstra_shortest(struct State *s, int start, int end) {
    int *dist = (int*)malloc(s->n * sizeof(int));
    int *visited = (int*)calloc(s->n, sizeof(int));
    for (int i = 0; i < s->n; i++) {
        dist[i] = INF;
    }
    dist[start] = s->adjlist[start].val;
    for (int count = 0; count < s->n; count++) {
        int u = -1;
        for (int v = 0; v < s->n; v++) {
            if (!visited[v] && (u == -1 || dist[v] < dist[u])) {
                u = v;
            }
        }
        
        if (u == -1 || dist[u] == INF) break;       
        visited[u] = 1;
        if (u == end) break;
        ArcNode *arc = s->adjlist[u].firstarc;
        while (arc != NULL) {
            int v = arc->number;
            if (!visited[v] && dist[u] != INF) {
                int newDist = dist[u] + s->adjlist[v].val;
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                }
            }
            arc = arc->nextarc;
        }
    }
    
    int result = dist[end];
    free(dist);
    free(visited);
    
    return result == INF ? 0 : result;
}
int dijkstra_second_shortest(struct State *s, int start, int end) {
    int *dist1 = (int*)malloc(s->n * sizeof(int));  // ??????
    int *dist2 = (int*)malloc(s->n * sizeof(int));  // ?ζ????
    int *visited1 = (int*)calloc(s->n, sizeof(int));
    int *visited2 = (int*)calloc(s->n, sizeof(int));
    for (int i = 0; i < s->n; i++) {
        dist1[i] = INF;
        dist2[i] = INF;
    }
    dist1[start] = s->adjlist[start].val;
    while (1) {
        // ???δ????????С????
        int u = -1;
        int useSecond = 0;
        
        for (int v = 0; v < s->n; v++) {
            if (!visited1[v] && dist1[v] < INF) {
                if (u == -1 || dist1[v] < (useSecond ? dist2[u] : dist1[u])) {
                    u = v;
                    useSecond = 0;
                }
            }
            if (!visited2[v] && dist2[v] < INF) {
                if (u == -1 || dist2[v] < (useSecond ? dist2[u] : dist1[u])) {
                    u = v;
                    useSecond = 1;
                }
            }
        }
        if (u == -1) break;
        if (useSecond) {
            visited2[u] = 1;
        } else {
            visited1[u] = 1;
        }
        int currentDist = useSecond ? dist2[u] : dist1[u];
        ArcNode *arc = s->adjlist[u].firstarc;
        while (arc != NULL) {
            int v = arc->number;
            int newDist = currentDist + s->adjlist[v].val;
            
            if (newDist < dist1[v]) {
                dist2[v] = dist1[v];
                dist1[v] = newDist;
            } else if (newDist > dist1[v] && newDist < dist2[v]) {
                dist2[v] = newDist;
            }
            arc = arc->nextarc;
        }
    }
    int result = dist2[end];
    free(dist1);
    free(dist2);
    free(visited1);
    free(visited2);
    return result == INF ? 0 : result;
}

int solve1(struct State *s) {
    if (s == NULL || s->n < 2) return 0;
    return dijkstra_shortest(s, 0, s->n - 1);
}

int solve2(struct State *s) {
    if (s == NULL || s->n < 2) return 0;
    return dijkstra_second_shortest(s, 0, s->n - 1);
}