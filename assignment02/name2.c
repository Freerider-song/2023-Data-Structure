#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬


int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found);
				
int compare_by_name( const void *n1, const void *n2);int compare_by_name( const void *n1, const void *n2);
// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언

// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 이름과 성별에 의해 정렬 리스트(ordered list)를 유지해야 함 (qsort 함수 사용하지 않음)
// 1. 이미 등장한 이름인지 검사하기 위해
// 2. 새로운 이름을 삽입할 위치를 찾기 위해 binary_search 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, tNames *names){
	char name[20];
	char sex;
	int freq;
	int year;
	
	int flag;
	
	while(fscanf(fp, "%d %s %c %d", &year, name, &sex, &freq) != EOF) { //fscanf함수는 파일에 끝에 가면 eof 반환
		flag=0;
		tName *key = malloc(sizeof(tName));
		//tName key; <- 요것때문에 계속 오류가 발생했음 왜? 
		//tName key;를 선언하고 이를 binary_search에 전달합니다. 이는 스택에 할당되며, 이 스택 공간은 load_names 함수가 반환된 후에 재사용될 수 있습니다. 따라서 binary_search에서 반환된 key 포인터가 가리키는 메모리는 더 이상 유효하지 않을 수 있습니다.
		strcpy(key->name, name);
		key->sex = sex;
		int len = names-> len;
		if (len == names->capacity){ //꽉차면
			names->capacity = names->capacity + 1000;
			//names->data가 가리키는 메모리의 크기를 capacity만큼 용량을 늘려야됨
			names->data = realloc(names->data, names->capacity * sizeof(tName)); //realloc을 이용하여 
		}
		int found =0;
		//int *found = malloc(sizeof(int));
		int num = binary_search(key, names->data, len, sizeof(tName), 
		compare_by_name, &found);
			//const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *), int *found
		/*key: 검색할 객체에 대한 포인터*/
		if(found){ //중복되는경우
			names->data[num].total_freq+=freq;
			names->data[num].freq[year-start_year] += freq;
	
		}
		else{ //중복안되는경우
			memmove(names->data + num+1, names->data + num, (names->len-num)*sizeof(tName));
			strcpy(names->data[num].name, name);
            names->data[num].sex = sex;
            names->data[num].total_freq = freq;
            memset(names->data[num].freq, 0, sizeof(int) * MAX_YEAR_DURATION);
            names->data[num].freq[year - start_year] = freq;
            names->len++;
		}
		free(key);
		//free(found);
			
	
	}
	return;
};

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year){
	for(int i=0; i < names->len; i++){
		//\t를 사용해야 예ㅡ쁘게 맞춰서 출력
		printf("%s\t%c\t%d", names->data[i].name, names->data[i].sex, names->data[i].total_freq);
		for(int j =0; j < num_year; j++){
			printf("\t%d", names->data[i].freq[j]);
		}
		printf("\n");
	}
};


// bsearch, qsort를 위한 비교 함수
// 정렬 기준 : 이름(1순위), 성별(2순위)
int compare_by_name( const void *n1, const void *n2){
	tName *name1 = (tName *)n1; // 계속 (tName *)n1이라고 사용할 수 없으므로 name1을 만들어줌
	tName *name2 = (tName *)n2;
	
	//int num;
	
	if(!strcmp(name1->name, name2->name)){//만약 이름이 같다면
		if(name1->sex < name2->sex) return -1; // 만약 name1이 m, name2가 f라면
		else if(name1->sex > name2->sex) return 1;
		else return 0; // 성별도 같다면
	}
	else{
		return strcmp(name1->name, name2->name);
	}
	//return num;
};

// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
int compare_by_freq( const void *n1, const void *n2){
	tName *name1 = (tName *)n1;
	tName *name2 = (tName *)n2;
	
	if (name1->total_freq > name2->total_freq){
		return -1; //내림차순이므로
	}
	else if (name1->total_freq < name2->total_freq){
		return 1;
	}
	
	// 빈도가 똑같은 경우 위 함수와 똑같이
	int num;
	
	if(!strcmp(name1->name, name2->name)){//만약 이름이 같다면
		if(name1->sex > name2->sex) num = 1; //만약 name1이 m, name2가 f라면
		else if(name1->sex <= name2->sex) num = -1;
	}
	else{
		num = strcmp(name1->name, name2->name);
	}
	return num;
	
};

// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스

// 기존 비서치와는 다르게 주소를, 못찾으면 삽입될 위치를 리턴해줘야함, 발견되면 *found에 1세팅, 발견안되면 0
// 기본적으로 반토막 반토막해서 왼쪽끝을 l, 오른쪽끝을 r 
// m=(l+r)/2 l과 r이 이동하면서...고착상태에 m<l이거나, m>r이면 *found =0 후 삽입될 위치는 l값이 됨! 해봐
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found){
		//printf("호출되었습니다\n");
		int l = 0;
		int r = nmemb-1;
		while(l<=r){
			int m = (l+r)/2;
			//tName *k = (tName *)key;
			const tName *m_name = (const tName *)(base + m*size);
			if(compare(key, m_name) >0){ // 미드값보다 key가 더 크면
				l=m +1;
				
			}
			else if(compare(key, m_name)<0){
				r=m-1;
			}
			else{ //값이 같으면 찾은 거니까 
				*found =1;
				//printf("중복됩니다\n"); //현재 compare(key, m_name) =0 인값이 안나옴
				return m; //종료
			}
		}
		//만약 못찾았다면
		*found = 0;
		return l;
		
					
};

////////////////////////////////////////////////////////////////////////////////
// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1000;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
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

	// 이름 구조체 초기화
	names = create_names();
	
	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// 연도별 입력 파일(이름 정보)을 구조체에 저장
	load_names( fp, 2009, names);
	
	fclose( fp);
	
	if (option == SORT_BY_NAME) {
		// 정렬 (이름순 (이름이 같은 경우 성별순))
		qsort( names->data, names->len, sizeof(tName), compare_by_name);
	}
	else { // SORT_BY_FREQ
		// 정렬 (빈도순 (빈도가 같은 경우 이름순))
		qsort( names->data, names->len, sizeof(tName), compare_by_freq);
	}
	
	// 이름 구조체를 화면에 출력
	print_names( names, MAX_YEAR_DURATION);
	
	// 이름 구조체 해제
	destroy_names( names);
	
	return 0;
}

