#include "life.h"


int main() {
  srand(time(0)); 
  // Inicializa o gerenciador do Jogo da Vida com 10x10 células
  Manager manager(20, 20);

  manager.run();

  return 0;
}