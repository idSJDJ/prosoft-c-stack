/* Type your code here, or load an example. */
#include <stdio.h>
#include <stdlib.h>
#include "cstack.h"

//эти вещи можно скрыть через файл, где они будут объявлены как "extern", но я думаю это излишне
struct _g_table * g_table;
const unsigned int size_g_t = sizeof(struct _g_table);
int stack_first_start = 0;
int stack_handler_count[STACK_HANLDER_MAX];//количество хэндлеров
//

hstack_t stack_new(void)
{
    if (stack_first_start == 0)
    {//обязываем пользователя создать стек
        stack_first_start++;
        for (int i0=0;i0 < STACK_HANLDER_MAX; i0++)
        {//освобождаем все хендлеры при первом пуске
            stack_handler_count[i0] = 0;
        }
    }

    //ищем первый попавшийся свободный хэндлер
    int i1 = 0;
    while (stack_handler_count[i1] != 0)
    {
        i1++;
        if (i1 == STACK_HANLDER_MAX)
        {//лимит стеков исчерпан
            return (-1);
        }
    }

    // выделение памяти под новый узел
    struct node * new;
    new = (struct node *)malloc(sizeof(struct node));
    if (new == NULL)
    {//нет свободной памяти
        return (-1);
    }
    new->prev = NULL;
    new->size = 0;

    //сдвигаем адрес указателя под новую "ищейку"
    g_table+= i1 * size_g_t;

    //выделяем память под "ищейку" для нового стека
    g_table = (struct _g_table *) malloc ( size_g_t );
    if (g_table == NULL)
    {//нет свободной памяти
        //возвращаем указатель на место, ведь он глобальный
        g_table-= i1 * size_g_t;
        free(new);
        return (-1);
    }
    g_table->point = new;
    g_table->size = 0;

    //возвращаем указатель на место, ведь он глобальный
    g_table-= i1 * size_g_t;

    //забиваем место за хэндлером
    stack_handler_count[i1] = 1;
    return(i1);
}

void stack_free(const hstack_t stack)
{
    if (stack_first_start == 0)
    {//не даём проникнуть в функции без создания хотя бы одного стека
        return;
    }
    if ((stack >= STACK_HANLDER_MAX) && (stack < 0))
    {//хэндлер стека вне диапазона допустимых хэндлеров
        return;
    }
    if (stack_handler_count[stack] != 1)
    {//стека нет (не создан)
        return;
    }

    //сдвигаемся к нужному месту
    g_table+= stack * size_g_t;

    //освобождаем стек по адресу в "ищейке"
    struct node * old;
    old = g_table->point;
    free(old);
    //освобождаем "ищейку"
    free(g_table);

    //возвращаем указатель на место, ведь он глобальный
    g_table-= stack * size_g_t;

    //освобождаем место хэндлера
    stack_handler_count[stack] = 0;
}

//0 - стек есть; 1 - стека нет
int stack_valid_handler(const hstack_t stack)
{
    if (stack_first_start == 0)
    {//не даём проникнуть в функции без создания хотя бы одного стека
        return 1;
    }
    if ((stack >= STACK_HANLDER_MAX) && (stack < 0))
    {//хэндлер стека вне диапазона допустимых хэндлеров
        return 1;
    }

    if (stack_handler_count[stack] == 1)
    {//стек есть
        return 0;
    } else
    {
        return 1;
    }
}

//колличество элементов стека
unsigned int stack_size(const hstack_t stack)
{
    if (stack_first_start == 0)
    {//не даём проникнуть в функции без создания хотя бы одного стека
        return 0;
    }
    if ((stack >= STACK_HANLDER_MAX) && (stack < 0))
    {//хэндлер стека вне диапазона допустимых хэндлеров
        return 0;
    }

    if (stack_handler_count[stack] != 1)
    {//стека нет (не создан)
        return 0;
    }
    //стек есть
    //сдвигаемся к нужному месту
    g_table+= stack * size_g_t;

    //записываем размер стека (количество созданных узлов всегда на 1 больше, но последний пустой)
    unsigned int out;
    out = g_table->size;

    //возвращаем указатель на место, ведь он глобальный
    g_table-= stack * size_g_t;
    return (out);
}

