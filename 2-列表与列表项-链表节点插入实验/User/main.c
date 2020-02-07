
#include "list.h"

/* 定义根节点 */
struct xLIST List_Text;

/*定义2个节点*/
struct xLIST_ITEM List_Item1;
struct xLIST_ITEM List_Item2;
struct xLIST_ITEM List_Item3;

int main (void)
{
	/* 根节点初始化 */
	vListInitialise(&List_Text);
	
	/* 节点初始化 */
	vListInitialiseItem(&List_Item1);
	vListInitialiseItem(&List_Item2);
	vListInitialiseItem(&List_Item3);
	
	vListInsert(&List_Text,&List_Item1);
	vListInsert(&List_Text,&List_Item2);
	vListInsert(&List_Text,&List_Item3);
	
	
	for(;;)
	{
		/*什么都不做*/
	}
}



