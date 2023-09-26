#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// Функция для генерации случайной матрицы размером rows x cols
std::vector<std::vector<int>> generateMatrix(int rows, int cols)
{
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = rand() % 10; // генерируем случайное число от 0 до 9
        }
    }
    return matrix;
}

// Функция для вычисления произведения Адамара двух матриц
std::vector<std::vector<int>> calculateHadamardProduct(const std::vector<std::vector<int>> &matrix1, const std::vector<std::vector<int>> &matrix2)
{
    int rows = matrix1.size();
    int cols = matrix1[0].size();
    std::vector<std::vector<int>> result(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result[i][j] = matrix1[i][j] * matrix2[i][j];
        }
    }
    return result;
}

// Функция для вычисления произведения Адамара для всех пар матриц в заданном диапазоне
void calculateHadamardProductRange(const std::vector<std::vector<std::vector<int>>> &matrices, int start, int end, std::vector<std::vector<int>> &result)
{
    int rows = matrices[0].size();
    int cols = matrices[0][0].size();
    for (int i = start; i < end; i++)
    {
        for (int j = i + 1; j < matrices.size(); j++)
        {
            std::vector<std::vector<int>> product = calculateHadamardProduct(matrices[i], matrices[j]);
            for (int k = 0; k < rows; k++)
            {
                for (int l = 0; l < cols; l++)
                {
                    result[k][l] *= product[k][l];
                }
            }
        }
    }
}

int main()
{
    int numThreads[] = {1, 2, 4, 8};
    int numMatrices = 1000;
    int matrixSize = 10;

    // Генерируем случайные матрицы
    std::vector<std::vector<std::vector<int>>> matrices(numMatrices);
    for (int i = 0; i < numMatrices; i++)
    {
        matrices[i] = generateMatrix(matrixSize, matrixSize);
    }

    // Создаем контейнер для хранения результата
    std::vector<std::vector<int>> result(matrixSize, std::vector<int>(matrixSize, 1));

    // Вычисляем произведение Адамара для каждого количества потоков
    for (int i = 0; i < sizeof(numThreads) / sizeof(int); i++)
    {
        int numThread = numThreads[i];

        // Запускаем таймер
        auto startTime = std::chrono::high_resolution_clock::now();

        // Создаем потоки и вычисляем произведение Адамара в каждом потоке
        std::vector<std::thread> threads;
        int batchSize = numMatrices / numThread;
        for (int j = 0; j < numThread; j++)
        {
            int start = j * batchSize;
            int end = (j == numThread - 1) ? numMatrices : (j + 1) * batchSize;
            threads.push_back(std::thread(calculateHadamardProductRange, std::ref(matrices), start, end, std::ref(result)));
        }

        // Дожидаемся завершения всех потоков
        for (auto &thread : threads)
        {
            thread.join();
        }

        // Останавливаем таймер и выводим время исполнения
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        std::cout << "Number of threads: " << numThread << ", Execution time: " << duration.count() << " seconds" << std::endl;
    }

    return 0;
}