#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "state.h"
#include <limits.h>

#define INF INT_MAX

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
    int *dist1 = (int*)malloc(s->n * sizeof(int));  // ��̾���
    int *dist2 = (int*)malloc(s->n * sizeof(int));  // �ζ̾���
    int *visited1 = (int*)calloc(s->n, sizeof(int));
    int *visited2 = (int*)calloc(s->n, sizeof(int));
    for (int i = 0; i < s->n; i++) {
        dist1[i] = INF;
        dist2[i] = INF;
    }
    dist1[start] = s->adjlist[start].val;
    while (1) {
        // �ҵ�δ�������С����
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