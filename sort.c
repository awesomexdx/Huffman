#include "sort.h"

void quickSort(
        QueueItem * array,
        int left,
        int right)
{
    static QueueItem buff;

    if (left < right) {

        int i = left,
                j = right;

        QueueItem pivot = array[(left + right) / 2];

        while (1) {
            while (array[i].priority > pivot.priority)
                i++;

            while (array[j].priority < pivot.priority)
                j--;

            if (i > j)
                break;

            if (array[i].priority < array[j].priority) {
                buff = array[i];
                array[i] = array[j];
                array[j] = buff;
            }

            i++;
            j--;
        }

        quickSort(array, i, right);

        quickSort(array, left, j);

    }
}
