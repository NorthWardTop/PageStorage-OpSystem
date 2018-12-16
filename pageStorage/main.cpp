
/*
数据结构
	页表结构	struct Page
	指令结构
数据
	页大小	PAGESIZE
	允许最大页数		MAX_PAGE
	主存最大页数		MAX_MEMPAGE
	相对最大值	MAX

	主存页表		Page memPageTab[MAX_Page]
	主存存在页号循环队列	size_t memPageNum[MAX_PAGE]
	缺页下标		size_t k
	指令数组	
操作
	初始化所有内存页
	根据指令下标取指令返回指令


*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PAGESIZE	4096	//页面大小
#define MAX_PAGE	7		//最大页数
#define MAX_MEMPAGE	4		//主存最大页数
#define INF			99		//无穷大,不在主存的页架号
#define MAX_INSTCT	12		//最大指令数

struct Page
{
	int		pageId;			//页号
	bool	estFlg;			//是否在主存
	int		mainId;			//页架号：设定页表在主存的位置
	bool	chFlg;			//修改标志：设定页面在主存中是否修改过
	int		dskIndex;		//磁盘上位置：设定页面在辅存中的位置
};

struct  Instct
{
	char	op;			//操作
	int		pageId;		//页号
	int		pageAddr;	//页内地址
};

void init(Page* memPageTab, int* memPageId, Instct* instct);
void inOutMen(int k, Instct* tmpInstct, Page* pageTab, int* memPageId);

int main()
{
	Page	pageTab[MAX_PAGE];		//所有页表页表
	Instct	instct[MAX_INSTCT];		//所有指令

	int		k = 0;					//循环队列尾
	int		memPageId[MAX_MEMPAGE];	//在主存页号,循环队列
	Instct*	tmpInstct=NULL;
	int		absAddr = 0;
	//初始化
	init(pageTab, memPageId, instct);
	//本循环重复取指令, i为指令下标
	for (int i = 0; i < MAX_INSTCT; ++i)
	{
		//取指令保存 
		tmpInstct = &instct[i];
		assert(tmpInstct->op);	//指令操作不为空继续
		printf("%d.执行[%c]指令\t", i,tmpInstct->op);
		//按照是否存在主存分两种情况
		if (pageTab[tmpInstct->pageId].estFlg)
			absAddr = pageTab[tmpInstct->pageId].mainId*PAGESIZE + tmpInstct->pageAddr;
		else //不在内存中
		{
			printf("[*] 页号为 %d 不在内存\n", tmpInstct->pageId);
			printf("\t页号为 %d 出内存\t", memPageId[k]);
			//队尾下标k,指令tmpInstct,页表pageTab,存在的页ID memPageId
			inOutMen(k, tmpInstct, pageTab, memPageId);
			absAddr = pageTab[tmpInstct->pageId].mainId*PAGESIZE + tmpInstct->pageAddr;
		}

		if (tmpInstct->op == 'i')//指令为存时则修改chFlg为1
			pageTab[tmpInstct->pageId].chFlg = 1;
		printf("绝对地址为:%d\n", absAddr);
		if (i == MAX_INSTCT - 1)
			printf("所有指令执行完毕!\n");
	}
	system("pause");
	return 0;
}

//初始化所有页表和指令
void init(Page* memPageTab, int* memPageId, Instct* instct)
{
	//初始化所有页表
	int		pageId_t[MAX_PAGE] = { 0, 1, 2, 3, 4, 5, 6 };
	bool	estFlg[MAX_PAGE] = { 1, 1, 1, 1, 0, 0, 0 };
	int		mainId[MAX_PAGE] = { 5, 8, 9, 1, INF, INF, INF };//MAX表示主存中不存在
	bool	chFlg[MAX_PAGE] = { 0 };
	int		dskIndex[MAX_PAGE] = { 11, 12, 13, 21, 22, 23, 121 };

	for (int i = 0; i < MAX_PAGE; ++i)
	{
		memPageTab[i].chFlg = chFlg[i];
		memPageTab[i].dskIndex = dskIndex[i];
		memPageTab[i].estFlg = estFlg[i];
		memPageTab[i].mainId = mainId[i];
		memPageTab[i].pageId = pageId_t[i];
		if (i < 4)	//页架号就是主存中存在的页ID
			memPageId[i] = memPageTab[i].pageId;	//初始化在内存的页ID
	}

	//初始化所有指令i为存入, o为读出
	char op[MAX_INSTCT] = { '+', '-', '*', 'i', 'o', '-', 'm', '+', 'i', 'o', '+', 'i' };
	int pageId_i[MAX_INSTCT] = { 0, 1, 2, 3, 0, 6, 4, 5, 1, 2, 4, 6 };
	int	pageAddr[MAX_INSTCT] = { 70, 50, 15, 21, 56, 40, 53, 23, 37, 78, 1, 84 };

	for (int i = 0; i < MAX_INSTCT; ++i)
	{
		instct[i].op = op[i];
		instct[i].pageAddr = pageAddr[i];
		instct[i].pageId = pageId_i[i];
	}
	printf("初始化完成!\n");
}

//队尾下标k,当前指令,页表pageTab,存在的页ID memPageId
void inOutMen(int k, Instct* tmpInstct, Page* pageTab, int* memPageId)
{
	pageTab[memPageId[k]].estFlg = 0;//将最老的存在标置0
	memPageId[k] = tmpInstct->pageId;//放入新页ID
	k = (k + 1) % MAX_MEMPAGE;//移动循环队列指针
	pageTab[tmpInstct->pageId].estFlg = 1;//将放入的存在标置1
	//方便起见,存入的页架号为磁盘位置
	pageTab[tmpInstct->pageId].mainId = pageTab[tmpInstct->pageId].dskIndex;
	printf("\t页号为 %d 进内存\t", tmpInstct->pageId);
}


//根据页号在页表查找是否存在内存(所查页号,内存页id表)
bool isEstMem(int pageId, int* memId)
{
	bool flg = 0;
	for (int i = 0; i < MAX_MEMPAGE; ++i)
	{
		if (memId[i] == pageId)
		{
			flg = 1;
			break;
		}
	}
	return flg;
}

//查找主存ID页号,所有页表, 查找页号对应页架
int findMainId(int pageId, Page* memPageTab)
{
	int mainId = 0;
	int i = 0;
	for (i = 0; i < MAX_PAGE; ++i)
	{	//遍历页表,当页表项对应的pageId时,赋值
		if (memPageTab[i].estFlg && memPageTab[i].pageId == pageId)
		{
			mainId = memPageTab[i].mainId;
			break;
		}
	}
	if (i == MAX_PAGE)	//找到最后也没有找到,发生错误
		printf("findMainId error!\n");
	return mainId;
}

//移入移出主存(内存中最老的页号,缺页的页号,主存ID队列)
//insertMainMem(oldPageId, tmpInstct->pageId, memPageId);
