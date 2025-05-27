#include <iostream>
#include <cstdarg>
#include <cstring>

class QUEUE {
    int* const elems;
    const int max;
    int head;
    int tail;
public:
    QUEUE(int m)
        : elems(new int[m]), max(m), head(0), tail(0) {}

    QUEUE(const QUEUE& q)
        : elems(new int[q.max]), max(q.max), head(q.head), tail(q.tail) {
        for (int i = 0; i < max; ++i) {
            elems[i] = q.elems[i];
        }
    }

    QUEUE(QUEUE&& q) noexcept
        : elems(q.elems), max(q.max), head(q.head), tail(q.tail) {
        *(int**)&q.elems = nullptr; // hack to modify const pointer
        *(int*)&q.max = 0;
        q.head = 0;
        q.tail = 0;
    }

    virtual int size() const {
        return max;
    }

    virtual int number() const {
        return (tail - head + max) % max;
    }

    virtual QUEUE& enter(int e) {
        if ((tail + 1) % max == head) {
            std::cerr << "QUEUE is full, cannot enter " << e << std::endl;
            return *this;
        }
        elems[tail] = e;
        tail = (tail + 1) % max;
        return *this;
    }

    virtual QUEUE& enter(short n, ...) {
        va_list ap;
        va_start(ap, n);
        for (short i = 0; i < n; ++i) {
            int e = va_arg(ap, int);
            enter(e);
        }
        va_end(ap);
        return *this;
    }

    virtual QUEUE& leave(int& e) {
        if (head == tail) {
            std::cerr << "QUEUE is empty, cannot leave" << std::endl;
            return *this;
        }
        e = elems[head];
        head = (head + 1) % max;
        return *this;
    }

    virtual QUEUE& leave(int& n, int* buf) {
        int cnt = 0;
        while (cnt < n && head != tail) {
            buf[cnt++] = elems[head];
            head = (head + 1) % max;
        }
        n = cnt;
        if (cnt == 0) {
            std::cerr << "QUEUE is empty, cannot leave (batch)" << std::endl;
        }
        return *this;
    }

    virtual QUEUE& operator=(const QUEUE& q) {
        if (this == &q) return *this;
        if (max != q.max) {
            std::cerr << "QUEUE assignment failed: size mismatch" << std::endl;
            return *this;
        }
        head = q.head;
        tail = q.tail;
        for (int i = 0; i < max; ++i) {
            elems[i] = q.elems[i];
        }
        return *this;
    }

    virtual QUEUE& operator=(QUEUE&& q) noexcept {
        if (this == &q) return *this;
        // swap pointers and values
        int* tmpElems = *(int**)&elems;
        *(int**)&elems = (int*)q.elems;
        *(int**)&q.elems = tmpElems;

        int tmpMax = *(int*)&max;
        *(int*)&max = *(int*)&q.max;
        *(int*)&q.max = tmpMax;

        std::swap(head, q.head);
        std::swap(tail, q.tail);
        return *this;
    }

    virtual QUEUE& queCat(const QUEUE& q) {
        int num = q.number();
        int idx = q.head;
        for (int i = 0; i < num; ++i) {
            enter(q.elems[idx]);
            idx = (idx + 1) % q.max;
        }
        return *this;
    }

    virtual void print(char* s) const {
        std::cout << s;
        int cnt = number();
        int idx = head;
        for (int i = 0; i < cnt; ++i) {
            std::cout << elems[idx] << (i < cnt - 1 ? " " : "");
            idx = (idx + 1) % max;
        }
        std::cout << std::endl;
    }

    virtual void clear() {
        head = tail = 0;
    }

    virtual ~QUEUE() {
        delete[] elems;
    }

    friend class STACK;
};

class STACK : public QUEUE {
    QUEUE q;
public:
    STACK(int m)
        : QUEUE(m), q(m) {}

    STACK(const STACK& s)
        : QUEUE(s), q(s.q) {}

    STACK(STACK&& s) noexcept
        : QUEUE(std::move(s)), q(std::move(s.q)) {}

    int size() const override {
        return QUEUE::size() + q.size();
    }

    int number() const override {
        return QUEUE::number() + q.number();
    }

