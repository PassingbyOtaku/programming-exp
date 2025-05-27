#include <iostream>
#include <list>
#include <stdexcept>
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
        for (int i = 0; i < max; ++i) elems[i] = q.elems[i];
    }

    QUEUE(QUEUE&& q) noexcept
        : elems(q.elems), max(q.max), head(q.head), tail(q.tail) {
        *(int**)&q.elems = nullptr;
        *(int*)&q.max = 0;
        q.head = 0;
        q.tail = 0;
    }

    virtual int size() const noexcept {
        return max;
    }

    virtual operator int() const noexcept {
        return (tail - head + max) % max;
    }

    virtual QUEUE& operator<<(int e) {
        if ((tail + 1) % max == head)
            throw std::overflow_error("QUEUE is full, cannot enter element");
        elems[tail] = e;
        tail = (tail + 1) % max;
        return *this;
    }

    virtual QUEUE& operator<<(std::list<int>& s) {
        if (s.size() == 0) return *this;
        for (int v : s) {
            *this << v;
        }
        return *this;
    }

    virtual QUEUE& operator>>(int& e) {
        if (head == tail)
            throw std::underflow_error("QUEUE is empty, cannot leave element");
        e = elems[head];
        head = (head + 1) % max;
        return *this;
    }

    virtual QUEUE& operator>>(std::list<int>& s) {
        size_t cnt = s.size();
        if (cnt == 0) cnt = 5;
        else cnt = std::min(cnt, (size_t)(int(*this)));
        s.clear();
        int tmp;
        for (size_t i = 0; i < cnt; ++i) {
            if (head == tail)
                throw std::underflow_error("QUEUE is empty, cannot leave element (batch)");
            (*this) >> tmp;
            s.push_back(tmp);
        }
        return *this;
    }

    virtual QUEUE& operator=(const QUEUE& q) {
        if (this == &q) return *this;
        if (max != q.max)
            throw std::runtime_error("QUEUE assignment failed: size mismatch");
        head = q.head;
        tail = q.tail;
        for (int i = 0; i < max; ++i) elems[i] = q.elems[i];
        return *this;
    }

    virtual QUEUE& operator=(QUEUE&& q) noexcept {
        if (this == &q) return *this;
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

    virtual void print(char* s) const {
        std::cout << s;
        int cnt = int(*this);
        int idx = head;
        for (int i = 0; i < cnt; ++i) {
            std::cout << elems[idx] << (i < cnt - 1 ? " " : "");
            idx = (idx + 1) % max;
        }
        std::cout << std::endl;
    }

    virtual void clear() noexcept {
        head = tail = 0;
    }

    virtual ~QUEUE() noexcept {
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

    int size() const noexcept override {
        return QUEUE::size() + q.size();
    }

    operator int() const noexcept override {
        return QUEUE::operator int() + q.operator int();
    }

    STACK& operator<<(int e) override {
        if (int(*this) >= size() - 2)
            throw std::overflow_error("STACK is full, cannot enter element");
        QUEUE* primary, * aux;
        if (QUEUE::operator int() != 0) {
            primary = this;
            aux = &q;
        }
        else {
            primary = &q;
            aux = this;
        }
        aux->clear();
        aux->QUEUE::operator<<(e); // 强制调用基类方法，避免递归
        int cnt = int(*primary);
        int tmp;
        for (int i = 0; i < cnt; ++i) {
            *primary >> tmp;
            aux->QUEUE::operator<<(tmp); // 同理
        }
        // swap roles
        if (primary == this) {
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

    STACK& operator<<(std::list<int>& s) override {
        if (s.size() == 0) return *this;
        for (int v : s) {
            *this << v;
        }
        return *this;
    }

    STACK& operator>>(int& e) override {
        if (int(*this) == 0)
            throw std::underflow_error("STACK is empty, cannot leave element");
        if (QUEUE::operator int() != 0)
            QUEUE::operator>>(e);
        else
            q.operator>>(e);
        return *this;
    }

    STACK& operator>>(std::list<int>& s) override {
        size_t cnt = s.size();
        if (cnt == 0) cnt = 5;
        else cnt = std::min(cnt, (size_t)(int(*this)));
        s.clear();
        int tmp;
        for (size_t i = 0; i < cnt; ++i) {
            if (int(*this) == 0)
                throw std::underflow_error("STACK is empty, cannot leave element (batch)");
            (*this) >> tmp;
            s.push_back(tmp);
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

    void print(char* s)const override {
        std::cout << s;
        const QUEUE* primary;
        if (QUEUE::operator int() != 0)
            primary = this;
        else
            primary = &q;
        int cnt = int(*primary);
        int idx = primary->head;
        for (int i = 0; i < cnt; ++i) {
            std::cout << primary->elems[idx];
            if (i < cnt - 1) std::cout << " ";
            idx = (idx + 1) % primary->max;
        }
        std::cout << std::endl;
    }

    void clear() noexcept override {
        QUEUE::clear();
        q.clear();
    }

    ~STACK() noexcept {}
};


// 测试代码
int main() {
    try {
        std::cout << "----栈基本功能测试----" << std::endl;
        STACK s(10); // 容量为18（2*10-2=18），但实际最多存18个元素
        int e;

        // 单个入栈
        s << 1 << 2 << 3;
        s.print((char*)"当前栈: ");

        // 单个出栈
        s >> e;
        std::cout << "弹出元素: " << e << std::endl;
        s.print((char*)"当前栈: ");

        // 批量入栈
        std::list<int> in = { 4, 5, 6, 7 };
        s << in;
        s.print((char*)"批量入栈后: ");

        // 批量出栈
        std::list<int> out(3); // 容量3，指定最多弹出3个
        s >> out;
        std::cout << "批量弹出: ";
        for (auto v : out) std::cout << v << " ";
        std::cout << std::endl;
        s.print((char*)"当前栈: ");

        // 批量出栈（size=0，弹出5个）
        std::list<int> out2;
        s >> out2;
        std::cout << "批量弹出(默认5个): ";
        for (auto v : out2) std::cout << v << " ";
        std::cout << std::endl;
        s.print((char*)"当前栈: ");

        // 清空
        s.clear();
        s.print((char*)"清空后: ");

        // 异常测试：弹空栈
        try {
            s >> e;
        }
        catch (const std::exception& ex) {
            std::cout << "异常捕获: " << ex.what() << std::endl;
        }

        // 异常测试：压满栈
        try {
            std::list<int> in2;
            for (int i = 0; i < 20; ++i) in2.push_back(i + 100);
            s << in2;
        }
        catch (const std::exception& ex) {
            std::cout << "异常捕获: " << ex.what() << std::endl;
        }

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
    }
    catch (const std::exception& ex) {
        std::cout << "主流程异常捕获: " << ex.what() << std::endl;
    }
    return 0;
}