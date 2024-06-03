#include "api.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  size_t state_size = api.open();

  void *game_state = malloc(state_size);

  printf("opened api and alloced %ld", state_size);

  api.init(game_state);

  while (api.update(game_state)) {
    api.render(game_state);
  }

  api.close();
  return 0;
}
