#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

#define N 7               // 矩阵时间序列大小
#define M 3               // 嵌入维数
#define T 1               // 延迟时间
#define K N - (M - 1) * T // 重构分量个数

// F1 相空间重构数组，得重构分量矩阵
int function1(int X[N], int Y[K][M]);
// F2 计算出每个重构分量对应的权重值
int function2(int Y[K][M], double weight[K]);
// F3 归类每个重构分量对应的排列模式
int function3(int pattern[K][M], int Y[K][M], int links[K]);
// F4 计算每种排列模式πk的加权概率值
int function4(double poss[K], double weight[K], int links[K]);
// F5 计算时间序列X的加权排列熵WPE值
double function5(double poss[K]);

int main()
{
    int X[N] = {4, 7, 9, 10, 6, 11, 3};
    int links[K] = {0}, Y[K][M] = {0}, pattern[K][M] = {0};
    double result = 0, weight[K] = {0}, poss[K] = {0};

    function1(X, Y);
    function2(Y, weight);
    function3(pattern, Y, links);
    function4(poss, weight, links);
    result = function5(poss);
    cout << "PWE=" << setprecision(16) << result << endl;

    return 0;
}
// F1 相空间重构数组，得重构分量矩阵
int function1(int X[N], int Y[K][M]) // X[N]时间序列，Y[K][M]相空间重构分量的集合
{
    for (int i = 0; i < K; i++)
        for (int j = 0; j < M; j++)
            Y[i][j] = X[i + j * T];

    // 输出查看
    cout << "X[N]=\n";
    for (int i = 0; i < N; i++)
        cout << X[i] << " ";
    cout << "\n\nY[K][M]=\n";
    for (int i = 0; i < K; i++)
    {
        for (int j = 0; j < M; j++)
        {
            cout << Y[i][j] << " ";
        }
        cout << "\n";
    }

    return 0;
}
// F2 计算出每个重构分量对应的权重值
int function2(int Y[K][M], double weight[K]) // Y[K][M]相空间重构分量的集合，weight[K]重构分量的对应权重
{
    // 每个重构分量的平均值，计算过程值
    double avg[K];
    for (int i = 0; i < K; i++)
    {
        avg[i] = 0;
        for (int j = 0; j < M; j++)
            avg[i] += Y[i][j];
        avg[i] /= M;
    }
    // 每个重构分量的权重值
    for (int i = 0; i < K; i++)
    {
        weight[i] = 0;
        for (int j = 0; j < M; j++)
            weight[i] += (Y[i][j] - avg[i]) * (Y[i][j] - avg[i]);
        weight[i] /= M;
    }

    // 输出查看
    cout << "\navg[K]=\n";
    for (int i = 0; i < K; i++)
        cout << avg[i] << " ";
    cout << "\n\nweight[K]=\n";
    for (int i = 0; i < K; i++)
        cout << weight[i] << " ";
    cout << "\n\n";

    return 0;
}
// F3 归类每个重构分量对应的排列模式
int function3(int pattern[K][M], int Y[K][M], int links[K]) // pattern[K][M]排列模式，Y[K][M]相空间重构分量的集合，links[K]重构分量与排列模式对应关系
{
    int k = 0;   // 重构分量计数
    int row = 0; // 排列模式计数
    while (k < K)
    {
        // 复制一份重构分量，便于排序处理；准备序号数组，记录排列模式
        int raw[M], order[M]; // raw[M]复制分量，order[M]排序后对应序号
        for (int i = 0; i < M; i++)
        {
            raw[i] = Y[k][i];
            order[i] = i;
        }
        // 对复制的重构分量排序（冒泡排序），并记录改变后序号
        for (int i = 0; i < M - 1; i++)
        {
            for (int j = 0; j < M - 1 - i; j++)
            {
                if (raw[j] > raw[j + 1])
                {
                    // 重构分量排序
                    int temp1 = raw[j];
                    raw[j] = raw[j + 1];
                    raw[j + 1] = temp1;
                    // 序号同步排序
                    int temp2 = order[j];
                    order[j] = order[j + 1];
                    order[j + 1] = temp2;
                }
            }
        }

        // 输出查看
        cout << "第" << k << "个重构分量排列模式"
             << ": ";
        for (int i = 0; i < M; i++)
            cout << order[i] << " ";
        cout << "\n";

        // 判断是否已存在相同排列模式，对排列模式归类
        int flag = 1; // 1代表不存在，0代表已存在
        for (int i = 0; i < row; i++)
        {
            int cnt = 0;
            for (int j = 0; j < M; j++)
            {
                if (pattern[i][j] == order[j])
                    cnt++;
                else
                {
                    cnt = 0;
                    break;
                }
            }
            if (cnt == M) // 如果已存在，记录重构分量对应模式
            {
                flag = 0;
                links[k] = i;
                break;
            }
        }
        // 未存在则对排列模式数组添加新模式，并记录重构分量对应模式
        if (flag)
        {
            for (int i = 0; i < M; i++)
                pattern[row][i] = order[i];
            links[k] = row;
            row++;
        }
        k++;
    }

    // 输出查看
    cout << "\npattern[K][M]=\n";
    for (int i = 0; i < row; i++)
    {
        cout << "[" << i << "]  ";
        for (int j = 0; j < M; j++)
        {
            cout << pattern[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\nlinks[K]=\n";
    for (int i = 0; i < K; i++)
    {
        cout << "重构分量" << i << ": pattern[" << links[i] << "]\n";
    }
    cout << "\n";

    return 0;
}
// F4 计算每种排列模式πk的加权概率值
int function4(double poss[K], double weight[K], int links[K]) // poss[K]排列模式的加权概率值，weight[K]重构分量的对应权重，links[K]重构分量与排列模式对应关系
{
    double sum = 0;
    for (int i = 0; i < K; i++)
    {
        sum += weight[i];
        poss[i] = 0;
    }
    for (int i = 0; i < K; i++)
    {
        poss[links[i]] += weight[i];
    }
    for (int i = 0; i < K; i++)
    {
        poss[i] = poss[i] / sum;
    }

    // 输出查看
    cout << "sum=" << sum << "\n";
    cout << "\nposs[K]=\n";
    for (int i = 0; i < K; i++)
    {
        if (poss[i] != 0)
            cout << i << ": " << poss[i] << "\n";
    }
    cout << "\n";

    return 0;
}
// F5 计算时间序列X的加权排列熵WPE值
double function5(double poss[K]) // poss[K]排列模式的加权概率值
{
    // 此处选用参考的python文件log2，而非论文中ln
    double res = 0;
    for (int i = 0; i < K; i++)
    {
        if (poss[i] != 0)
            res -= poss[i] * log(poss[i]) / log(2);
    }
    // 归一化处理，除以log2(维数阶乘)
    int factorial = 1;
    for (int i = M; i >= 1; i--)
    {
        factorial *= i;
    }
    res = res / (log(factorial) / log(2));
    return res;
}
