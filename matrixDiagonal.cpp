#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cstdlib> // Для std::abs
#include <memory>  // Для std::unique_ptr

// Базовый абстрактный класс матрицы
template <typename T>
class MatrixBase {
public:
    virtual ~MatrixBase() = default;

    virtual size_t GetRowsNum() const = 0;
    virtual size_t GetColsNum() const = 0;
    virtual T GetValue(size_t row, size_t col) const = 0;
    virtual void SetValue(size_t row, size_t col, T value) = 0;
    virtual void Print() const = 0;
    virtual void SaveToFile(const std::string &filename) const = 0;
    virtual MatrixBase<T>* operator+(const MatrixBase<T>& other) const = 0;
    virtual MatrixBase<T>* operator-(const MatrixBase<T>& other) const = 0;
    virtual MatrixBase<T>* operator*(const MatrixBase<T>& other) const = 0;
    virtual MatrixBase<T>* ElementWiseMultiply(const MatrixBase<T>& other) const = 0;
};

// Плотная (Dense) матрица
template <typename T>
class MatrixDense : public MatrixBase<T> {
private:
    std::vector<std::vector<T>> data;
    size_t rows, cols;

public:
    MatrixDense(size_t r, size_t c) : rows(r), cols(c) {
        data.resize(r, std::vector<T>(c, 0)); 
    }

    size_t GetRowsNum() const override { return rows; }
    size_t GetColsNum() const override { return cols; }
    T GetValue(size_t row, size_t col) const override { return data[row][col]; }
    void SetValue(size_t row, size_t col, T value) override { data[row][col] = value; }

    MatrixBase<T>* operator+(const MatrixBase<T>& other) const override {
        MatrixDense* result = new MatrixDense(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->SetValue(i, j, data[i][j] + other.GetValue(i, j));
            }
        }
        return result;
    }

    MatrixBase<T>* operator-(const MatrixBase<T>& other) const override {
        MatrixDense* result = new MatrixDense(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->SetValue(i, j, data[i][j] - other.GetValue(i, j));
            }
        }
        return result;
    }

    MatrixBase<T>* operator*(const MatrixBase<T>& other) const override {
        MatrixDense* result = new MatrixDense(rows, other.GetColsNum());
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < other.GetColsNum(); ++j) {
                T sum = 0;
                for (size_t k = 0; k < cols; ++k) {
                    sum += data[i][k] * other.GetValue(k, j);
                }
                result->SetValue(i, j, sum);
            }
        }
        return result;
    }

    MatrixBase<T>* ElementWiseMultiply(const MatrixBase<T>& other) const override {
        MatrixDense* result = new MatrixDense(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result->SetValue(i, j, data[i][j] * other.GetValue(i, j));
            }
        }
        return result;
    }

    void Print() const override {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << std::setw(4) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    void SaveToFile(const std::string &filename) const override {
        std::ofstream outFile(filename);
        if (!outFile) {
            throw std::runtime_error("Не удалось открыть файл для записи.");
        }
        outFile << rows << " " << cols << "\n";
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                outFile << data[i][j] << " ";
            }
            outFile << "\n";
        }
        outFile.close();
    }
      // Метод загрузки матрицы из файла
    void LoadFromFile(const std::string &filename) {
        std::ifstream inFile(filename);
        if (!inFile) {
            throw std::runtime_error("Не удалось открыть файл для чтения.");
        }

        size_t newRows, newCols;
        inFile >> newRows >> newCols;

        if (newRows == 0 || newCols == 0) {
            throw std::runtime_error("Некорректные размеры матрицы в файле.");
        }

        rows = newRows;
        cols = newCols;
        data.resize(rows, std::vector<T>(cols));

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                inFile >> data[i][j];
            }
        }

        if (inFile.fail()) {
            throw std::runtime_error("Ошибка чтения данных из файла.");
        }

        inFile.close();
    }
    
 // Вывод матрицы на экран
    void Print() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << std::setw(4) << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    
};

// Фабрика для создания матриц
template <typename T>
class MatrixFactory {
public:
    static std::unique_ptr<MatrixBase<T>> CreateMatrix(const std::string& type, size_t rows, size_t cols) {
        if (type == "dense") {
            return std::make_unique<MatrixDense<T>>(rows, cols);
        }
        else {
            throw std::invalid_argument("Неизвестный тип матрицы.");
        }
    }
};

// Функция для выбора типа матрицы пользователем
template <typename T>
std::unique_ptr<MatrixBase<T>> ChooseMatrixType() {
    std::string type;
    size_t rows, cols;
    std::cout << "Выберите тип матрицы (dense): ";
    std::cin >> type;

    std::cout << "Введите количество строк: ";
    std::cin >> rows;
    std::cout << "Введите количество столбцов: ";
    std::cin >> cols;

    return MatrixFactory<T>::CreateMatrix(type, rows, cols);
}

// Функция для выполнения операций и сохранения результата
template <typename T>
void PerformAndSaveOperation(const MatrixBase<T>& matrix1, const MatrixBase<T>& matrix2, const std::string& operation, const std::string& filename) {
    MatrixBase<T>* result = nullptr;

    if (operation == "add") {
        result = matrix1 + matrix2;
    } else if (operation == "sub") {
        result = matrix1 - matrix2;
    } else if (operation == "mul") {
        result = matrix1 * matrix2;
    } else if (operation == "elem_mul") {
        result = matrix1.ElementWiseMultiply(matrix2);
    } else {
        std::cerr << "Неизвестная операция!" << std::endl;
        return;
    }

    std::cout << "\nРезультат операции (" << operation << "):\n";
    result->Print();

    // Сохранение результата в файл
    result->SaveToFile(filename);
    std::cout << "Результат сохранен в файл: " << filename << "\n";

    delete result;
}

// Функция для ручного ввода значений матрицы
template <typename T>
void InputMatrixValues(MatrixBase<T>& matrix) {
    size_t rows = matrix.GetRowsNum();
    size_t cols = matrix.GetColsNum();
    std::cout << "Введите значения для матрицы (" << rows << "x" << cols << "):\n";
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            T value;
            std::cout << "Значение [" << i << "][" << j << "]: ";
            std::cin >> value;
            matrix.SetValue(i, j, value);
        }
    }
}

// Основная функция для выполнения операций с матрицами
int main() {
    try {
        std::cout << "Создание первой матрицы.\n";
        auto matrix1 = ChooseMatrixType<int>();
        InputMatrixValues(*matrix1);

        std::cout << "\nПервая матрица:\n";
        matrix1->Print();

        std::cout << "\nСоздание второй матрицы.\n";
        auto matrix2 = ChooseMatrixType<int>();
        InputMatrixValues(*matrix2);

        std::cout << "\nВторая матрица:\n";
        matrix2->Print();

        std::string operation;
        std::cout << "\nВыберите операцию (add, sub, mul, elem_mul): ";
        std::cin >> operation;

        std::string filename;
        std::cout << "Введите имя файла для сохранения результата: ";
        std::cin >> filename;


        PerformAndSaveOperation(*matrix1, *matrix2, operation, filename);

MatrixDense<int> matrix(5, 5);
    matrix.SetValue(0, 0, 1);
    matrix.SetValue(0, 1, 2);
    matrix.SetValue(0, 2, 3);
    matrix.SetValue(1, 0, 4);
    matrix.SetValue(1, 1, 5);
    matrix.SetValue(1, 2, 6);
    std::cout << "Загрузка матрицы из файла...\n";
    matrix.LoadFromFile(filename);

    std::cout << "Загруженная матрица:\n";
    matrix.Print();
        
    } catch (const std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }



    return 0;
}
