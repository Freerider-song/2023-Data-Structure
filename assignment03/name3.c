#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 이름 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*link; // 이름순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 이름순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;


static int compare_by_name(const tName *pName1, const tName *pName2);
static int compare_by_freq(const tName *pName1, const tName *pName2);

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void){
    LIST *list = (LIST *)malloc(sizeof(LIST)); // 메모리 할당
    if (list!=NULL) {
        list->count = 0;
        list->head = NULL;
        list->head2 = NULL;
    }
    return list; //list는 현재 포인터이므로 헤드 노드 포인터가 됨
};

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList){
	if(pList!=NULL){
		NODE *temp = malloc(sizeof(NODE));
		while(pList->head!=NULL){
			temp = pList->head;
			pList->head = pList->head->link; //head의 link를 list 헤드로 설정
			free(temp->dataPtr); //먼저 tName 데이터에 대한 메모리를 해제.
			//free(temp); //그다음으로 노드자체에 대해서 메모리를 해제해야만 하한다.
			
		}
		while(pList->head2!=NULL){
			temp = pList->head2;
            pList->head2 = pList->head2->link2;
			//free(temp->dataPtr); 이름순으로 할때 이미 해제했으므로 또하면 안됨!
            free(temp);
		}
	}
	free(pList);
};

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
// static이 붙은 함수는 내부에서 사용하는 함수
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr){
	NODE *pNew= malloc(sizeof(NODE));
	if ((pNew)==NULL) //메모리가 overflow하는 경우 alloc 안되면 null을 뱉음
        return 0;
	if(pPre==NULL){ // insert at beginning
		pNew->link = pList->head;
		pList->head = pNew;
		
		//pNew->link2 = pList->head2; // insert는 이름만 잇는건가 아 헷갈려
		//pList->head2 = pNew;
		
		
	}
	else{ // 처음이 아닌경우
		pNew->link = pPre->link;
		pPre->link = pNew;
		
	}
	pNew->dataPtr = dataInPtr;
	(pList->count)++;
	
	return 1;
};

