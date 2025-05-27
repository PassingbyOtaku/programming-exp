#define _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <cstring>
#include <cstdio>

struct Queue {
    int* elems;
    int max;
    int head;
    int tail;
};

// 初始化队列，分配m个元素 
void queInit(Queue* const p, int m) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queInit(int m)" << std::endl;
        return;
    }
    if (p->elems != nullptr) {
        std::cerr << "Error: Queue already initialized in queInit(int m)" << std::endl;
        return;
    }
    p->elems = new int[m];
    p->max = m;
    p->head = 0;
    p->tail = 0;
}

// 深拷贝重载构造函数
void queInit(Queue* const p, const Queue& q) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queInit(Queue q)" << std::endl;
        return;
    }
    if (p->elems != nullptr) {
        std::cerr << "Error: Queue already initialized in queInit(Queue q)" << std::endl;
        return;
    }
    p->elems = new int[q.max];
    for (int i = 0; i < q.max; ++i)
        p->elems[i] = q.elems[i];
    p->max = q.max;
    p->head = q.head;
    p->tail = q.tail;
}

// 返回队列的最大容量
int queSize(const Queue* const p) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queSize" << std::endl;
        return -1;
    }
    return p->max-1;
}

// 返回队列当前元素个数
int queNumber(const Queue* const p) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queNumber" << std::endl;
        return -1;
    }
    return (p->tail >= p->head) ? (p->tail - p->head) : (p->max - p->head + p->tail);
}

// 元素入队
Queue* const queEnter(Queue* const p, int e) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queEnter" << std::endl;
        return p;
    }
    if ((p->tail + 1) % p->max == p->head) {//队列已满
        std::cerr << "Error: Queue is full in queEnter" << std::endl;
        return p;
    }
    p->elems[p->tail] = e;
    p->tail = (p->tail + 1) % p->max;
    return p;
}

// 元素出队
Queue* const queLeave(Queue* const p, int& e) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queLeave" << std::endl;
        return p;
    }
    if (p->head == p->tail) {//队列已空
        std::cerr << "Error: Queue is empty in queLeave" << std::endl;
        return p;
    }
    e = p->elems[p->head];//将队首元素赋值给e
    p->head = (p->head + 1) % p->max;//将队列后移
    return p;
}

// 队列赋值操作
Queue* const queAssign(Queue* const p, const Queue& q) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queAssign" << std::endl;
        return p;
    }
    delete[] p->elems;
    p->elems = new int[q.max];//清空后重新分配，防止q.max过大
    for (int i = 0; i < q.max; ++i)
        p->elems[i] = q.elems[i];
    p->max = q.max;
    p->head = q.head;
    p->tail = q.tail;
    return p;
}

// 打印队列到字符串s
void quePrint(const Queue* const p, char* s) {
    if (p == nullptr || s == nullptr) {
        std::cerr << "Error: p or s is null in quePrint" << std::endl;
        return;
    }
    s[0] = '\0';//将s置空
    int current = p->head;
    while (current != p->tail) {
        char temp[20];
        sprintf_s(temp, "%d ", p->elems[current]);//用 sprintf_s 函数将当前队列元素格式化为字符串，并追加一个空格
        strcat(s, temp);//用 strcat 函数将格式化后的字符串 temp 连接到字符数组 s 的末尾
        current = (current + 1) % p->max;
    }
    if (strlen(s) > 0)
        s[strlen(s) - 1] = '\0'; // 去除末尾空格
}

// 清空队列
void queClear(Queue* const p) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queClear" << std::endl;
        return;
    }
    p->head = 0;
    p->tail = 0;
}

// 销毁队列
void queDestroy(Queue* const p) {
    if (p == nullptr) {
        std::cerr << "Error: p is null in queDestroy" << std::endl;
        return;
    }
    p->max = 0;
    p->head = 0;
    p->tail = 0;
    if (p->elems == nullptr) {
        std::cerr << "Error: p is already empty" << std::endl;
        return;
    }
    delete[] p->elems;
    p->elems = nullptr;    
}

int main() {
    Queue q1 = { nullptr, 0, 0, 0 };
    queInit(&q1, 3);//max=3，最大容量2（因为牺牲了1个容量用于检测队列是否满队）

    queEnter(&q1, 1);
    queEnter(&q1, 2);
    queEnter(&q1, 3);//应当在此弹出err
    std::cout << "Queue q1 size: " << queSize(&q1) << std::endl;//应为2
    std::cout << "Queue q1 num: " << queNumber(&q1) << std::endl;//应为2

    char buffer[100];
    quePrint(&q1, buffer);
    std::cout << "Queue q1: " << buffer << std::endl;

    int e;
    queLeave(&q1, e);
    std::cout << "Dequeued: " << e << std::endl;//应为1

    quePrint(&q1, buffer);
    std::cout << "Queue q1 after dequeue: " << buffer << std::endl;//应为2
    std::cout << "Queue q1 num: " << queNumber(&q1) << std::endl;//应为2

    Queue q2 = { nullptr, 0, 0, 0 };
    queInit(&q2, q1);//深拷贝q1
    queInit(&q2, q1);//深拷贝q1，应当报错q2不为空
    queEnter(&q2, 4);
    queEnter(&q2, 5);//应当报错队列已满
    quePrint(&q2, buffer);
    std::cout << "Queue q2: " << buffer << std::endl;//应为2 4

    queAssign(&q1, q2);
    quePrint(&q1, buffer);
    std::cout << "Queue q1 after assignment: " << buffer << std::endl;//q1此时也应为2 4

    queClear(&q1);
    queDestroy(&q2);
    quePrint(&q1, buffer);
    std::cout << "Queue q1: " << buffer << std::endl;//应为空
    queDestroy(&q2);//应报错

    return 0;
}