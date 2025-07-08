#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "state.h"

#define INF 99999999
int *dist;                   // 距离数组，存储从起点到各点的最短距离
int *parent;                 // 父节点数组，用于重构路径
int *vis;                    // 访问数组，标记是否已访问
int *path;                   // 最短路径数组
int path_length;             // 路径长度
int start_node;              // 起点
int n_nodes;                 // 节点总数
// 找到距离最小且未访问的顶点
int find_min_unvisited() {
    int min_dist = INF;
    int min_vertex = -1;
    
    for (int i = 0; i < n_nodes; i++) {
        if (!vis[i] && dist[i] < min_dist) {
            min_dist = dist[i];
            min_vertex = i;
        }
    }
    return min_vertex;
}
// 重构从起点到指定终点的路径
void build_path(int end) {
    if (dist[end] == INF) {
        path_length = 0;
        return;
    }
    int temp_path[n_nodes];
    int temp_length = 0;
    int current = end;
    
    while (current != -1) {
        temp_path[temp_length++] = current;
        current = parent[current];
    }
    
    path_length = temp_length;
    for (int i = 0; i < temp_length; i++) {
        path[i] = temp_path[temp_length - 1 - i];
    }
}
// 函数1：找最短路径并标记vis数组，维护dist数组
int find_shortest_path(struct State *s, int start, int end) {
    if (!s || !s->adjlist || s->n <= 0) {
        return -1; // 错误：无效输入
    }
    n_nodes = s->n;
    start_node = start;
    // 初始化数组
    for (int i = 0; i < n_nodes; i++) {
        dist[i] = INF;
        parent[i] = -1;
        vis[i] = 0;
    }
    dist[start] = 0;
    // 迪杰斯特拉算法
    for (int count = 0; count < s->n - 1; count++) {
        int u = find_min_unvisited();
        if (u == -1) break; // 没有可达的顶点
        
        vis[u] = 1; // 标记为已访问
        // 更新相邻顶点的距离
        ArcNode *current = s->adjlist[u].firstarc;
        while (current != NULL) {
            int v = current->number;
            int weight = current->val; // 边的权重为产业值
            
            if (!vis[v] && dist[u] != INF && 
                dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
            current = current->nextarc;
        }
    }
    // 标记最短路径上的所有节点为已访问
    build_path(end);
    for (int i = 0; i < path_length; i++) {
        vis[path[i]] = 1;
    }
    
    return 0; // 成功
}
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
    // 初始化所有顶点
    init_State(s);
    // 计算每个顶点的产业值
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            int node_id = y * width + x;
            PXL* px = get_PXL(p, x, y);
            if (!px)
             continue;
            // 计算产业值：255*255*3 - R? - G? - B?
            int R = px->red;
            int G = px->green;
            int B = px->blue;
            s->adjlist[node_id].val = 255*255*3 - R*R - G*G - B*B;
        }
    }
    // 构建邻接表（添加边）
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++)
         {
            int node_id = y * width + x;
            // 方向数组：上、下、左、右
            int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            // 检查四个方向的邻居
            for (int d = 0; d < 4; d++) 
            {
                int ny = y + dirs[d][0];
                int nx = x + dirs[d][1];
                // 检查邻居是否在地图范围内
                if (ny >= 0 && ny < height && nx >= 0 && nx < width) 
                {
                    int neighbor_id = ny * width + nx;
                    // 创建新的邻接表节点
                    ArcNode* new_arc = (ArcNode*)malloc(sizeof(ArcNode));
                    if (!new_arc) {
                        printf("Error: 内存分配失败\n");
                        continue;
                    }
                    // 设置边的信息
                    new_arc->number = neighbor_id + 1;  // 顶点编号从1开始
                    new_arc->val = s->adjlist[neighbor_id].val;  // 邻居的产业值
                    // 将新节点插入邻接表头部
                    new_arc->nextarc = s->adjlist[node_id].firstarc;
                    s->adjlist[node_id].firstarc = new_arc;
                }
            }
        }
    }
}
int solve1(struct State *s) {
    // 假设要找从节点0到节点n-1的最短路径
    int start = 0;
    int end = s->n - 1;
    
    if (find_shortest_path(s, start, end) != 0) {
        return -1;
    }
    
    if (path_length == 0) {
        return -1; // 无路径
    }
    
    return dist[end]; // 返回最短距离
}
int solve2(struct State *s) {
    int start = 0;
    int end = s->n - 1;
    // 先找到最短路径
    if (find_shortest_path(s, start, end) != 0) {
        return -1;
    }
    int shortest_dist = dist[end];
    if (shortest_dist == INF) {
        return -1; // 无路径存在
    }
    // 保存最短路径
    int shortest_path[n_nodes];
    int shortest_path_len = path_length;
    for (int i = 0; i < path_length; i++) {
        shortest_path[i] = path[i];
    }
    int second_shortest_dist = INF;
    int second_shortest_path[n_nodes];
    int second_shortest_path_len = 0;
    
    // 尝试删除最短路径上的每一条边
    for (int i = 0; i < shortest_path_len - 1; i++) {
        int u = shortest_path[i];
        int v = shortest_path[i + 1];
        
        // 临时删除边u->v，找到替代路径
        // 重新初始化
        for (int j = 0; j < n_nodes; j++) {
            dist[j] = INF;
            parent[j] = -1;
            vis[j] = 0;
        }
        dist[start] = 0;
        for (int count = 0; count < s->n - 1; count++) {
            int curr = find_min_unvisited();
            if (curr == -1) break;
            vis[curr] = 1;
            ArcNode *current = s->adjlist[curr].firstarc;
            while (current != NULL) {
                int next = current->number;
                int weight = current->val;
                // 跳过被删除的边
                if (curr == u && next == v) {
                    current = current->nextarc;
                    continue;
                }
                
                if (!vis[next] && dist[curr] != INF && 
                    dist[curr] + weight < dist[next]) {
                    dist[next] = dist[curr] + weight;
                    parent[next] = curr;
                }
                current = current->nextarc;
            }
        }
        
        // 检查是否找到了更好的次短路径
        if (dist[end] != INF && dist[end] > shortest_dist && 
            dist[end] < second_shortest_dist) {
            second_shortest_dist = dist[end];
            build_path(end);
            second_shortest_path_len = path_length;
            for (int j = 0; j < path_length; j++) {
                second_shortest_path[j] = path[j];
            }
        }
    }
    
    if (second_shortest_dist == INF) {
        return -1; // 次短路径不存在
    }
    
    return second_shortest_dist; // 返回次短距离
}