#include "state.h"
#include <math.h>
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
int solve1(struct State *s)
{

    return 0;
}
int solve2(struct State *s)
{
    // TODO
    return 0;
}
