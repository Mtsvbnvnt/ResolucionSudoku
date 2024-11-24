#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <json/json.h>
#include <omp.h>
#include <chrono> // Para medir tiempo de ejecución

// Leer el tablero desde un archivo JSON
std::vector<std::vector<int>> readSudoku(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        exit(1);
    }

    Json::Value root;
    file >> root;

    std::vector<std::vector<int>> board;
    for (const auto& row : root["board"]) {
        std::vector<int> boardRow;
        for (const auto& cell : row)
            boardRow.push_back(cell.asInt());
        board.push_back(boardRow);
    }

    return board;
}

// Escribir el tablero resuelto en un archivo JSON
void writeSudoku(const std::vector<std::vector<int>>& board, const std::string& filename) {
    Json::Value root;
    Json::Value jsonBoard(Json::arrayValue);

    for (const auto& row : board) {
        Json::Value jsonRow(Json::arrayValue);
        for (const auto& cell : row) {
            jsonRow.append(cell);
        }
        jsonBoard.append(jsonRow);
    }

    root["solved_board"] = jsonBoard;

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  "; // Para formatear con espacios y saltos de línea

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo escribir el archivo: " << filename << std::endl;
        exit(1);
    }

    file << Json::writeString(writer, root); // Escritura formateada
    file.close();
}

// Imprimir el tablero
void printBoard(const std::vector<std::vector<int>>& board) {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}

// Verificar si un número puede colocarse en una celda
bool isValid(const std::vector<std::vector<int>>& board, int row, int col, int num, int size) {
    int blockSize = std::sqrt(size);

    for (int x = 0; x < size; x++) {
        if (board[row][x] == num || board[x][col] == num) {
            return false;
        }
    }

    int startRow = (row / blockSize) * blockSize;
    int startCol = (col / blockSize) * blockSize;

    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            if (board[startRow + i][startCol + j] == num) {
                return false;
            }
        }
    }

    return true;
}

// Resolver el Sudoku utilizando backtracking
bool solveSudoku(std::vector<std::vector<int>>& board, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (board[row][col] == 0) {
                for (int num = 1; num <= size; num++) {
                    if (isValid(board, row, col, num, size)) {
                        board[row][col] = num;

                        if (solveSudoku(board, size)) {
                            return true;
                        }

                        board[row][col] = 0; // Backtrack
                    }
                }
                return false; // No hay solución
            }
        }
    }
    return true; // Tablero resuelto
}

// Resolver el Sudoku en paralelo utilizando OpenMP
bool parallelSolveSudoku(std::vector<std::vector<int>>& board, int size) {
    bool solved = false;

    #pragma omp parallel shared(solved)
    {
        #pragma omp single
        {
            solved = solveSudoku(board, size);
        }
    }

    return solved;
}

int main() {
    std::string inputFile;
    std::string outputFile;

    // Solicitar al usuario la ubicación del archivo JSON de entrada
    std::cout << "Ingrese la ubicación del archivo JSON de entrada (por ejemplo, sudoku.json): ";
    std::getline(std::cin, inputFile); // Captura la entrada del usuario

    // Validar si el archivo existe
    std::ifstream testFile(inputFile);
    if (!testFile.is_open()) {
        std::cerr << "El archivo especificado no existe o no se puede abrir: " << inputFile << std::endl;
        return 1;
    }
    testFile.close();

    // Pedir al usuario el nombre del archivo de salida
    std::cout << "Ingrese el nombre del archivo JSON de salida (por ejemplo, solved_sudoku.json): ";
    std::getline(std::cin, outputFile); // Captura completa del nombre del archivo

    // Leer el tablero desde el archivo JSON
    std::vector<std::vector<int>> board = readSudoku(inputFile);
    int size = board.size(); // Tamaño del tablero

    // Validar que el tamaño sea válido
    if (size != 9 && size != 16 && size != 25) {
        std::cerr << "El tamaño del tablero no es válido. Debe ser 9x9, 16x16 o 25x25." << std::endl;
        return 1;
    }

    // Mostrar información de paralelismo
    std::cout << "Número máximo de hilos disponibles: " << omp_get_max_threads() << std::endl;

    // Mostrar el tablero inicial
    std::cout << "Tablero inicial:" << std::endl;
    printBoard(board);

    // Medir el tiempo de ejecución
    auto start = std::chrono::high_resolution_clock::now();

    // Resolver el Sudoku en paralelo
    if (parallelSolveSudoku(board, size)) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "\nTablero resuelto:" << std::endl;
        printBoard(board);

        // Guardar el tablero resuelto en un archivo JSON
        writeSudoku(board, outputFile);

        std::cout << "El tablero resuelto se ha guardado en: " << outputFile << std::endl;
        std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos\n";
    } else {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "\nNo se pudo resolver el Sudoku." << std::endl;
        std::cout << "Tiempo de ejecución: " << elapsed.count() << " segundos\n";
    }

    return 0;
}
