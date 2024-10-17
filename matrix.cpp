#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdlib> // Для функции std::abs

// Класс для хранения полной матрицы
class MatrixDiagonal {
private:
    std::vector<std::vector<int>> data; // Двумерный массив для хранения элементов матрицы
    size_t rows, cols;                  // Количество строк и столбцов

public:
    // Конструктор по умолчанию
    MatrixDiagonal(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(r, std::vector<int>(c, 0)); // Инициализация нулями
    }

    // Метод для задания значения элемента
    void SetValue(size_t row, size_t col, int value) {
        if (row < rows && col < cols) {
            data[row][col] = value;
        }
    }

    // Метод для получения значения элемента
    int GetValue(size_t row, size_t col) const {
        if (row < rows && col < cols) {
            return data[row][col];
        }
        return 0;
    }

    // Метод для вывода матрицы на экран
    void Print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << std::setw(4) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    // Метод для получения количества строк
    size_t GetRowsNum() const {
        return rows;
    }

    // Метод для получения количества столбцов
    size_t GetColsNum() const {
        return cols;
    }
};

// Класс для хранения экономного представления матрицы (только диагональные элементы)
class MatrixFunc {
private:
    size_t rows, cols;                             // Размеры матрицы
    std::map<int, std::vector<int>> diagonals;     // Хранилище для ненулевых диагоналей

public:
    // Конструктор
    MatrixFunc(size_t r, size_t c) : rows(r), cols(c) {}

    // Метод для задания значения элемента (только для диагоналей)
    void SetDiagonalValue(int diagonal, size_t index, int value) {
        // Преобразуем отрицательный индекс диагонали в беззнаковый тип size_t
        size_t diagonal_abs = static_cast<size_t>(std::abs(diagonal));
        if (diagonal >= -static_cast<int>(rows) + 1 && diagonal <= static_cast<int>(cols) - 1) {
            diagonals[diagonal].resize(std::min(rows, cols) - diagonal_abs, 0);
            diagonals[diagonal][index] = value;
        }
    }

    // Метод для получения значения элемента
    int GetValue(size_t row, size_t col) const {
        int diagonal = static_cast<int>(col) - static_cast<int>(row);
        if (diagonals.find(diagonal) != diagonals.end()) {
            size_t index = (diagonal >= 0) ? row : col; // Определяем индекс в массиве диагонали
            return diagonals.at(diagonal)[index];
        }
        return 0;
    }

    // Метод для вывода матрицы на экран
    void Print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << std::setw(4) << GetValue(i, j) << " ";
            }
            std::cout << std::endl;
        }
    }

    // Метод для преобразования полной матрицы в экономный формат
    void FromMatrix(const MatrixDiagonal &matrix) {
        rows = matrix.GetRowsNum();
        cols = matrix.GetColsNum();
        diagonals.clear();
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                int value = matrix.GetValue(i, j);
                if (value != 0) {
                    int diagonal = static_cast<int>(j) - static_cast<int>(i);
                    SetDiagonalValue(diagonal, (diagonal >= 0) ? i : j, value);
                }
            }
        }
    }

    // Метод для получения количества строк
    size_t GetRowsNum() const {
        return rows;
    }

    // Метод для получения количества столбцов
    size_t GetColsNum() const {
        return cols;
    }
};

// Функция для тестирования работы классов
int main() {
    // Создание полной матрицы и инициализация
    MatrixDiagonal fullMatrix(5, 5);
    fullMatrix.SetValue(0, 0, 1);
    fullMatrix.SetValue(1, 1, 2);
    fullMatrix.SetValue(2, 2, 3);
    fullMatrix.SetValue(3, 3, 4);
    fullMatrix.SetValue(4, 4, 5);
    fullMatrix.SetValue(0, 4, 9); // Параметр не на диагонали, для проверки экономии

    std::cout << "Полная матрица:\n";
    fullMatrix.Print();

    // Создание экономной матрицы и преобразование из полной
    MatrixFunc diagMatrix(5, 5);
    diagMatrix.FromMatrix(fullMatrix);

    std::cout << "\nЭкономная матрица (диагональные элементы):\n";
    diagMatrix.Print();

    return 0;
}
