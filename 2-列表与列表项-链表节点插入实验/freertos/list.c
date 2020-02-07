#include "FreeRTOS.h"
#include <stdlib.h>
#include "list.h"



//����Ĳ�����һ���ڵ�
void vListInitialiseItem(ListItem_t * const pxItem)
{
	pxItem->pvContainer = NULL;//����ڵ㲻ָ���κ�һ������
}

//������ڵ��ʼ��
void vListInitialise (List_t * const pxList)
{
	/* ����������ָ��ָ�����һ���ڵ� */  //ָ�������λ��,ǿ��ת��Ϊ�ڵ����͵����ݽṹ
	pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd );
	
	/* ���������һ���ڵ�ĸ��������ֵ����Ϊ���ȷ���ýڵ������������ڵ� */
	pxList->xListEnd.xItemValue = portMAX_DELAY;
	
	/* �����һ���ڵ��pxNext��pxPreviousָ��ָ��ڵ�������ʾ����Ϊ�� */
	pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );
	pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd );
		
	/* ��ʼ������ڵ��������ֵΪ0����ʾ����Ϊ�� */
	pxList->uxNumberOfItems = ( UBaseType_t ) 0U;
	
}

/* ���ڵ���뵽����β�� */
/*������pxList��������ʾ��Ҫ�����Ϊ�ĸ�����
				pxNewListItem���µĽڵ�       */
void vListInsertEnd(List_t *  const pxList,ListItem_t * const pxNewListItem )
{
	ListItem_t * const pxIndex = pxList->pxIndex;  //end
	
	/* ���裺
		 1.��һ���½ڵ����һ���ڵ�ָ��end 
		 2.��һ���½ڵ����һ��ָ��ԭ��end����һ���ڵ�
   	 3.end��һ������һ��ָ��һ���µĽڵ�
		 4.end����һ���ڵ�ָ��һ���²���Ľڵ� */
	
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;
	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;
	
	/* �øýڵ��ס�ýڵ����ڵ����� */
	pxNewListItem->pvContainer = ( void * ) pxList;
	
	/* ����ڵ������ */
	( pxList->uxNumberOfItems )++;
}



/* ���ڵ㰴���������в��뵽���� */
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
	ListItem_t *pxIterator;
	
	/* ��ȡ�ڵ��������ֵ */
	const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

	/* Ѱ�ҽڵ�Ҫ�����λ�� */
	//���ֵ�������ģ�ֱ�Ӳ��뵽end 
	if( xValueOfInsertion == portMAX_DELAY )
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	//����ͨ��forѭ������ֵ�ҳ������Ҽ�¼������һ������
	else
	{
		for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd );
		     pxIterator->pxNext->xItemValue <= xValueOfInsertion; 
			 pxIterator = pxIterator->pxNext )
		{
			/* û��������������ϵ���ֻΪ���ҵ��ڵ�Ҫ�����λ�� */			
		}
	}

	//��1��2��3Ϊ����2���½ڵ�
	//pxIterator��ԭ�Ƚڵ�
	//pxPrevious����һ��
	pxNewListItem->pxNext = pxIterator->pxNext;//�½ڵ����һ����ָ����ԭ�Ƚڵ����һ��
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;//�½ڵ����һ������һ��ָ���½ڵ�
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;

	/* ��ס�ýڵ����ڵ����� */
	pxNewListItem->pvContainer = ( void * ) pxList;

	/* ����ڵ������++ */
	( pxList->uxNumberOfItems )++;
}


/* ���ڵ��������ɾ�� */
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
	/* ��ȡ�ڵ����ڵ����� pvContainer��ָ��ĵط�*/
	List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
 //
	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;//ɾ����2 3֮���
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;//ɾ����1 3 ֮���

	/* ��������Ľڵ�����ָ�� */
	if( pxList->pxIndex == pxItemToRemove )
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}

	/* ��ʼ���ýڵ����ڵ�����Ϊ�գ���ʾ�ڵ㻹û�в����κ����� */
	//��������Ѿ���ɾ��
	pxItemToRemove->pvContainer = NULL;
	
	/* ����ڵ������-- */
	( pxList->uxNumberOfItems )--;

	/* ����������ʣ��ڵ�ĸ��� */
	return pxList->uxNumberOfItems;
}















