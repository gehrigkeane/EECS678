#include <stdlib.h>
#include <stdio.h>

struct job {
	int jid;
	int arv;
	int pri;
};

int arrival_sort(const void* a, const void* b)
{
	struct job ja = *(struct job*) a;
	struct job jb = *(struct job*) b;

	return (ja.arv - jb.arv);
}//arrival_sort

int priority_sort(const void* a, const void* b)
{
	struct job ja = *(struct job*) a;
	struct job jb = *(struct job*) b;

	if (ja.pri == jb.pri)
		return arrival_sort(a, b);
	else
		return (jb.pri - ja.pri);
}//priority_sort

int main() {

	// Array to hold each process
	int i = 0;
	struct job js[7];
	js[0].jid = 5;	js[0].arv = 10;	js[0].pri = 3;
	js[1].jid = 2;	js[1].arv = 4;	js[1].pri = 0;
	js[2].jid = 7;	js[2].arv = 14;	js[2].pri = 0;
	js[3].jid = 3;	js[3].arv = 6;	js[3].pri = 1;
	js[4].jid = 1;	js[4].arv = 2;	js[4].pri = 1;
	js[5].jid = 4;	js[5].arv = 8;	js[5].pri = 2;
	js[6].jid = 6;	js[6].arv = 12;	js[6].pri = 3;

	printf("\nArrival Sort:\n");
	qsort(js, 7, sizeof(struct job), arrival_sort);
	for (i = 0; i < 7; i++)
		printf("Job: %d\tArrival: %d\tPriority: %d\n", js[i].jid, js[i].arv, js[i].pri);

	printf("\nPriority Sort:\n");
	qsort(js, 7, sizeof(struct job), priority_sort);
	for (i = 0; i < 7; i++)
		printf("Job: %d\tArrival: %d\tPriority: %d\n", js[i].jid, js[i].arv, js[i].pri);

	return 0;
}
