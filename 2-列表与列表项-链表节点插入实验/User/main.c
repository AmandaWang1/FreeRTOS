
#include "list.h"

/* ������ڵ� */
struct xLIST List_Text;

/*����2���ڵ�*/
struct xLIST_ITEM List_Item1;
struct xLIST_ITEM List_Item2;
struct xLIST_ITEM List_Item3;

int main (void)
{
	/* ���ڵ��ʼ�� */
	vListInitialise(&List_Text);
	
	/* �ڵ��ʼ�� */
	vListInitialiseItem(&List_Item1);
	vListInitialiseItem(&List_Item2);
	vListInitialiseItem(&List_Item3);
	
	vListInsert(&List_Text,&List_Item1);
	vListInsert(&List_Text,&List_Item2);
	vListInsert(&List_Text,&List_Item3);
	
	
	for(;;)
	{
		/*ʲô������*/
	}
}



