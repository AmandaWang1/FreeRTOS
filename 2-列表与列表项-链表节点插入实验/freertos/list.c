#include "FreeRTOS.h"
#include <stdlib.h>
#include "list.h"



//传入的参数是一个节点
void vListInitialiseItem(ListItem_t * const pxItem)
{
	pxItem->pvContainer = NULL;//这个节点不指向任何一个链表
}

//链表根节点初始化
void vListInitialise (List_t * const pxList)
{
	/* 将链表索引指针指向最后一个节点 */  //指向结束的位置,强制转换为节点类型的数据结构
	pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd );
	
	/* 将链表最后一个节点的辅助排序的值设置为最大，确保该节点就是链表的最后节点 */
	pxList->xListEnd.xItemValue = portMAX_DELAY;
	
	/* 将最后一个节点的pxNext和pxPrevious指针指向节点自身，表示链表为空 */
	pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );
	pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd );
		
	/* 初始化链表节点计数器的值为0，表示链表为空 */
	pxList->uxNumberOfItems = ( UBaseType_t ) 0U;
	
}

/* 将节点插入到链表尾部 */
/*参数：pxList：链表，表示你要插入的为哪个链表
				pxNewListItem：新的节点       */
void vListInsertEnd(List_t *  const pxList,ListItem_t * const pxNewListItem )
{
	ListItem_t * const pxIndex = pxList->pxIndex;  //end
	
	/* 步骤：
		 1.将一个新节点的下一个节点指向end 
		 2.将一个新节点的上一个指向原来end的上一个节点
   	 3.end上一个的下一个指向一个新的节点
		 4.end的上一个节点指向一个新插入的节点 */
	
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;
	
	/* 让该节点记住该节点所在的链表 */
	pxNewListItem->pvContainer = ( void * ) pxList;
	
	/* 链表节点计数器 */
	( pxList->uxNumberOfItems )++;
}



/* 将节点按照升序排列插入到链表 */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	ListItem_t *pxIterator;
	
	/* 获取节点的排序辅助值 */
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

	/* 寻找节点要插入的位置 */
	//如果值等于最大的，直接插入到end 
	if( xValueOfInsertion == portMAX_DELAY )
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	//否则通过for循环，将值找出，并且记录属于哪一个链表
	else
	{
		for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd );
		     pxIterator->pxNext->xItemValue <= xValueOfInsertion; 
			 pxIterator = pxIterator->pxNext )
		{
			/* 没有事情可做，不断迭代只为了找到节点要插入的位置 */			
		}
	}

	//以1，2，3为例，2是新节点
	//pxIterator：原先节点
	//pxPrevious：上一个
	pxNewListItem->pxNext = pxIterator->pxNext;//新节点的下一个，指向了原先节点的下一个
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;//新节点的下一个的上一个指向新节点
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;

	/* 记住该节点所在的链表 */
	pxNewListItem->pvContainer = ( void * ) pxList;

	/* 链表节点计数器++ */
	( pxList->uxNumberOfItems )++;
}


/* 将节点从链表中删除 */
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* 获取节点所在的链表 pvContainer所指向的地方*/
	List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
 //
	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;//删除了2 3之间的
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;//删除了1 3 之间的

	/* 调整链表的节点索引指针 */
	if( pxList->pxIndex == pxItemToRemove )
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}

	/* 初始化该节点所在的链表为空，表示节点还没有插入任何链表 */
	//这个链表已经被删除
	pxItemToRemove->pvContainer = NULL;
	
	/* 链表节点计数器-- */
	( pxList->uxNumberOfItems )--;

	/* 返回链表中剩余节点的个数 */
	return pxList->uxNumberOfItems;
}















