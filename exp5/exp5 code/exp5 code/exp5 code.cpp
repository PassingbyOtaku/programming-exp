#define _CRT_SECURE_NO_WARNINGS
#include <iomanip>
#include <exception>
#include <typeinfo>
#include <string.h>
#include <stdexcept>
#include <iostream>
using namespace std;

template <typename T>
class MAT {
    T* const e; // 指向矩阵元素的常量指针（指向不可变，内容可变）
    const int r, c; // 常量行数和列数

public:
    // 构造函数：动态分配r*c的矩阵空间
    MAT(int r, int c) : r(r), c(c), e(new T[r * c]) {
        if (r <= 0 || c <= 0) throw invalid_argument("Invalid matrix dimensions");
        memset(e, 0, r * c * sizeof(T)); // 初始化为0
    }

    // 深拷贝构造函数
    MAT(const MAT& a) : r(a.r), c(a.c), e(new T[a.r * a.c]) {
        memcpy(e, a.e, r * c * sizeof(T));
    }

    // 移动构造函数（noexcept）
    MAT(MAT&& a) noexcept : r(a.r), c(a.c), e(a.e) {
        const_cast<T*&>(a.e) = nullptr; // 移除源对象所有权（通过const_cast修改常量指针）
    }

    // 析构函数（noexcept）
    virtual ~MAT() noexcept {
        if (e) delete[] e; // 安全删除指针
    }

    // 获取第r行首元素指针（带越界检查）
    virtual T* const operator[](int row) {
        if (row < 0 || row >= r) throw out_of_range("Row index out of range");
        return e + row * c;
    }

    // 矩阵加法（需维度相同）
    virtual MAT operator+(const MAT& a) const {
        if (r != a.r || c != a.c) throw invalid_argument("Matrix dimensions mismatch for addition");
        MAT res(r, c);
        for (int i = 0; i < r * c; ++i) res.e[i] = e[i] + a.e[i];
        return res;
    }

    // 矩阵减法（需维度相同）
    virtual MAT operator-(const MAT& a) const {
        if (r != a.r || c != a.c) throw invalid_argument("Matrix dimensions mismatch for subtraction");
        MAT res(r, c);
        for (int i = 0; i < r * c; ++i) res.e[i] = e[i] - a.e[i];
        return res;
    }

    // 矩阵乘法（需满足：this.c == a.r）
    virtual MAT operator*(const MAT& a) const {
        if (c != a.r) throw invalid_argument("Matrix dimensions mismatch for multiplication");
        MAT res(r, a.c);
        for (int i = 0; i < r; ++i) {
            for (int k = 0; k < c; ++k) {
                for (int j = 0; j < a.c; ++j) {
                    res.e[i * a.c + j] += e[i * c + k] * a.e[k * a.c + j];
                }
            }
        }
        return res;
    }

    // 矩阵转置（行列互换）
    virtual MAT operator~() const {
        MAT res(c, r);
        for (int i = 0; i < r; ++i)
            for (int j = 0; j < c; ++j)
                res.e[j * r + i] = e[i * c + j];
        return res;
    }

    // 深拷贝赋值运算符
    virtual MAT& operator=(const MAT& a) {
        if (this != &a) {
            if (r != a.r || c != a.c) throw invalid_argument("Matrix dimensions mismatch for assignment");
            memcpy(e, a.e, r * c * sizeof(T));
        }
        return *this;
    }

    // 移动赋值运算符（noexcept）
    virtual MAT& operator=(MAT&& a) noexcept {
        if (this != &a) {
            if (r != a.r || c != a.c) {
                // 维度不匹配时终止程序（noexcept不允许抛出异常）
                cerr << "Fatal: Dimension mismatch in move assignment" << endl;
                terminate();
            }
            memcpy(e, a.e, r * c * sizeof(T)); // 移动语义实现为内存拷贝（受限于const成员）
            const_cast<T*&>(a.e) = nullptr;     // 置空源对象指针
        }
        return *this;
    }

