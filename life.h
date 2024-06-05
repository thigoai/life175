#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <unordered_map>
#include "canvas.h"
#include "common.h"
#include "lodepng.h"

#define ALIVE '0'
#define DEAD '1'

struct Index
{
  size_t row;
  size_t col;

  Index() = default;
  Index(size_t row, size_t col) : row(row), col(col) {}

  bool is_in_range(const Index& max) const
  {
    return row < max.row && col < max.col;
  }
};

// Função para gerar um vetor com uma faixa de valores
std::vector<size_t> range(size_t end)
{
  std::vector<size_t> res(end);
  for (size_t i = 0; i < end; ++i) res.at(i) = i;
  return res;
}

// Classe Life para representar o grid do Jogo da Vida
template <typename T>
class Life
{
private:
  std::vector<T> m_data;
  size_t m_rows;
  size_t m_cols;

public:
  Life() : m_rows(0), m_cols(0) {}

  Life(size_t rows, size_t cols, T initial_value) :
    m_data(cols * rows, initial_value),
    m_rows(rows),
    m_cols(cols) {}

  Life(size_t rows, size_t cols, T(initializer)(void)) :
    m_data(cols * rows),
    m_rows(rows),
    m_cols(cols)
  {
    for (auto& e : m_data) e = initializer();
  }

  Life(size_t rows, size_t cols, T(initializer)(size_t, size_t)) :
    m_data(cols * rows),
    m_rows(rows),
    m_cols(cols)
  {
    for (auto row : this->rows())
      for (auto col : this->cols())
        at(row, col) = initializer(row, col);
  }

  Life& operator=(const Life& rhs)
  {
    if (this != &rhs)
    {
      if (m_rows != rhs.m_rows || m_cols != rhs.m_cols) {
        m_rows = rhs.m_rows;
        m_cols = rhs.m_cols;
        m_data.resize(m_rows * m_cols);
      }

      for (size_t row : this->rows())
      {
        for (size_t col : this->cols())
        {
          m_data.at(row * m_cols + col) = rhs.at(row, col);
        }
      }
    }
    return *this;
  }

  T at(size_t row, size_t col) const
  {
    return m_data.at(row * m_cols + col);
  }

  T& at(size_t row, size_t col)
  {
    return m_data.at(row * m_cols + col);
  }

  std::vector<Index> get_neighbours(size_t row, size_t col) const
  {
    std::vector<Index> result;
    for (int i = -1; i <= 1; ++i)
    {
      for (int j = -1; j <= 1; ++j)
      {
        if (i == 0 && j == 0) continue;

        Index index(row + i, col + j);
        if (index.is_in_range(max_index())) result.push_back(index);
      }
    }

    return result;
  }

  size_t row_size() const { return m_rows; }
  size_t col_size() const { return m_cols; }
  size_t total_size() const { return m_data.size(); }

  Index max_index() const { return Index(m_rows, m_cols); }

  std::vector<size_t> rows() const { return range(m_rows); }
  std::vector<size_t> cols() const { return range(m_cols); }

  bool operator==(const Life& other) const
  {
    if (this->m_rows != other.m_rows || this->m_cols != other.m_cols) {
      return false;
    }
    for (size_t row : this->rows()) {
      for (size_t col : this->cols()) {
        if (this->at(row, col) != other.at(row, col)) {
          return false;
        }
      }
    }
    return true;
  }

  bool operator!=(const Life& other) const
  {
    return !(*this == other);
  }
};

class Manager
{
private:
  std::unordered_map<std::string, Life<char>> generations;
  Life<char> life;
  Life<char> life_save;
  bool stable_state = false;
  Canvas canvas;

public:
  Manager(size_t rows, size_t cols) :
    life(rows, cols,  []() { return (rand() % 10 == 0) ? ALIVE : DEAD; }),
    life_save(rows, cols, DEAD),
    canvas(rows, cols)
  {}

  bool is_extinct() const {
    for (auto row : life.rows()) {
      for (auto col : life.cols()) {
        if (life.at(row, col) == ALIVE) {
          return false;
        }
      }
    }
    return true;
  }

  void update()
  {
    Life<char> copyLife(life);

    for (auto row : life.rows())
    {
      for (auto col : life.cols())
      {
        int neighboursAlive = 0;
        auto indexNeighbours = life.get_neighbours(row, col);
        for (auto index : indexNeighbours)
        {
          if (copyLife.at(index.row, index.col) == ALIVE) {
            neighboursAlive++;
          }
        }
        if (copyLife.at(row, col) == ALIVE)
        {
          if (neighboursAlive < 2 || neighboursAlive > 3)
          {
            life.at(row, col) = DEAD;
          }
        }
        else
        {
          if (neighboursAlive == 3)
          {
            life.at(row, col) = ALIVE;
          }
        }
      }
    }
  }

  void render()
  {
    Color green = {0, 255, 0, 255};   // Verde
    Color black = {0, 0, 0, 255};     // Preto

    for (auto row : life.rows())
    {
      for (auto col : life.cols())
      {
        if (life.at(row, col) == ALIVE) {
          canvas.setPixel(row, col, green);
        } else {
          canvas.setPixel(row, col, black);
        }
      }
    }
    canvas.print();
  }

  void encode_png(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
    unsigned error = lodepng::encode(filename, image, width, height);

    if (error != 0U) {
      std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
  }

  void save_generation_image(int generation_count)
  {
    std::string folder_path = "gens/";
    std::string filename = folder_path + "geracao_" + std::to_string(generation_count) + ".png";

    // Obter os dados da imagem diretamente do Canvas
    const std::vector<unsigned char>& image = canvas.getPixelData();

    // Codificar a imagem
    encode_png(filename.c_str(), image.data(), canvas.Width(), canvas.Height());
  }

  void run()
  {
    int gen = 1;
    while (!stable_state)
    {
      std::cout << "Geração: " << gen << std::endl;
      life_save = life;
      render();
      save_generation_image(gen);
      std::cout << std::endl;

      std::string key = generateKey(life);

      if (generations.find(key) != generations.end()) {
        stable_state = true;
        break;
      }

      generations[key] = life;

      update();
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      gen++;
      stable_state = (life == life_save);
    }
    if (is_extinct())
    {
      std::cout << "A geração " << gen - 1  << " foi extinta :(" << std::endl;
      render();
      std::cout << "Todas as células estão mortas." << std::endl;
    }
    else if(stable_state)
    {
      std::cout << "A geração " << gen - 1 << " atingiu um estado estável." << std::endl;
      render();
    }
  }

  std::string generateKey(const Life<char>& life) const
  {
    std::string key;
    for (auto row : life.rows())
    {
      for (auto col : life.cols())
      {
        key += life.at(row, col);
      }
    }
    return key;
  }
};