/*функция добавления данных из буфера в стек
*stack - хэндлер; data_in - указатель на буфер; size - размер буфера
*/
void stack_push(const hstack_t stack, const void* data_in, const unsigned int size)
{
    if (stack_first_start == 0)
    {//не даём проникнуть в функции без создания хотя бы одного стека
        return;
    }
    if ((stack >= STACK_HANLDER_MAX) && (stack < 0))
    {//хэндлер стека вне диапазона допустимых хэндлеров
        return;
    }
    if (stack_handler_count[stack] != 1)
    {//стека нет (не создан)
        return;
    }
    if ((size == 0 ) || (size > STACK_NODE_MAX_SIZE))
    {//размер данных невалидный
        return;
    }
    if (data_in == NULL)
    {//подсмотрел в тесте отправку в качества адреса нулевой указатель, у меня такого нет, но я попытаюсь...
        return;
    }

    //сдвигаемся к нужному месту
    g_table+= stack * size_g_t;

    if (g_table->size >= STACK_NODE_MAX_COUNT)
    {//превышено максимальное количество узлов в стеке (нельзя добавить новый)
        //возвращаем указатель на место, ведь он глобальный
        g_table-= stack * size_g_t;
        return;
    }

    struct node * temp, * last;
    temp = g_table->point;

    int control = 0;
    //т.к. мы пришли с корня, то ищём последний созданный узел в стеке
    while (temp->prev != NULL)
    {//
        control++;
        temp = temp->prev;
        if (control > STACK_NODE_MAX_COUNT)
        {//нет последнего узла (внутренняя ошибка, если что-то пошло не так)
            //возвращаем указатель на место, ведь он глобальный
            g_table-= stack * size_g_t;
            return;
        }
    }

    //создаём новый узел
    last = (struct node *)malloc(sizeof(struct node));
    if (last == NULL)
    {//нет свободной памяти
        //возвращаем указатель на место, ведь он глобальный
        g_table-= stack * size_g_t;
        free(last);
        return;
    }
    //выделяем память под дату (ранее была выделена память только под указатель)
    temp->data =(char *) malloc (size * sizeof(char));
    if (temp->data == NULL)
    {//нет столько места
        //возвращаем указатель на место, ведь он глобальный
        g_table-= stack * size_g_t;
        free(last);
        free(temp->data);
        return;
    }

    //когда все проверки завершены, выставляем указатели
    temp->prev = last;//теперь это предпоследний
    last->prev = NULL;//а это новый конец
    last->size = 0;
    g_table->size +=1;


    for (int i = 0; i < size; i++)
    {
        //записываем побайтно, независимо что пришло
        //я бы написал uint8_t вместо char везде, но чтобы не было проблем...
        //и ещё личное замечание - указатель "нулевой элемент" - мне недоступен,
        //считаю, что это равносильно указателю с адресом "0х0" - который условно я бракую,
        // т.к. другого "нулевого указателя" у меня нет
        //а также считаю, что доступ для ЧТЕНИЯ разрешён ко ВСЕМ ячейкам памяти
        //в противном случае, на валидацию некорректно введённого адреса нужна
        //довольно большая портянка с заранее известными ограничениями по памяти
        *(temp->data+i) =*(char*)(data_in+i);
    }
    temp->size = size;

    //возвращаем указатель на место, ведь он глобальный
    g_table-= stack * size_g_t;
}

/*функция извлечения данных
*stack - хэндлер стека; data_out - указатель на буфер; size - размер буфера.
*Returns: размер записанных данных в байтах, если соответствующий хэндлеру стек существует, или 0 в противном случае.
*/
unsigned int stack_pop(const hstack_t stack, void* data_out, const unsigned int size)
{
    if (stack_first_start == 0)
    {//не даём проникнуть в функции без создания хотя бы одного стека
        return 0;
    }
    if ((stack >= STACK_HANLDER_MAX) && (stack < 0))
    {//хэндлер стека вне диапазона допустимых хэндлеров
        return 0;
    }
    if (stack_handler_count[stack] != 1)
    {//стека нет (не создан)
        return 0;
    }
    if ((size == 0 ) || (size > STACK_NODE_MAX_SIZE))
    {//размер данных невалидный
        return 0;
    }
    if (data_out == NULL)
    {//подсмотрел в тесте отправку в качества адреса нулевой указатель, у меня такого нет, но я попытаюсь...
        return 0;
    }

    //сдвигаемся к нужному месту
    g_table+= stack * size_g_t;
    struct node * temp, * finder;
    temp = g_table->point;

    int control = 0;
    //т.к. мы пришли с корня, то ищём последний созданный узел в стеке
    while (temp->prev != NULL)
    {//
        control++;
        finder = temp;//запоминаем предыдущий узел
        temp = temp->prev;
        if (control > STACK_NODE_MAX_COUNT)
        {//нет последнего узла (внутренняя ошибка, если что-то пошло не так)
            //возвращаем указатель на место, ведь он глобальный
            g_table-= stack * size_g_t;
            return 0;
        }
    }

    if (g_table->size == 0)
    {//если у нас один голый корень, без узлов
        g_table-= stack * size_g_t;
        return 0;
    }
    if (size < finder->size)
    {//если размер буфера меньше размера данных в узле
        return 0;
    }

    //вытаскиваем данные в буфер
    for (int i = 0; i < finder->size; i++)
    {
        *(char*)(data_out+i) = *(finder->data+i);
    }

    //зачищаем узел (теперь он последний)
    finder->prev = NULL;
    finder->size = 0;
    free(finder->data);//удаляем дату, чтобы память освободить
    free(temp);//удаляем узел, который считался последним(последний узел всегда пустой и не идёт в счёт)
    g_table->size -=1;//уменьшаем стек

    return finder->size;
}

