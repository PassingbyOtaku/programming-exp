#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdarg>
#include <cstring>
using namespace std;

class QUEUE {
    int* const elems;  // 存储队列元素的数组
    const int max;      // 队列最大容量
    int head;           // 队首指针
    int tail;           // 队尾指针

public:
    // 构造函数
    QUEUE(int m) : elems(new int[m]), max(m), head(0), tail(0) {
        if (m <= 0) {
            cerr << "Error: max size must be positive." << endl;
            exit(1);
        }
    }

    // 深拷贝构造函数
    QUEUE(const QUEUE& q) : elems(new int[q.max]), max(q.max), head(q.head), tail(q.tail) {
        if (head != tail) {
            int** temp = const_cast<int**>(&elems);
            for (int i = head; i != tail; i++) {
                if (i == max)
                    i = 0;
                if (i == tail)
                    break;
                if ((*temp + i) != nullptr)
                    *(*temp + i) = q.elems[i];
            }
        }
    }

    // 移动构造函数
    QUEUE(QUEUE&& q) noexcept : elems(nullptr), max(q.max), head(q.head), tail(q.tail) {
        *(const_cast<int*>(&max)) = q.max;
        *(const_cast<int**>(&elems)) = q.elems;

        *(const_cast<int*>(&q.max)) = 0;
        *(const_cast<int**>(&q.elems)) = nullptr;
        q.head = 0;
        q.tail = 0;
    }

    // 返回队列容量
    int queSize() const { return max; }

    // 返回当前元素个数
    int queNumber() const {
        if (max == 0)
            return 0;
        return (tail - head + max) % max; 
    }

    // 入队单个元素
    QUEUE& queEnter(int e) {
        if ((tail + 1) % max == head) {
            cerr << "Error: queue is full." << endl;
            exit(1);
        }
        elems[tail] = e;
        tail = (tail + 1) % max;
        return *this;
    }

    // 批量入队（可变参数）
    QUEUE& queEnter(short n, ...) {
        if (n <= 0) {
            cerr << "Error: n must be positive." << endl;
            exit(1);
        }
        if (queNumber() + n >= max) {
            cerr << "Error: insufficient space." << endl;
            exit(1);
        }

        va_list args;
        va_start(args, n);
        for (int i = 0; i < n; ++i) {
            int e = va_arg(args, int);
            queEnter(e);
        }
        va_end(args);
        return *this;
    }

    // 出队单个元素
    QUEUE& queLeave(int& e) {
        if (head == tail) {
            cerr << "Error: queue is empty." << endl;
            exit(1);
        }
        e = elems[head];
        head = (head + 1) % max;
        return *this;
    }

    // 批量出队到缓冲区
    QUEUE& queLeave(int& n, int* buf) {
        if (n <= 0 || buf == nullptr) {
            cerr << "Error: invalid arguments." << endl;
            exit(1);
        }
        int count = min(n, queNumber());
        for (int i = 0; i < count; ++i) {
            buf[i] = elems[head];
            head = (head + 1) % max;
        }
        n = count;
        return *this;
    }

    // 深拷贝赋值
    QUEUE& operator=(const QUEUE& q) {
        if (elems != nullptr && elems != q.elems) {
            delete[] elems;
            *(const_cast<int**>(&elems)) = nullptr;
        }
        if (elems == q.elems)
            return *this;
        head = q.head;
        tail = q.tail;
        *(const_cast<int*>(&max)) = q.max;
        *(const_cast<int**>(&elems)) = new int[max];
        if (head != tail) {
            int** ptemp = const_cast<int**>(&elems);//赋值以引用不便直接修改的对象elem
            for (int i = head; i != tail; i++) {
                if (i == max)//哨兵
                    i = 0;
                if (i == tail)
                    break;
                if ((*ptemp + i) != nullptr)
                    *(*ptemp + i) = q.elems[i];
            }
        }
        return *this;
    }

    // 移动赋值
    QUEUE& operator=(QUEUE&& q) noexcept {
        if (elems!=nullptr&&elems!=q.elems) {
            delete[] elems;
            *(const_cast<int**>(&elems)) = nullptr;
        }
        if (elems == q.elems)
            return *this;
        
        head = q.head;
        tail = q.tail;
        *(const_cast<int*>(&max)) = q.max;
        *(const_cast<int**>(&elems)) = q.elems;
        *(const_cast<int*>(&q.max)) = 0;
        *(const_cast<int**>(&q.elems)) = nullptr;
        q.head = q.tail = 0;
        return *this;
    }

    // 拼接队列
    QUEUE& queCat(const QUEUE& q) {
        int required = queNumber() + q.queNumber();
        if (required >= queSize()) {
            int newSize = required + 1;  // 保证至少能容纳总和
            int* newElems = new int[newSize];
            int cnt = 0;
            // 复制当前队列元素
            while (head != tail) {
                newElems[cnt++] = elems[head];
                head = (head + 1) % max;
            }
            // 复制q的元素
            int qHead = q.head;
            while (qHead != q.tail) {
                newElems[cnt++] = q.elems[qHead];
                qHead = (qHead + 1) % q.max;
            }
            delete[] elems;
            *(const_cast<int*>(&max)) = newSize;
            *(const_cast<int**>(&elems)) = newElems;
            head = 0;
            tail = cnt;
        }
        else {
            // 直接拼接
            int qHead = q.head;
            while (qHead != q.tail) {
                queEnter(q.elems[qHead]);
                qHead = (qHead + 1) % q.max;
            }
        }
        return *this;
    }

    // 打印队列内容
    void quePrint(const char* s) const {
        cout << s << ": [";
        int current = head;
        while (current != tail) {
            cout << elems[current];
            current = (current + 1) % max;
            if (current != tail) cout << ", ";
        }
        cout << "]" << endl;
    }

    // 清空队列
    void queClear() {
        head = tail = 0;
    }

    // 析构函数
    ~QUEUE() {
        delete[] elems;
    }
};

// 测试主函数
int main() {
    // 测试构造函数和入队
    QUEUE q1(5);
    q1.queEnter(1).queEnter(2).queEnter(3);
    q1.quePrint("Queue1");

    // 测试拷贝构造函数
    QUEUE q2 = q1;
    q2.queEnter(4);
    q2.quePrint("Queue2");

    // 测试移动构造函数
    QUEUE q3 = move(q1);
    q3.quePrint("Queue3 (moved from q1)");
    q1.quePrint("q1 after move"); // q1应为空

    // 测试批量入队 有问题
    q3.queClear();
    q3.queEnter(4, 4, 2, 5, 9);
    q3.quePrint("Queue3 after batch enter");

    // 测试出队
    int e;
    q3.queLeave(e);
    cout << "Dequeued element: " << e << endl;
    q3.quePrint("Queue3 after dequeue");

    // 测试批量出队
    int buf[3] = {};
    int n = 2;
    q3.queLeave(n, buf);
    cout << "Dequeued " << n << " elements: ";
    for (int i = 0; i < n; ++i) cout << buf[i] << " ";
    cout << endl;

    // 测试队列拼接
    QUEUE q4(10);
    q4.queCat(q3);
    q4.quePrint("Queue4 after concatenation q3");
    q4.queCat(q2);
    q4.quePrint("Queue4 after concatenation q2");

    return 0;
}