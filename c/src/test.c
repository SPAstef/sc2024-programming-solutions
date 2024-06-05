#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int y;
    int m;
    int d;
} date_t;

int compare_dates(const void *pa, const void *pb)
{
    date_t a = *(const date_t *)pa;
    date_t b = *(const date_t *)pb;

    if (a.y < b.y)
        return -1;
    if (a.y > b.y)
        return 1;

    if (a.m < b.m)
        return -1;
    if (a.m > b.m)
        return 1;

    if (a.d < b.d)
        return -1;
    if (a.d > b.d)
        return 1;

    return 0;
}

int main(void)
{
    size_t n = 10;
    date_t *dates = malloc(n * sizeof(date_t));

    for (size_t i = 0; i < n; ++i)
    {
        dates[i].y = rand() % 100 + 1900;
        dates[i].m = rand() % 12 + 1;
        dates[i].d = rand() % 30 + 1;
    }

    printf("Initial dates:\n");
    for (size_t i = 0; i < n; ++i)
        printf("%02d/%02d/%04d\n", dates[i].d, dates[i].m, dates[i].y);
    printf("\n");

    qsort(dates, n, sizeof(date_t), compare_dates);

    printf("Sorted dates:\n");
    for (size_t i = 0; i < n; ++i)
        printf("%02d/%02d/%04d\n", dates[i].d, dates[i].m, dates[i].y);
    printf("\n");

    free(dates);

    return 0;
}