    // 矩阵加法复合赋值（需维度相同）
    virtual MAT& operator+=(const MAT& a) {
        *this = *this + a;
        return *this;
    }

    // 矩阵减法复合赋值（需维度相同）
    virtual MAT& operator-=(const MAT& a) {
        *this = *this - a;
        return *this;
    }

    // 矩阵乘法复合赋值（需满足：this.c == a.r且结果维度不变）
    virtual MAT& operator*=(const MAT& a) {
        *this = *this * a;
        return *this;
    }

    // 矩阵打印函数（noexcept）
    virtual char* print(char* s) const noexcept {
        if (e == nullptr) {
            strcpy(s, "Matrix is empty or uninitialized.\n");
            return s;
        }
        char buffer[256];
        char* pos = s;
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                const T& val = e[i * c + j];
                // 根据类型选择格式化字符串
                if (typeid(T) == typeid(int) || typeid(T) == typeid(long)) {
                    sprintf(buffer, "%6ld", static_cast<long>(val));
                }
                else if (typeid(T) == typeid(long long)) {
                    sprintf(buffer, "%6lld", static_cast<long long>(val));
                }
                else if (typeid(T) == typeid(float) || typeid(T) == typeid(double)) {
                    sprintf(buffer, "%8.2f", static_cast<double>(val));
                }
                else {
                    strcpy(buffer, "  ?  "); // 未知类型处理
                }
                strcpy(pos, buffer);
                pos += strlen(buffer);
                *pos++ = ' '; // 列间空格
            }
            *(pos - 1) = '\n'; // 行尾换行（覆盖最后一个空格）
            *pos = '\0';
        }
        return s;
    }
};

int main(int argc, char* argv[]) {
    // 测试1：int类型矩阵
    cout << "===== Testing MAT<int> =====" << endl;
    MAT<int> a(2, 2), b(2, 2), c(2, 2);
    char t[2048];

    // 初始化矩阵a: [1, 2; 3, 4]
    a[0][0] = 1; a[0][1] = 2;
    a[1][0] = 3; a[1][1] = 4;
    cout << "Matrix A:\n" << a.print(t);

    // 初始化矩阵b: [5, 6; 7, 8]
    b[0][0] = 5; b[0][1] = 6;
    b[1][0] = 7; b[1][1] = 8;
    cout << "Matrix B:\n" << b.print(t);

    // 测试矩阵加法
    c = a + b;
    cout << "A + B:\n" << c.print(t);

    // 测试矩阵减法
    c = a - b;
    cout << "A - B:\n" << c.print(t);

    // 测试矩阵乘法
    c = a * b;
    cout << "A * B:\n" << c.print(t);

    // 测试矩阵转置
    c = ~a;
    cout << "Transpose of A:\n" << c.print(t);

    // 测试复合赋值
    c = a;
    c += b;
    cout << "C += B:\n" << c.print(t);

    c -= a;
    cout << "C -= A:\n" << c.print(t);

    c *= b;
    cout << "C *= B:\n" << c.print(t);

    // 测试移动语义
    MAT<int> d = move(c);
    cout << "After move (Matrix D):\n" << d.print(t);
    cout << "Matrix C should be empty: " << (c.print(t), t) << endl;

    // 测试2：long long类型矩阵
    cout << "\n===== Testing MAT<long long> =====" << endl;
    MAT<long long> e(2, 2), f(2, 2), g(2, 2);
    e[0][0] = 10000000000LL; e[0][1] = 20000000000LL;
    e[1][0] = 30000000000LL; e[1][1] = 40000000000LL;
    f[0][0] = 5; f[0][1] = 6;
    f[1][0] = 7; f[1][1] = 8;

    cout << "Matrix E:\n" << e.print(t);
    cout << "Matrix F:\n" << f.print(t);
    g = e * f;
    cout << "E * F:\n" << g.print(t);

    return 0;
}