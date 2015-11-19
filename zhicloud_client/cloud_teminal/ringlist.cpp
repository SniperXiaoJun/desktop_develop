#include <stdlib.h>
#include <stdio.h>
#include "ringlist.h"                                                                                                                                                                  
#define OVERFLOW -1
#define ERROR    -2
#define OK       1
#define true     1
#define false    0
 
void list_init(dulinklist *l)
{ /* �����յ�˫��ѭ������l */
  *l=(dulinklist)malloc(sizeof(dulnode));
  if (*l)
  {
	  (*l)->next = (*l)->prior = *l;
	  (*l)->data = NULL;
  }
  else
    exit(OVERFLOW);
}
void list_destory(dulinklist *l)
{
  /* �������������˫��ѭ������l */
  dulinklist q,p=(*l)->next; /* pָ���һ����� */
  while(p!=*l) /* pû����ͷ */
  {
    q=p->next;
    free(p);
    p=q;
  }
  free(*l);
  *l=NULL;
}
void list_clear(dulinklist l) /* ���ı�l */
{ /* ��ʼ������l�Ѵ��ڡ������������l����Ϊ�ձ� */
  dulinklist q,p=l->next; /* pָ���һ����� */
  while(p!=l) /* pû����ͷ */
  {
    q=p->next;
    free(p);
    p=q;
  }
  l->next=l->prior=l; /* ͷ��������ָ�����ָ������ */
}
int list_empty(dulinklist l)
{ /* ��ʼ���������Ա�l�Ѵ��ڡ������������lΪ�ձ��򷵻�true�����򷵻�false */
  if(l->next==l&&l->prior==l)
    return true;
  else
    return false;
}
int list_length(dulinklist l)
{ /* ��ʼ������l�Ѵ��ڡ��������������l������Ԫ�ظ��� */
  int i=0;
  dulinklist p=l->next; /* pָ���һ����� */
  while(p!=l) /* pû����ͷ */
  {
    i++;
    p=p->next;
  }
  return i;
}
int list_getelem(dulinklist l,int i,elemtype *e)
{ /* ����i��Ԫ�ش���ʱ����ֵ����e������OK�����򷵻�ERROR */
  int j=1; /* jΪ������ */
  dulinklist p=l->next; /* pָ���һ����� */
  do 
  {
	 if (j == i)
	 {
		 *e = p->data;
	 }
	 j++;
	 p = p->next;
  } while (p != l);
  return OK;
}
 
int list_locate_elem(dulinklist l,elemtype e,int(*compare)(elemtype,elemtype))
{ /* ��ʼ������l�Ѵ��ڣ�compare()������Ԫ���ж����� */
  /* �������������l�е�1����e�����ϵcompare()������Ԫ�ص�λ�� */
  /*           ������������Ԫ�ز����ڣ��򷵻�ֵΪ0 */
  int i=0;
  dulinklist p=l->next; /* pָ���1��Ԫ�� */
  while(p!=l)
  {
    i++;
    if(compare(p->data,e)) /* �ҵ�����������Ԫ�� */
      return i;
    p=p->next;
  }
  return 0;
}
 
struct dulnode* list_priorelem(dulinklist l, elemtype cur_e, elemtype *pre_e)
{ /* �����������cur_e��l������Ԫ�أ��Ҳ��ǵ�һ��������pre_e��������ǰ���� */
  /*           �������ʧ�ܣ�pre_e�޶��� */
  dulinklist p=l->prior; /* pָ���prior��Ԫ�� */
  do /* pû����ͷ */
  {
    if(p->data==cur_e)
    {
      *pre_e=p->prior->data;
	  if (*pre_e == NULL)//l->data is null
	  {
		  *pre_e = p->prior->prior->data;
		  return p->prior->prior;
	  }
	  return p->prior;
    }
    p=p->prior;
  } while (p != l);
  return false;
}
 
struct dulnode* list_nextelem(dulinklist l, elemtype cur_e, elemtype *next_e)
{ /* �����������cur_e��l������Ԫ�أ��Ҳ������һ��������next_e�������ĺ�̣� */
  /*           �������ʧ�ܣ�next_e�޶��� */
  dulinklist p=l->next; /* pָ���1��Ԫ�� */
  do /* pû����ͷ */                                                                                                                                                          
  {
    if(p->data==cur_e)
    {
      *next_e=p->next->data;
	  if (*next_e == NULL)//l->data is null
	  {
		  *next_e = p->next->next->data;
		  return p->next->next;
	  }
	  return p->next;
	}
    p=p->next;
  } while ((p != l));
  return false;
}
 
dulinklist list_get_elemp(dulinklist l,int i) /* ��� */
{ /* ��˫������l�з��ص�i��Ԫ�صĵ�ַ��iΪ0������ͷ���ĵ�ַ������i��Ԫ�ز����ڣ�*/
  /* ����NULL */
  int j;
  dulinklist p=l; /* pָ��ͷ��� */
  if(i<0||i>list_length(l)) /* iֵ���Ϸ� */
    return NULL;
  for(j=1;j<=i;j++)
    p=p->next;
  return p;
}
int list_insert(dulinklist l,int i,elemtype e)
{ /* �ڴ�ͷ����˫��ѭ�����Ա�l�е�i��λ��֮ǰ����Ԫ��e��i�ĺϷ�ֵΪ1��i�ܱ�+1 */
  /* �Ľ��㷨2.18�������޷��ڵڱ�+1�����֮ǰ����Ԫ�� */
  dulinklist p,s;
  if(i<1||i>list_length(l)+1) /* iֵ���Ϸ� */
    return ERROR;
  p=list_get_elemp(l,i-1); /* ��l��ȷ����i��Ԫ��ǰ����λ��ָ��p */
  if(!p) /* p=NULL,����i��Ԫ�ص�ǰ��������(��ͷ���Ϊ��1��Ԫ�ص�ǰ��) */
    return ERROR;
  s=(dulinklist)malloc(sizeof(dulnode));
  if(!s)
    return OVERFLOW;
  s->data=e;
  s->prior=p; /* �ڵ�i-1��Ԫ��֮����� */
  s->next=p->next;
  p->next->prior=s;
  p->next=s;
  return OK;
}
 
int list_delete(dulinklist l,elemtype e)
{ /* ɾ����ͷ����˫��ѭ�����Ա�l�ĵ�i��Ԫ�أ�i�ĺϷ�ֵΪ1��i�ܱ� */
  dulinklist p = l->next;//1
  do 
  {
	  if (p->data == e)
	  {
		  p->prior->next = p->next;//������û�п�������ͷ������β��
		  p->next->prior = p->prior;
		  free(p);
		  return OK;
	  }
	  p = p->next;
  } while (p != l);
  return OK;                                                                                                                                                                           
}
void list_traverse(dulinklist l,void(*visit)(elemtype))
{ /* ��˫��ѭ�����Ա�l��ͷ�������������ÿ������Ԫ�ص��ú���visit() */
  dulinklist p=l->next; /* pָ��ͷ��� */
  while(p!=l)
  {
    visit(p->data);
    p=p->next;
  }
  printf("\n");
}
 
void list_traverse_back(dulinklist l,void(*visit)(elemtype))
{ /* ��˫��ѭ�����Ա�l��ͷ�������������ÿ������Ԫ�ص��ú���visit()����� */
  dulinklist p=l->prior; /* pָ��β��� */
  while(p!=l)
  {
    visit(p->data);
    p=p->prior;
  }
  printf("\n");
}