// internal function
// connects node into a frequency list
// pLoc = 현재 위치
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc){ //insert와 똑같, 대신 빈도로
	if(pPre==NULL){
		pLoc->link2 = pList->head2;
		pList->head2 = pLoc;
	}
	else{
		pLoc->link2 = pPre->link2;
		pPre->link2 = pLoc;
	}
	
	
};

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
// 이름순으로 정렬되어있는 리스트에서 search, pArgu를 받아서 search, link 따라감
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){ //아 그니까 이제 insert하기 전에 pPre알기 위해서 이거하겠다
//return은 1,0이므로, pPre를 갱신하는 방식으로 해야겠네
	*pPre = NULL;
	*pLoc = pList->head;
	
	while((*pLoc)!=NULL && compare_by_name((*pLoc)->dataPtr, pArgu)<0){ // target > pLoc key인 경우
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link;
	}
	if((*pLoc)==NULL) return 0;
	else {
		if(compare_by_name((*pLoc)->dataPtr, pArgu)==0) return 1;
		else if(compare_by_name((*pLoc)->dataPtr, pArgu)>0) return 0;// 일치하는것이 없는 경우
	}
	
};
// 빈도순으로 정렬되어있는 리스트에서 search, link2 따라감
static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu){
	*pPre = NULL;
	*pLoc = pList->head2;
	
	while(*pLoc!=NULL && compare_by_freq((*pLoc)->dataPtr, pArgu)<0){
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link2;
	}
	if(*pLoc==NULL) return 0;
	else {
		if(compare_by_freq((*pLoc)->dataPtr, pArgu)==0) return 1;
		else if(compare_by_freq((*pLoc)->dataPtr, pArgu)>0) return 0;
	}

};

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex){ //search 함수에서 사용
	tName *nNew = (tName *)malloc(sizeof(tName)); //assignment2때의 실수 방지하자
	strcpy(nNew->name, name);
	nNew->sex = sex;
	nNew->total_freq = 0;
	
	return nNew;

}; 

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode){
	if(pNode!=NULL) free(pNode);
};

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
//입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축)
void load_names( FILE *fp, int start_year, LIST *list){
	char name[20];
	char sex;
	int freq;
	int year;

	while(fscanf(fp, "%d %s %c %d", &year, name, &sex, &freq) != EOF){
		tName *key = malloc(sizeof(tName));
		//tName key; <- 요것때문에 계속 오류가 발생했음 왜? 
		//tName key;를 선언하고 이를 binary_search에 전달합니다. 이는 스택에 할당되며, 이 스택 공간은 load_names 함수가 반환된 후에 재사용될 수 있습니다. 따라서 binary_search에서 반환된 key 포인터가 가리키는 메모리는 더 이상 유효하지 않을 수 있습니다.
		strcpy(key->name, name);
		key->sex = sex;
		//NODE *pPre = malloc(sizeof(NODE)); //사실 이것들은 리스트 노드를 가리키는 용도로 사용되므로 별도의 메모리를 할당받을 필요가 없다는 것이 중론
		//NODE *pLoc = malloc(sizeof(NODE));
		NODE *pPre, *pLoc;
		int found;
		found = _search(list, &pPre, &pLoc, key); //LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu)
		if(found){ //일치하는 것이 있다면 밑에는 과제 1,2와 같음
			pLoc->dataPtr->total_freq +=freq;
			pLoc->dataPtr->freq[year-start_year] += freq;
			//free(key);
		}
		else{
			//insert ->LIST *pList, NODE *pPre, tName *dataInPtr
			tName *nNew = createName(name,sex); //name, sex만 입력되어있음
			nNew->total_freq = freq;
			nNew->freq[year-start_year] = freq;
			_insert(list, pPre, nNew);
			
		}
		free(key);
		//free(pPre);
		//free(pLoc);
	}
	return;
};

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결, insert하지 않고 포인터 연산만 함
void connect_by_frequency(LIST *list){
	//if(list==NULL)
	//NODE *pTemp = list->head2; 

	NODE *pTemp;        // = malloc(sizeof(NODE));
	pTemp = list->head; //이렇게 해야지 됨 왜냐면 원래 이름순 리스트를 순회하여야하기 떄문이고 이에 따라 pTemp->dataPtr도 원래 head로 설정해야하기때문임
	while(pTemp!=NULL){
		NODE *pPre =NULL, *pLoc=NULL;
		_search_by_freq(list, &pPre, &pLoc, pTemp->dataPtr); //pPre와 pLoc가 설정됨!
		_link_by_freq(list, pPre, pTemp); //LIST *pList, NODE *pPre, NODE *pLoc
		pTemp=pTemp->link; //다음 링크로 옮겨가며 순회, 마지막에 다다르면 pTemp가 null
		
	}
	
};

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year){
	NODE *pTemp;
	pTemp = pList->head;
	while(pTemp!=NULL){
		printf("%s\t%c\t%d", pTemp->dataPtr->name, pTemp->dataPtr->sex, pTemp->dataPtr->total_freq);
		for(int j =0; j < num_year; j++){
			printf("\t%d", pTemp->dataPtr->freq[j]);
		}
		printf("\n");
		
		pTemp = pTemp ->link;
	}
	
}; // 이름순
void print_names_by_freq( LIST *pList, int num_year){ //num_year = 10
	NODE *pTemp;
	// = malloc(sizeof(NODE));
	pTemp = pList->head2;
	while(pTemp!=NULL){
		printf("%s\t%c\t%d", pTemp->dataPtr->name, pTemp->dataPtr->sex, pTemp->dataPtr->total_freq);
		for(int j =0; j < num_year; j++){
			printf("\t%d", pTemp->dataPtr->freq[j]);
		}
		printf("\n");
		
		pTemp = pTemp ->link2;
	}
	
}; // 빈도순

////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
// 정렬 기준 : 이름(1순위), 성별(2순위)
static int compare_by_name(const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
static int compare_by_freq(const tName *pName1, const tName *pName2)
{
	int ret = pName2->total_freq - pName1->total_freq;
	if (ret != 0) return ret;
	
	return compare_by_name(pName1, pName2);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by name\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_NAME;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축) -> 
	load_names( fp, 2009, list);
	
	fclose( fp);

	if (option == SORT_BY_NAME) {
		
		// 이름순 리스트를 화면에 출력
		print_names( list, MAX_YEAR_DURATION);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_names_by_freq( list, MAX_YEAR_DURATION);
	}
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}
