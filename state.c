#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "state.h"

#define INF 99999999
int *dist;                   // �������飬�洢����㵽�������̾���
int *parent;                 // ���ڵ����飬�����ع�·��
int *vis;                    // �������飬����Ƿ��ѷ���
int *path;                   // ���·������
int path_length;             // ·������
int start_node;              // ���
int n_nodes;                 // �ڵ�����
// �ҵ�������С��δ���ʵĶ���
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
// �ع�����㵽ָ���յ��·��
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
// ����1�������·�������vis���飬ά��dist����
int find_shortest_path(struct State *s, int start, int end) {
    if (!s || !s->adjlist || s->n <= 0) {
        return -1; // ������Ч����
    }
    n_nodes = s->n;
    start_node = start;
    // ��ʼ������
    for (int i = 0; i < n_nodes; i++) {
        dist[i] = INF;
        parent[i] = -1;
        vis[i] = 0;
    }
    dist[start] = 0;
    // �Ͻ�˹�����㷨
    for (int count = 0; count < s->n - 1; count++) {
        int u = find_min_unvisited();
        if (u == -1) break; // û�пɴ�Ķ���
        
        vis[u] = 1; // ���Ϊ�ѷ���
        // �������ڶ���ľ���
        ArcNode *current = s->adjlist[u].firstarc;
        while (current != NULL) {
            int v = current->number;
            int weight = current->val; // �ߵ�Ȩ��Ϊ��ҵֵ
            
            if (!vis[v] && dist[u] != INF && 
                dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
            current = current->nextarc;
        }
    }
    // ������·���ϵ����нڵ�Ϊ�ѷ���
    build_path(end);
    for (int i = 0; i < path_length; i++) {
        vis[path[i]] = 1;
    }
    
    return 0; // �ɹ�
}
void init_State(struct State *s)
{
    for (int i = 0; i < s->n; i++) // s->n������������
    {
        s->adjlist[i].number = i + 1;  // ���±�ת��Ϊ��1��ʼ����
        s->adjlist[i].firstarc = NULL; // ����ʼ��ָ����Ϊ��
        s->adjlist[i].val = 0;         // ��ʼ�Ĳ�ҵ��ֵ��Ϊ0
    }
}
void delete_State(struct State *s)
{
    // �������е����򣨶��㣩
    for (int i = 0; i < s->n; i++)
    {
        ArcNode *p = s->adjlist[i].firstarc; // ��ȡ��ǰ����ı�����ͷ
        while (p != NULL)
        {
            ArcNode *temp = p; // �洢��ǰ�߽ڵ�
            p = p->nextarc;      // �ƶ�����һ���߽ڵ�
            free(temp);          // �ͷŵ�ǰ�߽ڵ���ڴ�
        }
    }
    // �ͷ��ڽӱ������ڴ�
    free(s->adjlist);
}
void parse(struct State *s, struct PNG *p)
{
    // ����������
    if (!s || !p || !p->image || p->width <= 0 || p->height <= 0) 
    {
        printf("Error: ��Ч���������\n");
        return;
    }
    int width = p->width;
    int height = p->height;
    int total_nodes = width * height;
    // ������������
    s->n = total_nodes;
    // ��ʼ�����ж���
    init_State(s);
    // ����ÿ������Ĳ�ҵֵ
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            int node_id = y * width + x;
            PXL* px = get_PXL(p, x, y);
            if (!px)
             continue;
            // �����ҵֵ��255*255*3 - R? - G? - B?
            int R = px->red;
            int G = px->green;
            int B = px->blue;
            s->adjlist[node_id].val = 255*255*3 - R*R - G*G - B*B;
        }
    }
    // �����ڽӱ���ӱߣ�
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++)
         {
            int node_id = y * width + x;
            // �������飺�ϡ��¡�����
            int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            // ����ĸ�������ھ�
            for (int d = 0; d < 4; d++) 
            {
                int ny = y + dirs[d][0];
                int nx = x + dirs[d][1];
                // ����ھ��Ƿ��ڵ�ͼ��Χ��
                if (ny >= 0 && ny < height && nx >= 0 && nx < width) 
                {
                    int neighbor_id = ny * width + nx;
                    // �����µ��ڽӱ�ڵ�
                    ArcNode* new_arc = (ArcNode*)malloc(sizeof(ArcNode));
                    if (!new_arc) {
                        printf("Error: �ڴ����ʧ��\n");
                        continue;
                    }
                    // ���ñߵ���Ϣ
                    new_arc->number = neighbor_id + 1;  // �����Ŵ�1��ʼ
                    new_arc->val = s->adjlist[neighbor_id].val;  // �ھӵĲ�ҵֵ
                    // ���½ڵ�����ڽӱ�ͷ��
                    new_arc->nextarc = s->adjlist[node_id].firstarc;
                    s->adjlist[node_id].firstarc = new_arc;
                }
            }
        }
    }
}
int solve1(struct State *s) {
    // ����Ҫ�Ҵӽڵ�0���ڵ�n-1�����·��
    int start = 0;
    int end = s->n - 1;
    
    if (find_shortest_path(s, start, end) != 0) {
        return -1;
    }
    
    if (path_length == 0) {
        return -1; // ��·��
    }
    
    return dist[end]; // ������̾���
}
int solve2(struct State *s) {
    int start = 0;
    int end = s->n - 1;
    // ���ҵ����·��
    if (find_shortest_path(s, start, end) != 0) {
        return -1;
    }
    int shortest_dist = dist[end];
    if (shortest_dist == INF) {
        return -1; // ��·������
    }
    // �������·��
    int shortest_path[n_nodes];
    int shortest_path_len = path_length;
    for (int i = 0; i < path_length; i++) {
        shortest_path[i] = path[i];
    }
    int second_shortest_dist = INF;
    int second_shortest_path[n_nodes];
    int second_shortest_path_len = 0;
    
    // ����ɾ�����·���ϵ�ÿһ����
    for (int i = 0; i < shortest_path_len - 1; i++) {
        int u = shortest_path[i];
        int v = shortest_path[i + 1];
        
        // ��ʱɾ����u->v���ҵ����·��
        // ���³�ʼ��
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
                // ������ɾ���ı�
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
        
        // ����Ƿ��ҵ��˸��õĴζ�·��
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
        return -1; // �ζ�·��������
    }
    
    return second_shortest_dist; // ���شζ̾���
}