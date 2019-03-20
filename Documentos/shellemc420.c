#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int _cd(char **args);
int _help(char **args);
int exit_(char **args);

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "mkdir",
  "chown",
  "rm",
  "cat",
  "grep",
  "head",
  "tail",
  "clear",
  "history"
};

int (*builtin_func[]) (char **) = {
  &_cd,
  &_help,
  &exit_
};

int _num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int _cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Erro: Argumentos esperados para \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("Erro ao executar\n");
    }
  }
  return 1;
}

int _help(char **args)
{
  int i;
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf(" Prof: Petrônio Júnior\n");
		printf(" Lista de exercícios 01\n");
		printf(" Sistemas Operacionais\n");
		printf("        Grupo\n");
		printf(" William Pessoa-181080833\n");
		printf(" Igor Jales-181080157\n");
		printf("         2019.1\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

  for (i = 0; i < _num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  printf("Digite o nome do comando que deseja usar, seguido de seus parâmetros.\n");
  return 1;
}

int exit_(char **args)
{
  return 0;
}

int _launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {

    if (execvp(args[0], args) == -1) {
      perror("Erro ao executar\n");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {

    perror("Erro ao executar\n");
  } else {

    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int _execute(char **args)
{
  int i;

  if (args[0] == NULL) {

    return 1;
  }

  for (i = 0; i < _num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return _launch(args);
}

#define _RL_BUFSIZE 1024

char *_read_line(void)
{
  int bufsize = _RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "Erro ao alocar memória\n");
    exit(EXIT_FAILURE);
  }

  while (1) {

    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;


    if (position >= bufsize) {
      bufsize += _RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define _TOK_BUFSIZE 64
#define _TOK_DELIM " \t\r\n\a"

char **_split_line(char *line)
{
  int bufsize = _TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "Erro ao alocar memória\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, _TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += _TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, _TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void _loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("--> ");
    line = _read_line();
    args = _split_line(line);
    status = _execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  _loop();

  return EXIT_SUCCESS;
}