    STACK& enter(int e) override {
        if (number() >= size() - 2) {
            std::cerr << "STACK is full, cannot enter " << e << std::endl;
            return *this;
        }
        // 两个队列都当作队列，模拟栈
        // 选择非空队列作为主队列，空队列作为辅助队列
        QUEUE* primary, * aux;
        if (QUEUE::number() != 0) {
            primary = this;
            aux = &q;
        }
        else {
            primary = &q;
            aux = this;
        }
        aux->clear();
        aux->QUEUE::enter(e);
        int cnt = primary->number();
        int tmp;
        for (int i = 0; i < cnt; ++i) {
            primary->leave(tmp);
            aux->enter(tmp);
        }
        // swap roles
        if (primary == this) {
            // swap contents
            int* tmpElems = *(int**)&elems;
            *(int**)&elems = *(int**)&q.elems;
            *(int**)&q.elems = tmpElems;

            int tmpMax = *(int*)&max;
            *(int*)&max = *(int*)&q.max;
            *(int*)&q.max = tmpMax;

            std::swap(head, q.head);
            std::swap(tail, q.tail);
        }
        return *this;
    }

    STACK& enter(short n, ...) override {
        va_list ap;
        va_start(ap, n);
        for (short i = 0; i < n; ++i) {
            int e = va_arg(ap, int);
            enter(e);
        }
        va_end(ap);
        return *this;
    }

    STACK& leave(int& e) override {
        if (number() == 0) {
            std::cerr << "STACK is empty, cannot leave" << std::endl;
            return *this;
        }
        if (QUEUE::number() != 0) {
            QUEUE::leave(e);
        }
        else {
            q.leave(e);
        }
        return *this;
    }

    STACK& leave(int& n, int* buf) override {
        int cnt = 0;
        while (cnt < n && number() > 0) {
            leave(buf[cnt]);
            ++cnt;
        }
        n = cnt;
        if (cnt == 0) {
            std::cerr << "STACK is empty, cannot leave (batch)" << std::endl;
        }
        return *this;
    }

    STACK& operator=(const STACK& s) {
        if (this == &s) return *this;
        QUEUE::operator=(s);
        q = s.q;
        return *this;
    }

    STACK& operator=(STACK&& s) noexcept {
        if (this == &s) return *this;
        QUEUE::operator=(std::move(s));
        q = std::move(s.q);
        return *this;
    }

    void print(char* s) const override {
        std::cout << s;
        // 打印顺序：从栈底到栈顶
        // 栈底在主队列head，栈顶在主队列tail前一个
        const QUEUE* primary;
        if (QUEUE::number() != 0)
            primary = this;
        else
            primary = &q;
        int cnt = primary->number();
        int idx = primary->head;
        for (int i = 0; i < cnt; ++i) {
            std::cout << primary->elems[idx];
            if (i < cnt - 1) std::cout << " ";
            idx = (idx + 1) % primary->max;
        }
        std::cout << std::endl;
    }

    void clear() override {
        QUEUE::clear();
        q.clear();
    }

    ~STACK() {}
};

int main() {
    std::cout << "----栈基本功能测试----" << std::endl;
    STACK s(10); // 容量为18（2*10-2=18），但实际最多存18个元素
    int e;

    // 单个入栈
    s.enter(1).enter(2).enter(3);
    s.print((char*)"当前栈: ");

    // 单个出栈
    s.leave(e);
    std::cout << "弹出元素: " << e << std::endl;
    s.print((char*)"当前栈: ");

    // 批量入栈
    s.enter((short)4, 4, 5, 6, 7);
    s.print((char*)"批量入栈后: ");

    // 批量出栈
    int buf[10];
    int n = 3;
    s.leave(n, buf);
    std::cout << "批量弹出: ";
    for (int i = 0; i < n; ++i) std::cout << buf[i] << " ";
    std::cout << std::endl;
    s.print((char*)"当前栈: ");

    // 清空
    s.clear();
    s.print((char*)"清空后: ");

    // 错误处理：弹空栈
    s.leave(e);

    // 错误处理：压满栈
    for (int i = 0; i < 20; ++i) s.enter(i + 100);
    s.print((char*)"压满后: ");

    // 拷贝构造
    STACK s2 = s;
    s2.print((char*)"拷贝构造: ");

    // 移动构造
    STACK s3 = std::move(s2);
    s3.print((char*)"移动构造: ");

    // 深拷贝赋值
    STACK s4(10);
    s4 = s3;
    s4.print((char*)"深拷贝赋值: ");

    // 移动赋值
    STACK s5(10);
    s5 = std::move(s4);
    s5.print((char*)"移动赋值: ");

    return 0;
}