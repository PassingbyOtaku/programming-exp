#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <exception>
#include <typeinfo>
#include <string.h>
using namespace std;

template <typename T>
class MAT {
    T* const e;
    const int r, c;
public:
    // 构造函数
    MAT(int r_, int c_) : e(new T[r_ * c_]()), r(r_), c(c_) {}

    // 拷贝构造
    MAT(const MAT& a) : e(new T[a.r * a.c]), r(a.r), c(a.c) {
        for (int i = 0; i < r * c; ++i) e[i] = a.e[i];
    }

    // 移动构造
    MAT(MAT&& a) noexcept : e(a.e), r(a.r), c(a.c) {
        *(T**)&a.e = nullptr; // hack: 允许指针为nullptr
        *(int*)&a.r = 0;
        *(int*)&a.c = 0;
    }

    // 析构
    virtual ~MAT() noexcept {
        delete[] e;
    }

    // 下标运算符: 取r行首地址，越界抛异常
    virtual T* const operator[](int row) {
        if (row < 0 || row >= r)
            throw std::out_of_range("行下标越界");
        return e + row * c;
    }

    // 加法
    virtual MAT operator+(const MAT& a) const {
        if (r != a.r || c != a.c) throw std::invalid_argument("矩阵加法维度不符");
        MAT res(r, c);
        for (int i = 0; i < r * c; ++i) res.e[i] = e[i] + a.e[i];
        return res;
    }

    // 减法
    virtual MAT operator-(const MAT& a) const {
        if (r != a.r || c != a.c) throw std::invalid_argument("矩阵减法维度不符");
        MAT res(r, c);
        for (int i = 0; i < r * c; ++i) res.e[i] = e[i] - a.e[i];
        return res;
    }

    // 乘法
    virtual MAT operator*(const MAT& a) const {
        if (c != a.r) throw std::invalid_argument("矩阵乘法维度不符");
        MAT res(r, a.c);
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < a.c; ++j) {
                T sum = 0;
                for (int k = 0; k < c; ++k) {
                    sum += (*this)[i][k] * a[k][j];
                }
                res[i][j] = sum;
            }
        }
        return res;
    }

    // 转置
    virtual MAT operator~() const {
        MAT res(c, r);
        for (int i = 0; i < r; ++i) for (int j = 0; j < c; ++j)
            res[j][i] = (*this)[i][j];
        return res;
    }

    // 赋值
    virtual MAT& operator=(const MAT& a) {
        if (this == &a) return *this;
        if (r != a.r || c != a.c) throw std::invalid_argument("赋值维度不符");
        for (int i = 0; i < r * c; ++i) e[i] = a.e[i];
        return *this;
    }

    // 移动赋值
    virtual MAT& operator=(MAT&& a) noexcept {
        if (this == &a) return *this;
        if (r != a.r || c != a.c) throw std::invalid_argument("移动赋值维度不符");
        for (int i = 0; i < r * c; ++i) e[i] = a.e[i];
        return *this;
    }

    // +=
    virtual MAT& operator+=(const MAT& a) {
        if (r != a.r || c != a.c) throw std::invalid_argument("+=维度不符");
        for (int i = 0; i < r * c; ++i) e[i] += a.e[i];
        return *this;
    }
    // -=
    virtual MAT& operator-=(const MAT& a) {
        if (r != a.r || c != a.c) throw std::invalid_argument("-=维度不符");
        for (int i = 0; i < r * c; ++i) e[i] -= a.e[i];
        return *this;
    }
    // *=
    virtual MAT& operator*=(const MAT& a) {
        *this = *this * a;
        return *this;
    }

    // 打印
    virtual char* print(char* s) const noexcept {
        s[0] = 0; // 清空
        char buf[128];
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                if constexpr (std::is_integral<T>::value) {
                    sprintf(buf, "%6lld", (long long)(*this)[i][j]);
                }
                else if constexpr (std::is_floating_point<T>::value) {
                    sprintf(buf, "%8lf", (double)(*this)[i][j]);
                }
                strcat(s, buf);
                if (j != c - 1) strcat(s, " ");
            }
            strcat(s, "\n");
        }
        std::cout << s;
        return s;
    }

    // 行数
    int rows() const { return r; }
    // 列数
    int cols() const { return c; }
};


// 扩展main函数，全面测试
int main(int argc, char* argv[]) {
    MAT<int> a(1, 2), b(2, 2), c(1, 2);
    char t[2048];

    a[0][0] = 1; a[0][1] = 2;
    a.print(t);

    b[0][0] = 3; b[0][1] = 4;
    b[1][0] = 5; b[1][1] = 6;
    b.print(t);

    // 乘法
    c = a * b;
    c.print(t);

    // 加法
    (a + c).print(t);

    // 减法
    c = c - a;
    c.print(t);

    // +=
    c += a;
    c.print(t);

    // 转置
    c = ~a;
    c.print(t);

    // -=
    try {
        c -= a;
        c.print(t);
    }
    catch (const std::exception& ex) {
        std::cout << "异常: " << ex.what() << std::endl;
    }

    // *=
    try {
        MAT<int> d(2, 2);
        d[0][0] = 1; d[0][1] = 2; d[1][0] = 3; d[1][1] = 4;
        d.print(t);
        d *= d;
        d.print(t);
    }
    catch (const std::exception& ex) {
        std::cout << "异常: " << ex.what() << std::endl;
    }

    // 类型测试：long long
    MAT<long long> m1(2, 2), m2(2, 2);
    m1[0][0] = 10000000000LL; m1[0][1] = 2;
    m1[1][0] = 3; m1[1][1] = 4;
    m2[0][0] = 5; m2[0][1] = 6;
    m2[1][0] = 7; m2[1][1] = 8;
    m1.print(t);
    m2.print(t);
    MAT<long long> m3 = m1 * m2;
    m3.print(t);

    // 类型测试：float
    MAT<float> f1(2, 2), f2(2, 2);
    f1[0][0] = 1.1f; f1[0][1] = 2.2f;
    f1[1][0] = 3.3f; f1[1][1] = 4.4f;
    f2[0][0] = 5.5f; f2[0][1] = 6.6f;
    f2[1][0] = 7.7f; f2[1][1] = 8.8f;
    f1.print(t);
    f2.print(t);
    MAT<float> f3 = f1 * f2;
    f3.print(t);

    // 错误测试
    try {
        MAT<int> bad1(2, 3), bad2(4, 2);
        bad1 + bad2;
    }
    catch (const std::exception& ex) {
        std::cout << "异常: " << ex.what() << std::endl;
    }
    try {
        MAT<int> bad1(2, 3), bad2(4, 2);
        bad1* bad2;
    }
    catch (const std::exception& ex) {
        std::cout << "异常: " << ex.what() << std::endl;
    }
    try {
        MAT<int> bad1(2, 3), bad2(2, 2);
        bad1 = bad2;
    }
    catch (const std::exception& ex) {
        std::cout << "异常: " << ex.what() << std::endl;
    }

    return 0;
}